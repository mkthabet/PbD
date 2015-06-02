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
	double time_step;	//the time step between 2 successive calls to advance(), i.e. sampling time
	bool constructed;
	double time_seed_value;	//time factor for execution, >1 is slowing down, <1 is speeding up
	boost::numeric::ublas::matrix<int> prev_marking;	//needed for state change comparison
	std::vector<int> exec_end_sigs;	//a list of receive execution end signals


	void updateTimedTransitions(std::vector<int>& enabled_list);
//	std::vector<int> getEnabledList () const;
	std::vector<int> getFireList(std::vector<int>& enabled_list);
	void constructNaivePN(StreamBundle& bundle);	// construct naive PN from bundle
	void addPlace(const Place& p);
	void addTransition(const Transition& t);
	void bind(const Place& p, const Transition& t, int weight = 1, double expr = 0);	//bind input place to output transition
	void bind(const Transition& t, const Place& p, int weight = 1, double expr = 1);	//bind input transition to output place
	void constructIncidence();	// builds the incidence matrix
	void consolidateConcurrency();
	bool findRepetition(int& length, int& count, int& startID, std::vector<Place>& pList);	//finds repeating sequences of places
	bool areEqual(const Transition& t1 , const Transition& t2);	//checks if two transitions are equal
	void fold();	//makes repetitions constructs
	void merge(Place& p1, Place& p2);	//merges 2 places
	void updateLists();	//update lists by removing elements marked for deletion and updating IDs
	void initMarking();
	int resolveConflict(int t1_id, int t2_id);	//resolves a conflict between 2 transitions, returns loser

public:
	HLPN();
	HLPN(int p, int t);
	HLPN(int p, int t, const matrix<int>& m, const matrix<int>& w);
	virtual ~HLPN();

	void constructPN(StreamBundle& bundle);
	bool advance();		// moves the state forward
	bool advance(double time_step);	//moves the state forward and changes the sampling time
	void initPN(double time_step, double time_seed_value = 1);	//inits PN, time_seed_value is the time factor for execution, >1 is slowing down, <1 is speeding up
	bool getExecSymbols(std::vector<Symbol>& syms);	//retrieves execution symbols vector (passed by red)
	void signalExecEnd (int p_id);	//signals to the PN that symbol associated with place p_id has finished executing
	void printPlaces(bool onlyActive = true);
	void printExprMatrix();
	void printTransitions(bool enabled_only = true);
	void printState();
};

} /* namespace PN */

#endif /* SRC_HLPN_H_ */
