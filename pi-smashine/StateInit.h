/*
 * StateInit.h
 *
 *  Created on: Nov 18, 2016
 *      Author: denis
 */

#ifndef PI_SMASHINE_STATEINIT_H_
#define PI_SMASHINE_STATEINIT_H_

#include "State.h"

namespace smashine {
namespace state {

class StateInit: public State {
public:
	StateInit(const std::shared_ptr<StateMashineItf> itf);
	virtual ~StateInit();

	virtual void OnEntry() override;
	virtual bool OnEvent(const std::shared_ptr<Event> event) override;
	virtual bool OnTimer(const int id) override;

};

} /* namespace state */
} /* namespace smashine */

#endif /* PI_SMASHINE_STATEINIT_H_ */
