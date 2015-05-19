/*
 * Place.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: thabetm1
 */

#include "Place.h"
#include <iostream>

namespace PN {

Place::Place() {
	// TODO Auto-generated constructor stub

}

Place::Place(int ID, const Symbol& symbol) :
		ID(ID), symbol(symbol), ordinary(true) , marked4Deletion(false), init_token_cnt(0), type(ORDINARY){
}

Place::~Place() {
	// TODO Auto-generated destructor stub
}

void Place::addToken(const Token& token) {
	tokens.push_back(token);
}

void Place::removeToken() {
	tokens.pop_back();
}

void Place::empty() {
	tokens.empty();
}

bool Place::hasTokens() {
	if (tokens.size() > 0) {
		return true;
	} else {
		return false;
	}
}

int Place::getTokenCount() {
	return tokens.size();
}

Token Place::popToken() {
	Token token = *(tokens.rend());
	tokens.pop_back();
	return token;
}

void Place::addInputTransition(int tID) {
	inputTransitionsIDs.push_back(tID);
}

void Place::addOutputTransition(int tID) {
	outputTransitionsIDs.push_back(tID);
}

void Place::printPlace() {
	std::cout << "Place: " << ID << ", Primitive: ";
	symbol.printAll();
	std::cout << ", i/p: ";
	for (unsigned int i = 0 ; i<inputTransitionsIDs.size(); i++){
		std::cout << inputTransitionsIDs[i] << ", ";
	}
	std::cout << ", o/p: ";
	for (unsigned int i = 0 ; i<outputTransitionsIDs.size(); i++){
		std::cout << outputTransitionsIDs[i] << ", ";
	}
	std::cout << "tokens: " << getTokenCount();
	if(ordinary && hasTokens()){
		std::cout << "value: " << tokens[0].value;
	}
	std::cout << std::endl;
}

} /* namespace PN */

bool PN::Place::operator ==(const Place& p) {
	return (this->symbol == p.getSymbol());
}

void PN::Place::operator =(const Place& p) {
	this->ID = p.ID;
	this->inputTransitionsIDs = p.inputTransitionsIDs;
	this->outputTransitionsIDs = p.outputTransitionsIDs;
	this->symbol = p.symbol;
	this->marked4Deletion = p.marked4Deletion;
}
