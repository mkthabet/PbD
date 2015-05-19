/*
 * Token.h
 *
 *  Created on: Apr 18, 2015
 *      Author: thabetm1
 */

#ifndef SRC_TOKEN_H_
#define SRC_TOKEN_H_

namespace PN {

class Token {
	friend class Place;
	friend class HLPN;
private:
	double value;
public:
	Token();
	Token(double value);
	virtual ~Token();
};

} /* namespace PN */

#endif /* SRC_TOKEN_H_ */
