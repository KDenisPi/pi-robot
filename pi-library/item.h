/*
 * item.h
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_ITEM_H_
#define PI_LIBRARY_ITEM_H_

#include <functional>
#include "gpio.h"

namespace pirobot {
namespace item {

enum ItemTypes{
	UNKNOWN = 0,
	LED = 1,
	BUTTON = 2,
	SERVO = 3,
	TILT_SWITCH = 4,
	STEPPER = 5,
	DCMotor = 6
};

enum BUTTON_STATE{
	BTN_NOT_PUSHED = 0,
	BTN_PUSHED = 1
};


class Item {
public:
	Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio, int itype = ItemTypes::UNKNOWN) :
	m_gpio(gpio),
	m_name("Item"),
	m_comment("Comment"),
	m_type(itype),
	notify(nullptr)
	{};

	Item(const std::shared_ptr<pirobot::gpio::Gpio> gpio,
			const std::string name,
			const std::string comment,
			int itype = ItemTypes::UNKNOWN):
		m_gpio(gpio),
		m_name(name),
		m_comment(comment),
		m_type(itype),
		notify(nullptr)
	{};

	virtual ~Item() {};

	virtual bool initialize() {return true;};
	virtual void stop() {};

	void set_name(const std::string name) {m_name = name;}
	void set_comment(const std::string comment) {m_comment = comment;}

	inline const std::string name() const { return m_name; }
	inline const std::string comment() const { return m_comment;}
	inline const int type() const { return m_type;}
	inline const std::string type_name() { return std::string(ItemNames[type()]); }

	virtual const std::string to_string() = 0; 
	virtual const std::string printConfig() = 0;

	const std::shared_ptr<pirobot::gpio::Gpio> get_gpio() {return m_gpio;}

	static const char* ItemNames[];

	std::function<void(int, std::string&, void*)> notify;
private:
	std::string m_name;
	std::string m_comment;
	int m_type;

	std::shared_ptr<pirobot::gpio::Gpio> m_gpio;
};

}/* namespace item*/
}/* namespace pirobot */


#endif /* PI_LIBRARY_ITEM_H_ */
