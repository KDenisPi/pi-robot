/*
 * item.h
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_ITEM_H_
#define PI_LIBRARY_ITEM_H_

#include "gpio.h"

namespace pirobot {
namespace item {

class Item {
public:
	Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio) :
	m_gpio(gpio),
	m_name("Item"),
	m_comment("Comment") {};

	Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment):
		m_gpio(gpio),
		m_name(name),
		m_comment(comment)
	{};

	virtual ~Item() {};

	virtual bool initialize() {return true;};
	virtual void stop() {};
	void set_name(const std::string name) {m_name = name;}
	void set_comment(const std::string comment) {m_comment = comment;}
	const std::string name() { return m_name; }
	const std::string comment() { return m_comment;}

	virtual const std::string to_string() = 0; 
	virtual const std::string printConfig() = 0;

	const std::shared_ptr<pirobot::gpio::Gpio> get_gpio() {return m_gpio;}

private:
	std::string m_name;
	std::string m_comment;

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio;
};

}/* namespace item*/
}/* namespace pirobot */


#endif /* PI_LIBRARY_ITEM_H_ */
