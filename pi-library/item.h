/*
 * item.h
 *
 *  Created on: Nov 7, 2016
 *      Author: denis
 */

#ifndef PI_LIBRARY_ITEM_H_
#define PI_LIBRARY_ITEM_H_

namespace pirobot {

namespace item {

class Item {
public:
	Item() {};
	Item(const std::string name, const std::string comment):
		m_name(name),
		m_comment(comment)
	{};

	virtual ~Item() {};

	virtual bool initialize() = 0;
	void set_name(const std::string name) {m_name = name;}
	void set_comment(const std::string comment) {m_comment = comment;}
	const std::string name() { return m_name; }
	const std::string comment() { return m_comment;}

	virtual const std::string to_string();

private:
	std::string m_name;
	std::string m_comment;
};

}

}/* namespace pirobot */


#endif /* PI_LIBRARY_ITEM_H_ */
