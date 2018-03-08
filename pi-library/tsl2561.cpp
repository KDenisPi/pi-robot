/*
 * tsl2561.cpp
 * I2C Light-to-Digital Converter
 *  
 * 
 *  Note: This device can use three different addresses 0x29, 0x39 (default), 0x49 
 * 
 *  Created on: Mar 5, 2018
 *      Author: Denis Kudia
 */

#include "wiringPi.h"
#include "logger.h"
#include "I2CWrapper.h"
#include "tsl2561.h"

namespace pirobot {
namespace item {

const char TAG[] = "TSL2561";
const uint8_t Tsl2561::s_i2c_addr = 0x39;

//
//
//
Tsl2561::Tsl2561(const std::string& name, 
        const std::shared_ptr<pirobot::i2c::I2C> i2c,
        const uint8_t i2c_addr,
        const std::string& comment) :
    Item(name, comment, ItemTypes::SGP30), _i2caddr(i2c_addr) {

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Addr: " + std::to_string(_i2caddr));

    //register I2C user
    i2c->add_user(name, _i2caddr);

    I2CWrapper::lock();
    m_fd = I2CWrapper::I2CSetup(_i2caddr);
    I2CWrapper::unlock();

    //get timing value
    get_timing();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Descr: " + std::to_string(m_fd));
}

//
//
Tsl2561::~Tsl2561(){
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__));

}

//
const uint8_t Tsl2561::get_timing(){
    I2CWrapper::lock();
    _timing = I2CWrapper::I2CReadReg8(m_fd, TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING);
    I2CWrapper::unlock();

    _tsl2561IntegrationTime = get_integration_time();
    _tsl2561Gain = get_gain();
       
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timing: Integration time: " + get_integration_time_name()+ " Gain: " + get_gain_name());

    return _timing;
}

//set timing values
void  Tsl2561::set_timing(const tsl2561IntegrationTime_t integ, const tsl2561Gain_t gain, tsl2561Manual_t manual){

    _timing = (gain << 4) | (manual << 3) | integ;
    
    I2CWrapper::lock();
    I2CWrapper::I2CWriteReg8(m_fd, TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, _timing);
    I2CWrapper::unlock();

    _tsl2561IntegrationTime = get_integration_time();
    _tsl2561Gain = get_gain();
       
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Timing: Integration time: " + get_integration_time_name()+ " Gain: " + get_gain_name());
}

//get revision number
const uint8_t Tsl2561::get_id(){
    I2CWrapper::lock();
    uint8_t id = I2CWrapper::I2CReadReg8(m_fd, TSL2561_COMMAND_BIT | TSL2561_REGISTER_ID);
    I2CWrapper::unlock();

    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " ID.  Part number: " + ((id >> 4) == 0001 ? "TSL2561" : "TSL2560") + " Revision: " + std::to_string((id&0x0F)));

    return id; 
}

//read RAW sensor data
void Tsl2561::read_raw_data(){

    //switch power ON
    power_on();

    /* Wait x ms for ADC to complete */
    switch (_tsl2561IntegrationTime)
    {
        case TSL2561_INTEGRATIONTIME_13MS:
        	std::this_thread::sleep_for(std::chrono::milliseconds(TSL2561_DELAY_INTTIME_13MS));// KTOWN: Was 14ms
            break;
        case TSL2561_INTEGRATIONTIME_101MS:
           	std::this_thread::sleep_for(std::chrono::milliseconds(TSL2561_DELAY_INTTIME_13MS));// KTOWN: Was 102ms
          break;
        default:
        	std::this_thread::sleep_for(std::chrono::milliseconds(TSL2561_DELAY_INTTIME_13MS));// KTOWN: Was 403ms
          break;
    }    
    _data_vis_ir = read16(TSL2561_REGISTER_CHAN0_LOW);
    _data_ir_only = read16(TSL2561_REGISTER_CHAN1_LOW);

    //switch power ON
    power_off();
}


//Read word data with low first order
uint16_t Tsl2561::read16(const uint8_t reg){
    uint8_t buff[2];
    uint16_t result;

    I2CWrapper::lock();
    int rlen = I2CWrapper::I2CReadData(m_fd, TSL2561_COMMAND_BIT | TSL2561_BLOCK_BIT | reg, buff, 2);
    I2CWrapper::unlock();
    
    result = buff[1];
    return (result << 8) | buff[0]; 
}

