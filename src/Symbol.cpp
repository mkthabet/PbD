/*
 * Primitive.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#include "Symbol.h"

#include <iostream>

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
	if( (this->primitiveID == s.primitiveID)
			&& ( this->getDuration() >= (s.getDuration() - s.getDuration()*DURATION_TOLERANCE_FACTOR) )
			&& ( this->getDuration() <= (s.getDuration() + s.getDuration()*DURATION_TOLERANCE_FACTOR) )
			&& ( this->goalState[0] >= (s.goalState[0] - s.goalState[0]*GOAL_TOLERANCE_FACTOR) )
			&& ( this->goalState[0] <= (s.goalState[0] + s.goalState[0]*GOAL_TOLERANCE_FACTOR) )
			&& ( this->goalState[1] >= (s.goalState[1] - s.goalState[1]*GOAL_TOLERANCE_FACTOR) )
			&& ( this->goalState[1] <= (s.goalState[1] + s.goalState[1]*GOAL_TOLERANCE_FACTOR) )
			&& ( this->goalState[2] >= (s.goalState[2] - s.goalState[2]*GOAL_TOLERANCE_FACTOR) )
			&& ( this->goalState[2] <= (s.goalState[2] + s.goalState[2]*GOAL_TOLERANCE_FACTOR) )
			){
		return true;
	} else {
		return false;
	}
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
