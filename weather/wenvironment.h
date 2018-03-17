/*
 * MyEnv.h
 *
 *  Created on: Dec 8, 2016
 *      Author: Denis Kudia
 */

#ifndef WEATHER_WENV_H_
#define WEATHER_WENV_H_

#include <chrono>

#include "Environment.h"

namespace weather {

class WeatherEnv : public smachine::Environment {
public:
	WeatherEnv(){}
	virtual ~WeatherEnv() {}

	float _humidity;
	float _humidity_abs;
	float _temperature;  	//Si7021

	float _co2;
	float _tvoc;

	//save/restore this values
	float _co2_base;
	float _tvoc_base;

	float _pressure;
	float _temperature_bmp280;
	float _altitude;

	uint32_t _lux;
};

}

#endif /* WEATHER_WENV_H_ */