//Calculate Lux from RAW values
uint32_t Tsl2561::calculateLux(uint16_t broadband, uint16_t ir)
{
  unsigned long chScale;
  unsigned long channel1;
  unsigned long channel0;

  /* Make sure the sensor isn't saturated! */
  uint16_t clipThreshold;
  switch (_tsl2561IntegrationTime)
  {
    case TSL2561_INTEGRATIONTIME_13MS:
      clipThreshold = TSL2561_CLIPPING_13MS;
      break;
    case TSL2561_INTEGRATIONTIME_101MS:
      clipThreshold = TSL2561_CLIPPING_101MS;
      break;
    default:
      clipThreshold = TSL2561_CLIPPING_402MS;
      break;
  }

  /* Return 65536 lux if the sensor is saturated */
  if ((broadband > clipThreshold) || (ir > clipThreshold))
  {
    return 65536;
  }

  /* Get the correct scale depending on the intergration time */
  switch (_tsl2561IntegrationTime)
  {
    case TSL2561_INTEGRATIONTIME_13MS:
      chScale = TSL2561_LUX_CHSCALE_TINT0;
      break;
    case TSL2561_INTEGRATIONTIME_101MS:
      chScale = TSL2561_LUX_CHSCALE_TINT1;
      break;
    default: /* No scaling ... integration time = 402ms */
      chScale = (1 << TSL2561_LUX_CHSCALE);
      break;
  }

  /* Scale for gain (1x or 16x) */
  if (!_tsl2561Gain) chScale = chScale << 4;

  /* Scale the channel values */
  channel0 = (broadband * chScale) >> TSL2561_LUX_CHSCALE;
  channel1 = (ir * chScale) >> TSL2561_LUX_CHSCALE;

  /* Find the ratio of the channel values (Channel1/Channel0) */
  unsigned long ratio1 = 0;
  if (channel0 != 0) ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE+1)) / channel0;

  /* round the ratio value */
  unsigned long ratio = (ratio1 + 1) >> 1;

  unsigned int b, m;

#ifdef TSL2561_PACKAGE_CS
  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1C))
    {b=TSL2561_LUX_B1C; m=TSL2561_LUX_M1C;}
  else if (ratio <= TSL2561_LUX_K2C)
    {b=TSL2561_LUX_B2C; m=TSL2561_LUX_M2C;}
  else if (ratio <= TSL2561_LUX_K3C)
    {b=TSL2561_LUX_B3C; m=TSL2561_LUX_M3C;}
  else if (ratio <= TSL2561_LUX_K4C)
    {b=TSL2561_LUX_B4C; m=TSL2561_LUX_M4C;}
  else if (ratio <= TSL2561_LUX_K5C)
    {b=TSL2561_LUX_B5C; m=TSL2561_LUX_M5C;}
  else if (ratio <= TSL2561_LUX_K6C)
    {b=TSL2561_LUX_B6C; m=TSL2561_LUX_M6C;}
  else if (ratio <= TSL2561_LUX_K7C)
    {b=TSL2561_LUX_B7C; m=TSL2561_LUX_M7C;}
  else if (ratio > TSL2561_LUX_K8C)
    {b=TSL2561_LUX_B8C; m=TSL2561_LUX_M8C;}
#else
  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
    {b=TSL2561_LUX_B1T; m=TSL2561_LUX_M1T;}
  else if (ratio <= TSL2561_LUX_K2T)
    {b=TSL2561_LUX_B2T; m=TSL2561_LUX_M2T;}
  else if (ratio <= TSL2561_LUX_K3T)
    {b=TSL2561_LUX_B3T; m=TSL2561_LUX_M3T;}
  else if (ratio <= TSL2561_LUX_K4T)
    {b=TSL2561_LUX_B4T; m=TSL2561_LUX_M4T;}
  else if (ratio <= TSL2561_LUX_K5T)
    {b=TSL2561_LUX_B5T; m=TSL2561_LUX_M5T;}
  else if (ratio <= TSL2561_LUX_K6T)
    {b=TSL2561_LUX_B6T; m=TSL2561_LUX_M6T;}
  else if (ratio <= TSL2561_LUX_K7T)
    {b=TSL2561_LUX_B7T; m=TSL2561_LUX_M7T;}
  else if (ratio > TSL2561_LUX_K8T)
    {b=TSL2561_LUX_B8T; m=TSL2561_LUX_M8T;}
