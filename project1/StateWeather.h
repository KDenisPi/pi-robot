/*
 * StateWeather.h
 *
 *  Created on: Feb 02, 2018
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_StateWeather_H_
#define PROJECT1_StateWeather_H_

#include "State.h"

namespace project1 {
namespace state {

class StateWeather: public smachine::state::State {
public:
    StateWeather(smachine::StateMachineItf* itf);
    virtual ~StateWeather();

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

    template<class T> std::shared_ptr<T> get_env(){
        return std::static_pointer_cast<T>(get_itf()->get_env());
    }

    void get_spg30_values();
    void get_bmp280_values();
    void get_tsl2561_values();
    
};

} /* namespace state */
} /* namespace project1 */

#endif /* PROJECT1_StateWeather_H_ */
