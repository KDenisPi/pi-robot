/*
 * StateEnvAnalize.h
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#ifndef PROJECT1_STATEENVANALIZE_H_
#define PROJECT1_STATEENVANALIZE_H_

#include <State.h>

namespace project1 {
namespace state {

class StateEnvAnalize: public smashine::state::State {
public:
	StateEnvAnalize(const std::shared_ptr<smashine::StateMashineItf> itf);
	virtual ~StateEnvAnalize();

	virtual void OnEntry() override;
	virtual bool OnTimer(const int id) override;

};

} /* namespace state */
} /* namespace project1 */

#endif /* PROJECT1_STATEENVANALIZE_H_ */