#endif

  unsigned long temp;
  temp = ((channel0 * b) - (channel1 * m));

  /* Do not allow negative lux value */
  if (temp < 0) temp = 0;

  /* Round lsb (2^(LUX_SCALE-1)) */
  temp += (1 << (TSL2561_LUX_LUXSCALE-1));

  /* Strip off fractional portion */
  uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;

  /* Signal I2C had no errors */
  return lux;
}

/**************************************************************************/
/*!
    @brief  Gets the broadband (mixed lighting) and IR only values from
            the TSL2561, adjusting gain if auto-gain is enabled
    @param  broadband Pointer to a uint16_t we will fill with a sensor 
                      reading from the IR+visible light diode.
    @param  ir Pointer to a uint16_t we will fill with a sensor the 
               IR-only light diode.
*/
/**************************************************************************/
void Tsl2561::getLuminosity()
{
  bool valid = false;

  /* If Auto gain disabled get a single reading and continue */
  if(!_tsl2561AutoGain)
  {
    read_raw_data();
    return;
  }

  /* Read data until we find a valid range */
  bool _agcCheck = false;
  do
  {
    uint16_t _hi, _lo;
    tsl2561IntegrationTime_t _it = _tsl2561IntegrationTime;

    /* Get the hi/low threshold for the current integration time */
    switch(_it)
    {
      case TSL2561_INTEGRATIONTIME_13MS:
        _hi = TSL2561_AGC_THI_13MS;
        _lo = TSL2561_AGC_TLO_13MS;
        break;
      case TSL2561_INTEGRATIONTIME_101MS:
        _hi = TSL2561_AGC_THI_101MS;
        _lo = TSL2561_AGC_TLO_101MS;
        break;
      default:
        _hi = TSL2561_AGC_THI_402MS;
        _lo = TSL2561_AGC_TLO_402MS;
        break;
    }

    read_raw_data();

    /* Run an auto-gain check if we haven't already done so ... */
    if (!_agcCheck)
    {
      if ((_data_vis_ir < _lo) && (_tsl2561Gain == TSL2561_GAIN_1X))
      {
        /* Increase the gain and try again */
        set_gain(TSL2561_GAIN_16X);
        /* Drop the previous conversion results */
        read_raw_data();
        /* Set a flag to indicate we've adjusted the gain */
        _agcCheck = true;
      }
      else if ((_data_vis_ir > _hi) && (_tsl2561Gain == TSL2561_GAIN_16X))
      {
        /* Drop gain to 1x and try again */
        set_gain(TSL2561_GAIN_1X);
        /* Drop the previous conversion results */
        read_raw_data();
        /* Set a flag to indicate we've adjusted the gain */
        _agcCheck = true;
      }
      else
      {
        /* Nothing to look at here, keep moving ....
           Reading is either valid, or we're already at the chips limits */
        valid = true;
      }
    }
    else
    {
      /* If we've already adjusted the gain once, just return the new results.
         This avoids endless loops where a value is at one extreme pre-gain,
         and the the other extreme post-gain */
      valid = true;
    }
  } while (!valid);
}

//get results
const bool Tsl2561::get_results(uint32_t& lux){
    getLuminosity();
    lux = calculateLux(_data_vis_ir, _data_ir_only);
    logger::log(logger::LLOG::DEBUG, TAG, std::string(__func__) + " Lux: " + std::to_string(lux));

    return (lux < 65535); //if we have 65535 it seems value is overflow    
}

//Set Power On/Off
void Tsl2561::set_power(const bool on_off){
  if(_is_power_on == on_off)
      return;

  _is_power_on = on_off;

  I2CWrapper::lock();
  I2CWrapper::I2CWriteReg8(m_fd, TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, (_is_power_on ? TSL2561_CONTROL_POWERON : TSL2561_CONTROL_POWEROFF));
  I2CWrapper::unlock();
};

}//item
}//pirobot