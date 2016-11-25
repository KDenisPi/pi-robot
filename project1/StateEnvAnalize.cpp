/*
 * StateEnvAnalize.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: denis
 */

#include "StateEnvAnalize.h"

namespace project1 {
namespace state {

StateEnvAnalize::StateEnvAnalize(const std::shared_ptr<smashine::StateMashineItf> itf, const std::shared_ptr<pirobot::PiRobot> robot) :
		smashine::state::State(itf, robot)
{
	// TODO Auto-generated constructor stub

}

StateEnvAnalize::~StateEnvAnalize() {
	// TODO Auto-generated destructor stub
}

void StateEnvAnalize::OnEntry(){

}


} /* namespace state */
} /* namespace project1 */
