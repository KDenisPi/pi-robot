/*
 * StateEnvAnalize.h
 *
 *  Created on: Nov 24, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_STATEENVANALIZE_H_
#define PROJECT1_STATEENVANALIZE_H_

#include <random>
#include "State.h"
#include "defines.h"


namespace spi_test {
namespace state {

class StateEnvAnalize: public smachine::state::State {
public:
	StateEnvAnalize(smachine::StateMachineItf* itf);
	virtual ~StateEnvAnalize();

	virtual void OnEntry() override;
	virtual bool OnTimer(const int id) override;
	virtual bool OnEvent(const std::shared_ptr<smachine::Event> event) override;

	void add_transformations();

	void ntf_finished(const std::string& event_name)
	{
		std::shared_ptr<smachine::Event> event(new smachine::Event(smachine::EVENT_TYPE::EVT_USER, (event_name.empty() ? EVT_CYCLE_FINISHED : event_name)));
		EVENT(event);

	}

private:
	std::linear_congruential_engine<std::uint_fast32_t, 16807, 0, 0x00FFFFFF> _rand;

	int _cycle_counter = -1;
};

} /* namespace state */
} /* namespace project1 */

#endif /* PROJECT1_STATEENVANALIZE_H_ */
