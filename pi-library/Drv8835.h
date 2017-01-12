/*
 * DRV8835.h
 *
 *  Created on: Jan 11, 2017
 *      Author: denis
 */

#ifndef PI_LIBRARY_DRV8835_H_
#define PI_LIBRARY_DRV8835_H_

#include <memory>
#include "item.h"

namespace pirobot {
namespace item {

enum DRV8835_MODE {
	IN_IN = 0,
	PH_EN = 1
};


class Drv8835: public Item {
public:
	Drv8835(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
			DRV8835_MODE mode = DRV8835_MODE::PH_EN);

	Drv8835(const std::shared_ptr<pirobot::gpio::Gpio> gpio_mode,
			const std::string name,
			const std::string comment,
			DRV8835_MODE mode = DRV8835_MODE::PH_EN);

	inline const DRV8835_MODE get_mode() const {return m_mode;}

	virtual ~Drv8835();

	virtual const std::string to_string() override;
	virtual const std::string printConfig() override;

private:
	DRV8835_MODE m_mode;

};

} /* namespace item */
} /* namespace pirobot */

#endif /* PI_LIBRARY_DRV8835_H_ */
