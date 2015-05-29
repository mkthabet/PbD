/*
 * PetriNet.h
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#ifndef SRC_PETRINET_H_
#define SRC_PETRINET_H_

#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"

using namespace boost::numeric::ublas;

class PetriNet {
protected:
	int placeCount;
	int transitionCount;
	matrix<int> marking; //marking column vector of placeCount elements, declared as matrix for linear algebra operations
	matrix<int> incidence; //incidence matrix: placeCount rows and transitionCount columns
	matrix<int> conflict;	//transitionCount by transitionCount matrix, each element(i,j) is 1 if Ti and Tj are in conflict.
	matrix<int> charVector;	//characteristic vector, currently calculated in advance()
	std::vector<int> getEnabledList() const;// returns a list of enabled transitions.
	void CalculateConflictMatrix();
	virtual std::vector<int> getFireList();		// returns a list of transitions to fire
	virtual int resolveConflict(int t1, int t2);	//resolves a conflict between 2 transitions, returns loser
public:
	PetriNet();
	PetriNet(int p, int t);
	PetriNet(int p, int t, const matrix<int>& m, const matrix<int>& w);
	virtual ~PetriNet();

	void advance(const matrix<int>&);//takes a characteristic vector as an argument
	virtual bool advance();
	void printMarking();
	void printIncidence();
	void printConflict();
	void printEnabledList();	//for testing
	void printList(std::vector<int> list);	//for testing
	matrix<int> getIncidence() const {
		return incidence;
	}

	void setIncidence(const matrix<int>& incidence) {
		this->incidence = incidence;
		CalculateConflictMatrix();
	}

	matrix<int> getMarking() const {
		return marking;
	}

	void setMarking(const matrix<int>& marking) {
		this->marking = marking;
	}

	int getPlaceCount() const {
		return placeCount;
	}

	void setPlaceCount(int placeCount) {
		this->placeCount = placeCount;
	}

	int getTransitionCount() const {
		return transitionCount;
	}

	void setTransitionCount(int transitionCount) {
		this->transitionCount = transitionCount;
	}
};

#endif /* SRC_PETRINET_H_ */
