/*
 * Place.h
 *
 *  Created on: Apr 18, 2015
 *      Author: thabetm1
 */

/* This file contains the definition of the place class for use in the high-level Petri net (HLPN)
 * Places are implemented as having infinite capacity, and can have one of two color sets.
 * One color set is defined as "double" and is reflected in the token value,
 * while the other set is just a binary value (whether the token exists or not).
 * The color set is defined by the boolean "ordinary" for simplicity, where if it's true then
 * this place is associated with the first color set "double", and if it's false then it's
 * associated with the second boolean color set and the token value is ignored.
 * places with the color set "double" are associated with primitives, others are associated with coditions
 */

#ifndef SRC_PLACE_H_
#define SRC_PLACE_H_

#include "Symbol.h"
#include "Token.h"

namespace PN {

class Place {
	friend class HLPN;
private:
	int ID;	// must match corresponding index in incidence matrix and index in place list
	Symbol symbol;	//symbol associated with this place
	bool ordinary; 	//is this a place for a primitive (false if for something else, like a condition)
	enum Type {ORDINARY, START, AUTO, COUNT} type;
	std::vector<Token> tokens;
	std::vector<int> inputTransitionsIDs;
	std::vector<int> outputTransitionsIDs;
	bool marked4Deletion;
	int init_token_cnt;	//initial token count, for initializing marking
public:
	Place();
	Place(int ID, const Symbol& symbol);
	virtual ~Place();
	void addToken(const Token& token);
	void removeToken();
	void empty();
	bool hasTokens();
	int getTokenCount();
	Token popToken();	//removes the last token in the list and returns it
	void addInputTransition(int tID);
	void addOutputTransition(int tID);
	bool operator== (const Place& p);
	void operator= (const Place& p);
	void printPlace();

	int getId() const {
		return ID;
	}

	const Symbol& getSymbol() const {
		return symbol;
	}

	const std::vector<int>& getInputTransitionsIDs() const {
		return inputTransitionsIDs;
	}

	const std::vector<int>& getOutputTransitionsIDs() const {
		return outputTransitionsIDs;
	}
};

} /* namespace PN */

#endif /* SRC_PLACE_H_ */
