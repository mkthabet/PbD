/*
 * HLPN.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1

 */

#include "HLPN.h"
#include <iostream>
#include <algorithm>

namespace PN {

HLPN::HLPN() :
		PetriNet() {
	componentCount = 0;
}

void HLPN::constructNaivePN(StreamBundle& bundle) {
	/* This function creates a naive PN from a stream bundle. This PN
	 * serves as a foundation for further processing and refinement.
	 * In this PN each place has at the most 1 input transition and 1 output transition,
	 * while each transition has exactly one input place but may have more than one
	 * output place. Each place can have only one token in it at a time.
	 */
	// add the start place and transition
	Symbol s;
	Place p(0, s);
	p.type = Place::START;
	addPlace(p);
	Transition t(0);
	addTransition(t);
	bind(p, t);	// bind transitions to places AFTER they're BOTH pushed on the list because pointers are used
	// start construction
	for (int i = 0; i < bundle.getSymbolCount(); i++) {	// go thru all the symbols
		s = bundle.getNextSymbol();	// get next symbol in all of the bundle
		if (i == 0 || s.getStartTime() == 0) {// if this is the first symbol in the whole bundle or starts at time 0
			p = Place(placeCount, s);
			addPlace(p);
			// bind it to the first transition. the arc expression is the symbol duration since this is the first symbol
			bind(transitionList[0], p, 1, s.getDuration());
			continue;	// skip the rest of this iteration
		}
		if (s.getSymbolId() == 0) {	//if this is the first symbol in a stream but not the first in the whole bundle
			//construct dummy wait symbol between the start of the last symbol and the start of the new
			Symbol dummy(0,
					placeList[placeCount - 1].getSymbol().getStartTime(),
					s.getStartTime(), std::vector<double>());
			dummy.setComponentId(s.getComponentId());// give it the same component ID
			//now add a place for the dummy wait symbol and bind it to the last transition
			p = Place(placeCount, dummy);
			p.type = Place::AUTO ;
			addPlace(p);
			double expr;	//the input arc expression
			Place pBefore =
					placeList[transitionList.back().getInputPlacesIDs().front()];//the o/p p of the i/p t to this dummy wait p
			if (pBefore.ID == 0) {	// if it's the start place
				expr = dummy.getDuration();	// the i/p arc expression is just the duration
			} else {
				//the i/p arc expression = new_place_time / previous_place_duration (previous here is the i/p p to the i/p t for the new p)
				expr = dummy.getDuration() / pBefore.getSymbol().getDuration();
			}
			// bind the dummy place to the last transition (the i/p of the last place)
			bind(transitionList.back(), p, 1, expr);
		}
		// find the last place that had a primitive from the same component
		for (std::vector<Place>::reverse_iterator rit = placeList.rbegin();
				rit != placeList.rend(); rit++) {//go thru placeList newest first
			if (rit->getSymbol().getComponentId() == s.getComponentId()) {
				// create new t and bind it as o/p to the found p
				t = Transition(transitionCount);	//create new transition
				addTransition(t);
				//get arc expression = new_place_time / previous_place_duration
				double expr = s.getDuration() / rit->getSymbol().getDuration();
				bind(*rit, t);
				p = Place(placeCount, s);//create new place for the current symbol
				addPlace(p);
				bind(t, p, 1, expr);//bind it as o/p to the i/p t of last place of same component
				break;
			}
		}
	}
}

HLPN::~HLPN() {
// TODO Auto-generated destructor stub
}

PN::HLPN::HLPN(int p, int t) :
		PetriNet(p, t) {
}

PN::HLPN::HLPN(int p, int t, const matrix<int>& m, const matrix<int>& w) :
		PetriNet(p, t, m, w) {
}

void HLPN::addPlace(const Place& p) {
	placeList.push_back(p);
	placeCount++;
}

void HLPN::addTransition(const Transition& t) {
	transitionList.push_back(t);
	transitionCount++;
}

void HLPN::bind(const Place& p, const Transition& t, int weight, double expr) {
	placeList[p.getId()].addOutputTransition(t.getId());
	transitionList[t.getId()].addInputPlace(p.getId());
	Arc arc(PTArcs.size(), p.getId(), t.getId(), weight, expr);
	PTArcs.push_back(arc);
}

void HLPN::bind(const Transition& t, const Place& p, int weight, double expr) {
	placeList[p.getId()].addInputTransition(t.getId());
	transitionList[t.getId()].addOutputPlace(p.getId());
	Arc arc(TPArcs.size(), p.getId(), t.getId(), weight, expr);
	TPArcs.push_back(arc);
}

void HLPN::constructIncidence() {
	incidence.resize(placeCount, transitionCount);
	incidence.clear();
	exprMatrix.resize(placeCount, transitionCount);
	exprMatrix.clear();
// fill incidence and expression matrices
	for (unsigned int i = 0; i < PTArcs.size(); i++) {
		incidence(PTArcs[i].placeID, PTArcs[i].transitionID) =
				-PTArcs[i].weight;
	}
	for (unsigned int i = 0; i < TPArcs.size(); i++) {
		incidence(TPArcs[i].placeID, TPArcs[i].transitionID) = TPArcs[i].weight;
		// expressions are applicable only to transition output arcs
		exprMatrix(TPArcs[i].placeID, TPArcs[i].transitionID) =
				TPArcs[i].expression;
	}
}

void HLPN::constructPN(StreamBundle& bundle) {
	componentCount = bundle.getStreamCount();
	constructNaivePN(bundle);
	consolidateConcurrency();
	fold();
	constructIncidence();
	marking.resize(placeCount, 1);
	marking.clear();
	marking(0, 0) = 1;
	Token token;
	token.value = 1;
	placeList[0].addToken(token);
	charVector.resize(transitionCount, 1);
	charVector.clear();
	CalculateConflictMatrix();
}

void HLPN::advance() {

	PetriNet::advance();
// update tokens in places
	for (int j = 0; j < transitionCount; j++) {	//go thru all transitions
		if (charVector(j, 0) == 1) {	//if transition i has just fired
			Token token;	//token that will be fetched
			for (unsigned int i = 0;
					i < transitionList[j].getInputPlacesIDs().size(); i++) {//go thru all i/p places
				for (int k = 0;
						k
								< placeList[transitionList[j].getInputPlacesIDs()[i]].getTokenCount();
						k++) {
					if (placeList[transitionList[j].getInputPlacesIDs()[i]].tokens[k].value
							!= 0) {
						token.value =
								placeList[transitionList[j].getInputPlacesIDs()[i]].tokens[k].value;
					}
				}// for each transition there will always be a max of 1 i/p place with non-zero token
				placeList[transitionList[j].getInputPlacesIDs()[i]].empty();//remove all tokens
			}
			for (unsigned int i = 0;
					i < transitionList[j].getOutputPlacesIDs().size(); i++) {//go thru all o/p places
				Token tok;	//token to be deposited
				tok.value =
						token.value
								* exprMatrix(
										placeList[transitionList[j].getOutputPlacesIDs()[i]].getId(),
										transitionList[j].getId());	// do value transformation
				placeList[transitionList[j].getOutputPlacesIDs()[i]].addToken(
						tok);	//deposit token
			}
		}
	}
}

void HLPN::printPlaces(bool onlyActive) {
	for (unsigned int i = 0; i < placeList.size(); i++) {
		if (!onlyActive || marking(i, 0) >= 1) {
			placeList[i].printPlace();
		}
	}
}

void HLPN::consolidateConcurrency() {
	/*
	 * This method goes through the naive PN and consolidates concurrency between different
	 * 	components. It looks for wait-places (places that are not associated with a primitive)
	 * 	and binds them with places associated with other components via transitions.
	 */
	Place p;		// the wait-place
	Place pNext;// the place associated with the same component that follows the current wait-place
	Place pPrev;// the place associated with any component the precedes pNext
	for (int i = 0; i < placeCount; i++) {	//go through all places in PN
		if (placeList[i].getSymbol().getPrimitiveId() == 0) {//if this is a wait-place
			p = placeList[i];
			if (placeList[p.ID + 1].getSymbol().getComponentId()
					== p.getSymbol().getComponentId()) {// if the wait place is succeeded by a place of the same component
					//this means that the component just have to wait for some time on its own.
					// set the o/p transition delay of the wait-place with the wait duration
				transitionList[p.getOutputTransitionsIDs().front()].delay =
						p.getSymbol().getDuration();
				//since this is a naive PN, there's only 1 o/p transition
			} else {
				for (int j = i + 1; j < placeCount; j++) { //search all succeeding places
					if (placeList[j].getSymbol().getComponentId()
							== p.getSymbol().getComponentId()) { //if place j belongs to the same component
						pNext = placeList[j];
						break;
					}
				}
				pPrev = placeList[pNext.ID - 1];
				// bind the i/p transition of pPrev to the wait-place
				// this arc expression is zero because it's a boolean token
				bind(transitionList[pPrev.getInputTransitionsIDs().front()], p,
						1, 0);
				// increment the arc weight from p to its o/p transition
				for (unsigned int j = 0; j < PTArcs.size(); j++) {
					if (PTArcs[j].placeID == p.ID) {//since this is a naive PN, p has only 1 o/p arc
						PTArcs[j].weight++;
						break;
					}
				}
				transitionList[p.getOutputTransitionsIDs().front()].delay =
						p.getSymbol().getEndTime()
								- pPrev.getSymbol().getStartTime();
				//since this is a naive PN, there's only 1 o/p transition
			}
		}
	}
}

bool HLPN::findRepetition(int& l, int& c, int& s, std::vector<Place>& pList) {
	/********************
	 * This function finds repetitions in the place list and returns true if a repetition is found.
	 * the length of the repetion and the number of repetitions will be copied to
	 * the arguments passed by reference. The repetitions must be consequtive and adjacent.
	 ***********************/
	//make a second list of places so that places that shouldn't be processed can be deleted from it.
	pList = placeList;
	std::vector<Place>::iterator it;
	for (it = pList.begin() + 1; it != pList.end(); it++) {
		if (it->type != Place::ORDINARY) {
			pList.erase(it);//delete the place if it's an auto-created wait place at the start of a component stream
			break;
		}
	}
	int count = 0;	//repetition count
	std::vector<Place>::iterator found_it;	//search result
	std::vector<Place>::iterator search_it;	//search window start
	for (int length = (pList.size() - 1) / 2; length > 1; length--) {
		int startPos = 1;	// start position of the repetition sequence in new list
		for (it = pList.begin() + 1; it + 2 * length - 1 != pList.end(); it++) {
			search_it = it + length;
			while (1) {
				found_it = std::search(search_it, pList.end(), it, it + length);
				if (found_it == search_it && found_it != pList.end()) {
					//if the repeated sequence is adjacent to the original
					count++;
					search_it = found_it + length;
				} else {
					break;
				}
			}
			if (count > 0) {
				std::cout << "repetition found, count = " << count
						<< "start ID = " << startPos << "length = " << length
						<< std::endl;
				l = length;
				c = count;
				s = startPos;
				return true;
			}
			startPos++;
		}
	}
	std::cout << "found nothing";
	return false;
}

bool HLPN::areEqual(const Transition& t1, const Transition& t2) {
	/************
	 * Function checks if 2 transitions are equal. 2 Transitions are equal if all the
	 * i/p and o/p places of both transitions are equal.
	 *************/
	bool ret = false;
	if (t1.inputPlacesIDs.size() != t2.inputPlacesIDs.size()
			|| t1.outputPlacesIDs.size() != t2.outputPlacesIDs.size()) {
		return false;
	}
	for (unsigned int i = 0; i < t1.inputPlacesIDs.size(); i++) {
		ret = false;
		for (unsigned int j = 0; j < t2.inputPlacesIDs.size(); j++) {
			if (placeList[t1.inputPlacesIDs[i]]
					== placeList[t2.inputPlacesIDs[j]]) {
				ret = true;
				break;
			}
		}
		if (!ret) {
			return false;
		}
	}
	for (unsigned int i = 0; i < t1.outputPlacesIDs.size(); i++) {
		ret = false;
		for (unsigned int j = 0; j < t2.outputPlacesIDs.size(); j++) {
			if (placeList[t1.outputPlacesIDs[i]]
					== placeList[t2.outputPlacesIDs[j]]) {
				ret = true;
				break;
			}
		}
		if (!ret) {
			return false;
		}
	}
	return true;	//if we reach here, then they're equal
}

void HLPN::fold() {
	/**************
	 * This function sends equivalent places in repetitions to be merged
	 *************/
	int length, count, startPos;
	std::vector<Place> pList;
	while (findRepetition(length, count, startPos, pList)) {//params passed by reference
		for (int j = 1; j <= count; j++) {
			for (int i = 0; i < length; i++) {
				if (placeList[pList[startPos + i].ID]
						== placeList[pList[startPos + i + length * j].ID]) {
					merge(placeList[pList[startPos + i].ID],
							placeList[pList[startPos + i + length * j].ID]);
				} else {
					std::cout
							<< "HLPN::fold()|| WARNING! Tried to merge two unequal places!\n";
				}
			}
		}
		updateLists();
	}
}

void HLPN::merge(Place& p1, Place& p2) {
	/***********
	 * This function merges two places. p2 is absorbed into p1 and then marked for deletion.
	 * Actual deletion happens in updateLists().
	 * All i/p and o/p t's of p2 are checked against those of p1. If a t
	 * has an equivalent in p1, then this t is marked for deletion. If not, then this t
	 * is bound to p1 instead. Finally, arcs are updated accordingly.
	 *************/
	placeCount--;	// a place will be absorbed (deleted) here
	p2.marked4Deletion = true;
	for (unsigned int i = 0; i < p2.inputTransitionsIDs.size(); i++) {//go thru all p2 i/p t's
		Transition* t = &transitionList[p2.inputTransitionsIDs[i]];//define alias for more readable code
		if (t->marked4Deletion){
			continue;
		}
		bool tFound = false;//assume this t is not found in p1 i/p t's by default
		for (unsigned int j = 0; j < p1.inputTransitionsIDs.size(); j++) {
			//check against all i/p t's of p2
			if (areEqual(transitionList[p1.inputTransitionsIDs[j]], *t)) {
				tFound = true;
				break;
			}
		}
		if (tFound) {	//if t is found, mark it for deletion
			t->marked4Deletion = true;
			transitionCount--;
		} else {	//else bind it to p1 instead
			for (unsigned int j = 0; j < t->outputPlacesIDs.size(); j++) {
				if (t->outputPlacesIDs[j] == p2.ID) {
					t->outputPlacesIDs[j] = p1.ID;
					break;
				}
			}
			p1.addInputTransition(t->ID);
			for (unsigned int j = 0; j < TPArcs.size(); j++) { //find arc from t to p2
				if (TPArcs[j].transitionID == t->ID
						&& TPArcs[j].placeID == p2.ID) {
					TPArcs[j].placeID = p1.ID;
					break;
				}
			}
		}
	}
	// Do the exact same thing as above but for o/p t's
	for (unsigned int i = 0; i < p2.outputTransitionsIDs.size(); i++) { //go thru all p2 o/p t's
		Transition* t = &transitionList[p2.outputTransitionsIDs[i]]; //define alias for more readable code
		if (t->marked4Deletion){
			continue;
		}
		bool tFound = false; //assume this t is not found in p1 i/p t's by default
		for (unsigned int j = 0; j < p1.outputTransitionsIDs.size(); j++) {
			//check against all i/p t's of p2
			if (areEqual(transitionList[p1.outputTransitionsIDs[j]], *t)) {
				tFound = true;
				break;
			}
		}
		if (tFound) {	//if t is found mark it for deletion
			t->marked4Deletion = true;
			transitionCount--;
		} else {	//else bind it to p1 instead
			for (unsigned int j = 0; j < t->inputPlacesIDs.size(); j++) {
				if (t->inputPlacesIDs[j] == p2.ID) {
					t->inputPlacesIDs[j] = p1.ID;
					break;
				}
			}
			p1.addOutputTransition(t->ID);
			for (unsigned int j = 0; j < PTArcs.size(); j++) { //find arc from p2 to t
				if (PTArcs[j].transitionID == t->ID
						&& PTArcs[j].placeID == p2.ID) {
					PTArcs[j].placeID = p1.ID;
					break;
				}
			}
		}
	}
}

void HLPN::updateLists() {
	/******************
	 * This function deletes all elements marked for deletion, then updates all IDs
	 * to match the indices in lists. It also updates arcs by deleting irrelevant arcs
	 * and modifying IDs in still relevant ones.
	 ***************************/
	// update place list
	std::vector<Place> pList;	//new place list
	pList.reserve(placeCount);
	for (unsigned int i = 0; i < placeList.size(); i++) {//go thru all places in old list
		if (!placeList[i].marked4Deletion) {	//if this place is to be kept
			pList.push_back(placeList[i]);	//copy it to the new list
			pList.back().inputTransitionsIDs.clear();	//will be reconstructed later when updating arcs
			pList.back().outputTransitionsIDs.clear();
			if (pList.back().ID != pList.size() - 1) {//if there has been deletions before this place
				pList.back().ID = pList.size() - 1;	//modify its ID to be equal to its index
				for (unsigned int j = 0; j < TPArcs.size(); j++) {
					if (TPArcs[j].placeID == placeList[i].ID) { //find arcs to the old p
						TPArcs[j].placeID = pList.back().ID;	//update them to store the new ID
					}
				}
				for (unsigned int j = 0; j < PTArcs.size(); j++) {	//same thing but for o/p arcs
					if (PTArcs[j].placeID == placeList[i].ID) { //find arcs from the old p
						PTArcs[j].placeID = pList.back().ID;	//update them to store the new ID
					}
				}
			}
		}
	}
	placeList = pList;	//replace the old list with the new
	// update transition list
	std::vector<Transition> tList;	//new transition list
	tList.reserve(transitionCount);
	for (unsigned int i = 0; i < transitionList.size(); i++) {//go thru all t's in old list
		if (!transitionList[i].marked4Deletion) {	//if this transition is to be kept
			tList.push_back(transitionList[i]);	//copy it to the new list
			tList.back().inputPlacesIDs.clear();	//will be reconstructed later when updating arcs
			tList.back().outputPlacesIDs.clear();
			if (tList.back().ID != tList.size() - 1) {//if there has been deletions before this transition
				tList.back().ID = tList.size() - 1;	//modify its ID to be equal to its index
				for (unsigned int j = 0; j < TPArcs.size(); j++) {
					if (TPArcs[j].transitionID == transitionList[i].ID) { //find arcs from the old t
						TPArcs[j].transitionID = tList.back().ID;	//update them to store the new ID
					}
				}
				for (unsigned int j = 0; j < PTArcs.size(); j++) {	//same thing but for o/p arcs
					if (PTArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
						PTArcs[j].transitionID = tList.back().ID;	//update them to store the new ID
					}
				}
			}
		} else {	// if the t is to be deleted, then delete all associated arcs
			for (unsigned int j = 0; j < TPArcs.size(); j++) {
				if (TPArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
					TPArcs[j].marked4Deletion = true;	//delete arc
				}
			}
			for (unsigned int j = 0; j < PTArcs.size(); j++) {	//same thing but for i/p arcs
				if (PTArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
					PTArcs[j].marked4Deletion = true;	//delete arc
				}
			}
		}
	}
	transitionList = tList;	//replace the old t list with the new
	// Update arc lists and use them to update bindings in p's and t's
	std::vector<Arc> aList;
	for (unsigned int i = 0 ; i < PTArcs.size() ; i++){
		if(!PTArcs[i].marked4Deletion){
			aList.push_back(PTArcs[i]);
			placeList[PTArcs[i].placeID].addOutputTransition(PTArcs[i].transitionID);
			transitionList[PTArcs[i].transitionID].addInputPlace(PTArcs[i].placeID);
		}
	}
	PTArcs = aList;
	aList.clear();
	for (unsigned int i = 0 ; i < TPArcs.size() ; i++){
		if(!TPArcs[i].marked4Deletion){
			aList.push_back(TPArcs[i]);
			placeList[TPArcs[i].placeID].addInputTransition(TPArcs[i].transitionID);
			transitionList[TPArcs[i].transitionID].addOutputPlace(TPArcs[i].placeID);
		}
	}
	TPArcs = aList;
}
void HLPN::printExprMatrix() {
	std::cout << "Expr: " << exprMatrix << std::endl;
}

} // namespace PN
