/*
 * Transition.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: thabetm1
 */

#include "Transition.h"
#include <iostream>

namespace PN {

Transition::Transition() {
	// TODO Auto-generated constructor stub

}

Transition::~Transition() {
	// TODO Auto-generated destructor stub
}

} /* namespace PN */

PN::Transition::Transition(int ID) :
		ID(ID), delay(0), timeRemaining(0), enabled(false), marked4Deletion(
				false) {
}

void PN::Transition::addInputPlace(int pID) {
	inputPlacesIDs.push_back(pID);
}

void PN::Transition::resetDelay(double time_factor) {
	timeRemaining = delay * time_factor;
}

void PN::Transition::addOutputPlace(int pID) {
	outputPlacesIDs.push_back(pID);
}

bool PN::Transition::isTimed() {
	return (delay > 0.0);
}

void PN::Transition::printTransition() {
	std::cout << "Transition: " << ID;
	if (isTimed()) {
		std::cout << ", delay: " << delay << "time remaining: "
				<< timeRemaining;
	}
	std::cout << std::endl;
}
