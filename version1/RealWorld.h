/*
 * RealWorld.h
 *
 *  Created on: Nov 10, 2016
 *      Author: Denis Kudia
 */

#ifndef VERSION1_REALWORLD_H_
#define VERSION1_REALWORLD_H_

#include "PiRobot.h"

namespace realworld {

class RealWorld : public pirobot::PiRobot {

public:
	RealWorld();
	virtual ~RealWorld() {}

	virtual bool configure();
};

} /* namespace realworld */

#endif /* VERSION1_REALWORLD_H_ */
