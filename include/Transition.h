/*
 * Transition.h
 *
 *  Created on: Apr 18, 2015
 *      Author: thabetm1
 */

#ifndef SRC_TRANSITION_H_
#define SRC_TRANSITION_H_

#include <vector>

namespace PN {

class Place;

class Transition {
	friend class HLPN;
private:
	int ID;	//must match corresponding index in incidence matrix and index in transition list
	double delay;	// delay associated with transition
	double timeRemaining;	// time remaining till delay is over
	bool enabled;
	std::vector<int> inputPlacesIDs;
	std::vector<int> outputPlacesIDs;
	bool marked4Deletion;
	void enableTransition(double timeParameter);
public:
	Transition();
	Transition(int ID);
	virtual ~Transition();

	void addInputPlace(int pID);
	void addOutputPlace(int pID);

	int getId() const {
		return ID;
	}

	const std::vector<int>& getInputPlacesIDs() const {
		return inputPlacesIDs;
	}

	const std::vector<int>& getOutputPlacesIDs() const {
		return outputPlacesIDs;
	}
};

} /* namespace PN */

#endif /* SRC_TRANSITION_H_ */
