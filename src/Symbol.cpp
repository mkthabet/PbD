/*
 * Primitive.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#include "Symbol.h"

#include <iostream>
#include <math.h>

#define GOAL_TOLERANCE_FACTOR 0.1
#define DURATION_TOLERANCE_FACTOR 0.1

Symbol::Symbol() :
		symbolID(-1), primitiveID(-1), componentID(0), startTime(0), endTime(0) {
}

Symbol::Symbol(int primitiveID, double startTime, double endTime,
		const std::vector<double>& goalState) :
		symbolID(-1), primitiveID(primitiveID), componentID(0), startTime(
				startTime), endTime(endTime), goalState(goalState) {
}

Symbol::~Symbol() {
	// TODO Auto-generated destructor stub
}

double Symbol::getDuration() const {
	return endTime - startTime;
}

std::vector<double> Symbol::getGoalState() {
	return goalState;
}

bool Symbol::operator ==(const Symbol& s) {
	if( (this->componentID == s.componentID)
			&& (this->primitiveID == s.primitiveID)
			&& ( fabs(this->getDuration() - s.getDuration()) < s.getDuration()*DURATION_TOLERANCE_FACTOR )
			&& ( this->goalState.size() == s.goalState.size() )
			) {
		for (unsigned int i = 0 ; i < this->goalState.size() ; i++){
			if ( fabs(this->goalState[i]- s.goalState[i]) > s.goalState[i]*GOAL_TOLERANCE_FACTOR) {
				return false;
			}	//end last if
		}	//end for
		return true;
	} else {	//else for first if
		return false;
	}	//end first if
}

void Symbol::operator =(const Symbol& s) {
	this->componentID = s.componentID;
	this->endTime = s.endTime;
	this->goalState = s.goalState;
	this->primitiveID = s.primitiveID;
	this->startTime = s.startTime;
	this->symbolID = s.symbolID;
}

void Symbol::printAll() const {
	std::cout << "Primitive ID: " << primitiveID << ", componentID: "
			<< componentID << ", start: " << startTime << ", end: " << endTime
			<< std::endl;
}
