/*
 * HLPN.h
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#ifndef SRC_HLPN_H_
#define SRC_HLPN_H_

#include "PetriNet.h"
#include "StreamBundle.h"
#include "Transition.h"
#include "Place.h"
#include "Arc.h"

namespace PN {

class HLPN: public PetriNet {
	friend void testHLPN();
private:
	int componentCount;		//number of robot components used
	std::vector<Place> placeList;
	std::vector<Transition> transitionList;
	std::vector<Arc> PTArcs;	// list of arcs from places to transitions
	std::vector<Arc> TPArcs;	// list of arcs from transitions to places
	boost::numeric::ublas::matrix<double> exprMatrix;	//arc expression matrix
	void constructNaivePN(StreamBundle& bundle);	// construct naive PN from bundle
	void addPlace(const Place& p);
	void addTransition(const Transition& t);
	void bind(const Place& p, const Transition& t, int weight = 1, double expr = 0);	//bind input place to output transition
	void bind(const Transition& t, const Place& p, int weight = 1, double expr = 1);	//bind input transition to output place
	void constructIncidence();	// builds the incidence matrix
	void consolidateConcurrency();
	bool areEqual(const Transition& t1 , const Transition& t2);	//checks if two transitions are equal
	void fold();	//makes repetitions constructs
	void merge(Place& p1, Place& p2);	//merges 2 places
	void updateLists();	//update lists by removing elements marked for deletion and updating IDs
public:
	HLPN();
	HLPN(int p, int t);
	HLPN(int p, int t, const matrix<int>& m, const matrix<int>& w);
	virtual ~HLPN();

	void constructPN(StreamBundle& bundle);
	void advance();		// moves the state forward
	bool findRepetition(int& length, int& count, int& startID, std::vector<Place>& pList);	//finds repeating sequences of places
	void printPlaces(bool onlyActive = true);
	void printExprMatrix();
};

} /* namespace PN */

#endif /* SRC_HLPN_H_ */
