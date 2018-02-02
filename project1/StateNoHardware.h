/*
 * StateNoHardware.h
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_StateNoHardware_H_
#define PROJECT1_StateNoHardware_H_

#include "State.h"

namespace project1 {
namespace state {

class StateNoHardware: public smachine::state::State {
public:
    StateNoHardware(smachine::StateMachineItf* itf);
    virtual ~StateNoHardware();

    virtual void OnEntry() override;
    virtual bool OnTimer(const int id) override;
    virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

    template<class T> std::shared_ptr<T> get_env(){
        return std::static_pointer_cast<T>(get_itf()->get_env());
    }
};

} /* namespace state */
} /* namespace project1 */

#endif /* PROJECT1_StateNoHardware_H_ */
