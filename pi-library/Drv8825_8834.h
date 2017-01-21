/*
 * Drv8825_8834.h
 *
 *  Created on: Jan 20, 2017
 *      Author: denis
 */

#ifndef PI_LIBRARY_DRV8825_8834_H_
#define PI_LIBRARY_DRV8825_8834_H_

namespace pirobot {
namespace item {

enum DRV88_PIN {
	PIN_ENABLE = 0,
	PIN_RESET = 1,
	PIN_SLEEP = 2,
	PIN_DECAY = 3,
	PIN_MODE_0 = 4,
	PIN_MODE_1 = 5,
	PIN_MODE_2 = 6,
	PIN_DIR = 7
};

/*
 * Step size
 * Below values for Mode2, Mode1, Mode0
 */
enum DRV8825_MICROSTEP {
	STEP_FULL = 0, 	// 0,0,0
	STEP_1_2 = 1,	// 0,0,1
	STEP_1_4 = 2,	// 0,1,0
	STEP_1_8 = 3,	// 0,1,1
	STEP_1_16 = 4,	//1,0,0
	STEP_1_32 = 5	//1,0,1 (also 1,1,0 and 1,1,1)
};


/*
 * Step size
 * Below values for Mode1, Mode0
 *
 * Set Step Size
 *
 *
 * 	M0			M1		Microstep Resolution
 * 	----------------------------------------
 *	Low			Low		Full step
 *	High		Low		Half step
 *	Floating	Low		1/4 step
 *	Low			High	1/8 step
 *	High		High	1/16 step
 *	Floating	High	1/32 step
 */

enum DRV8834_MICROSTEP {
	STEP34_FULL = 0, 	// 0,0
	STEP34_1_2  = 1,	// 0,1
	STEP34_1_8  = 2,	// 1,0
	STEP34_1_16 = 3,	// 1,1

	STEP34_1_4  = 4,	// 0,X
	STEP34_1_32 = 5		// 1,X
};


}
}

#endif /* PI_LIBRARY_DRV8825_8834_H_ */
