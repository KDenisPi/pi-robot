/*
 * PiRobotPrj1.h
 *
 *  Created on: Dec 17, 2016
 *      Author: Denis Kudia
 */

#ifndef PROJECT1_PIROBOTPRJ1_H_
#define PROJECT1_PIROBOTPRJ1_H_

#include <PiRobot.h>

namespace spi_test {

class PiRobotPrj1: public pirobot::PiRobot {
public:
	PiRobotPrj1(const bool real_world = false);
	virtual ~PiRobotPrj1();

	virtual bool configure() override;

};

} /* namespace project1 */

#endif /* PROJECT1_PIROBOTPRJ1_H_ */