/*
 * PiRobotPrj1.h
 *
 *  Created on: Dec 17, 2016
 *      Author: denis
 */

#ifndef PROJECT1_PIROBOTPRJ1_H_
#define PROJECT1_PIROBOTPRJ1_H_

#include <PiRobot.h>

namespace project1 {

class PiRobotPrj1: public pirobot::PiRobot {
public:
	PiRobotPrj1();
	virtual ~PiRobotPrj1();

	virtual bool configure() override;

};

} /* namespace project1 */

#endif /* PROJECT1_PIROBOTPRJ1_H_ */
