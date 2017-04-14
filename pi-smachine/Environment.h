/*
 * Environment.h
 *
 *  Created on: Dec 14, 2016
 *      Author: Denis Kudia
 */

#ifndef PI_SMACHINE_ENVIRONMENT_H_
#define PI_SMACHINE_ENVIRONMENT_H_

#include <mutex>

namespace smachine {

/*
 * Use Environment class for saving your own data
 */
class Environment{
public:
	Environment() {}
	virtual ~Environment() {}

	/*
	 * Use this mutex if you needed to share data operations
	 */
	std::recursive_mutex mutex_sm;

 };

}
#endif /* PI_SMACHINE_ENVIRONMENT_H_ */
