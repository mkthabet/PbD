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
	constructed = false;
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
	p.init_token_cnt = 1;	//the start place should always have a token in the initial marking
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
			p.type = Place::AUTO;
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
//				if (rit->getSymbol().getPrimitiveId() == 0
//						|| rit->type == Place::ORDINARY) {//if it's an ordinary wait place
//					t.delay = rit->getSymbol().getDuration();//set the t's delay to the duration of the i/p wait place
//				}
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
	constructed = false;
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
	constructed = true;
}

bool HLPN::advance() {
	charVector.clear();
	std::vector<int> enabled_list(getEnabledList());
	if (enabled_list.size() == 0) {
		return false;
	}
	updateTimedTransitions(enabled_list);
	std::vector<int> fireList(getFireList(enabled_list));// a list of transitions to fire initialized with enabled transitions
	// now check for conflicts in the fire list and resolve them
	for (std::vector<int>::iterator it1 = fireList.begin();
			it1 != fireList.end(); ++it1) {	//for each enabled transition in list
//		printList(fireList);
		for (std::vector<int>::iterator it2 = it1 + 1; it2 != fireList.end();
				++it2) {	//test against following entries
			if (conflict(*it1, *it2)) {
				int transitionToErase = resolveConflict(*it1, *it2);
				if (transitionToErase == *it2) {
					fireList.erase(it2);
				} else {
					fireList.erase(it1);
				}
				it1--;//restart this iteration of outer loop since deleting  entry may lead to skipping one or bad indexing
				break;
			}
		}
	}
	// convert fireList into characteristic vector
	for (unsigned int i = 0; i < fireList.size(); i++) {
		charVector(fireList[i], 0) = 1;
	}
	PetriNet::advance(charVector);	//does matrix algebra to update marking;
	// update tokens in places and reset delay times on timed t's
	for (int j = 0; j < transitionCount; j++) {	//go thru all transitions
		if (charVector(j, 0) == 1) {	//if transition i has just fired
			Token token;	//token that will be fetched
			Place* p; //an alias for the place being processed for readability;
			for (unsigned int i = 0;
					i < transitionList[j].getInputPlacesIDs().size(); i++) { //go thru all i/p places
				p = &(placeList[transitionList[j].getInputPlacesIDs()[i]]);
				for (int k = 0; k < p->getTokenCount(); k++) { //go thru all tokens
					if (p->tokens[k].value != 0) {	//if this is a colored token
						token.value = p->tokens[k].value;
					}
				}// for each transition there will always be a max of 1 i/p place with non-zero token
				p->empty();	//remove all tokens
				if (p->getTokenCount() != marking(p->ID, 0)) {
					std::cout
							<< "[HPLN::advance()] Warning: Token count mismatch for place "
							<< p->ID << std::endl;
				}
			}
			for (unsigned int i = 0;
					i < transitionList[j].getOutputPlacesIDs().size(); i++) {//go thru all o/p places
				Token tok;	//token to be deposited
				p = &(placeList[transitionList[j].getOutputPlacesIDs()[i]]);
				tok.value = token.value
						* exprMatrix(p->getId(), transitionList[j].getId());// do value transformation
				p->addToken(tok);	//deposit token
				for (unsigned int k = 0;
						k < p->getOutputTransitionsIDs().size(); k++) {	//go thru all o/p t's of that place
					if (transitionList[p->getOutputTransitionsIDs()[k]].isTimed()) {
						transitionList[p->getOutputTransitionsIDs()[k]].resetDelay(
								time_seed_value);
					}
				}
				if (p->getTokenCount() != marking(p->ID, 0)) {
					std::cout
							<< "[HPLN::advance()] Warning: Token count mismatch for place "
							<< p->ID << std::endl;
				}
			}
		}
	}
	return true;
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
			pList.erase(it--);//delete the place if it's an auto-created wait place at the start of a component stream
		}
	}
	int count = 0;	//repetition count
	std::vector<Place>::iterator found_it;	//search result
	std::vector<Place>::iterator search_it;	//search window start
	for (int length = (pList.size() - 1) / 2; length > 1; length--) {
		int startPos = 1;// start position of the repetition sequence in new list
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
//		int i, j;
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
		//determine the loop transition to add the count place as i/p to it
		//these are t's that has an i/p and o/p p's of the same component and the i/p has a greater id that the o/p p
		for (unsigned int i = 0; i < transitionList.size(); i++) {
			bool found = false;
			for (unsigned int j = 0;
					j < transitionList[i].inputPlacesIDs.size(); j++) {
				for (unsigned int k = 0;
						k < transitionList[i].outputPlacesIDs.size(); k++) {
					if ((placeList[transitionList[i].inputPlacesIDs[j]].getSymbol().getComponentId()
							== placeList[transitionList[i].outputPlacesIDs[k]].getSymbol().getComponentId())
							&& (placeList[transitionList[i].inputPlacesIDs[j]].ID
									> placeList[transitionList[i].outputPlacesIDs[k]].ID)) {
						found = true;
						break;	//no need to check the rest of o/p places
					}

				}
				if (found) {
					break;//no need to check the rest of i/p places
				}
			}
			if (found){
				// check that it doesn't already have an i/p count place from previous runs
				for (unsigned int j = 0;
									j < transitionList[i].inputPlacesIDs.size(); j++){
					if (placeList[transitionList[i].inputPlacesIDs[j]].type == Place::COUNT){
						found = false;	//nope, false alarm
						break;	//no need to check the rest of i/p places
					}
				}
			}
			if (found){	//if it doesn't already have an i/p count place
				//create it
				Place p (placeCount, Symbol());	//dummy symbol
				p.type = Place::COUNT;
				p.init_token_cnt = count;
				addPlace(p);
				// bind it as input to the t being process. expr is zero because the colorset of this place is boolean
				bind(p, transitionList[i] , 1 , 0);

				// if you're reading this code...sorry :) but it's actually the easiest way
				//I hope the comments make it better though
			}
		}
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
		Transition* t = &transitionList[p2.inputTransitionsIDs[i]];	//define alias for more readable code
		if (t->marked4Deletion) {
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
		if (t->marked4Deletion) {
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
			pList.back().inputTransitionsIDs.clear();//will be reconstructed later when updating arcs
			pList.back().outputTransitionsIDs.clear();
			if (pList.back().ID != pList.size() - 1) {//if there has been deletions before this place
				pList.back().ID = pList.size() - 1;	//modify its ID to be equal to its index
				for (unsigned int j = 0; j < TPArcs.size(); j++) {
					if (TPArcs[j].placeID == placeList[i].ID) { //find arcs to the old p
						TPArcs[j].placeID = pList.back().ID; //update them to store the new ID
					}
				}
				for (unsigned int j = 0; j < PTArcs.size(); j++) { //same thing but for o/p arcs
					if (PTArcs[j].placeID == placeList[i].ID) { //find arcs from the old p
						PTArcs[j].placeID = pList.back().ID; //update them to store the new ID
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
		if (!transitionList[i].marked4Deletion) {//if this transition is to be kept
			tList.push_back(transitionList[i]);	//copy it to the new list
			tList.back().inputPlacesIDs.clear();//will be reconstructed later when updating arcs
			tList.back().outputPlacesIDs.clear();
			if (tList.back().ID != tList.size() - 1) {//if there has been deletions before this transition
				tList.back().ID = tList.size() - 1;	//modify its ID to be equal to its index
				for (unsigned int j = 0; j < TPArcs.size(); j++) {
					if (TPArcs[j].transitionID == transitionList[i].ID) { //find arcs from the old t
						TPArcs[j].transitionID = tList.back().ID; //update them to store the new ID
					}
				}
				for (unsigned int j = 0; j < PTArcs.size(); j++) { //same thing but for o/p arcs
					if (PTArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
						PTArcs[j].transitionID = tList.back().ID; //update them to store the new ID
					}
				}
			}
		} else { // if the t is to be deleted, then delete all associated arcs
			for (unsigned int j = 0; j < TPArcs.size(); j++) {
				if (TPArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
					TPArcs[j].marked4Deletion = true;	//delete arc
				}
			}
			for (unsigned int j = 0; j < PTArcs.size(); j++) {//same thing but for i/p arcs
				if (PTArcs[j].transitionID == transitionList[i].ID) { //find arcs to the old t
					PTArcs[j].marked4Deletion = true;	//delete arc
				}
			}
		}
	}
	transitionList = tList;	//replace the old t list with the new
	// Update arc lists and use them to update bindings in p's and t's
	std::vector<Arc> aList;
	for (unsigned int i = 0; i < PTArcs.size(); i++) {
		if (!PTArcs[i].marked4Deletion) {
			aList.push_back(PTArcs[i]);
			placeList[PTArcs[i].placeID].addOutputTransition(
					PTArcs[i].transitionID);
			transitionList[PTArcs[i].transitionID].addInputPlace(
					PTArcs[i].placeID);
		}
	}
	PTArcs = aList;
	aList.clear();
	for (unsigned int i = 0; i < TPArcs.size(); i++) {
		if (!TPArcs[i].marked4Deletion) {
			aList.push_back(TPArcs[i]);
			placeList[TPArcs[i].placeID].addInputTransition(
					TPArcs[i].transitionID);
			transitionList[TPArcs[i].transitionID].addOutputPlace(
					TPArcs[i].placeID);
		}
	}
	TPArcs = aList;
}

void HLPN::updateTimedTransitions(std::vector<int>& enabled_list) {
	for (unsigned int i = 0; i < enabled_list.size(); i++) {
		if (transitionList[enabled_list[i]].isTimed()) {
			transitionList[enabled_list[i]].timeRemaining -= time_step;
			if (transitionList[enabled_list[i]].timeRemaining < 0.0) {
				transitionList[enabled_list[i]].timeRemaining = 0.0;
			}
		}
	}
}

std::vector<int> HLPN::getFireList(std::vector<int>& enabled_list) {
	std::vector<int> fire_list;
	for (unsigned int i = 0; i < enabled_list.size(); i++) {
		if (transitionList[enabled_list[i]].isTimed()
				&& (transitionList[enabled_list[i]].timeRemaining > 0.0)) {
			continue;	//don't push this t and skip to the next one in the list
		}
		fire_list.push_back(enabled_list[i]);
	}
	return fire_list;
}

//std::vector<int> HLPN::getEnabledList() const {
//	std::vector<int> enabledList;
//		for (int i=0; i<placeCount; i++){	//examine each element corresponding to a place
//			if (marking(i,0) > 0){	//if this place is not empty
//				for (int j=0 ; j<placeList[i].outputTransitionsIDs.size() ; i++) {
//
//				}
//			}
//		}
//	return enabledList;
//}

bool HLPN::advance(double time_step) {
	this->time_step = time_step;
	return advance();
}

void HLPN::initPN(double time_step, double time_seed_value) {
	// inits PN for execution
	if (!constructed) {
		std::cout << "Error: you must construct or load the PN first!"
				<< std::endl;
		exit(1);
	}
	this->time_step = time_step;
	this->time_seed_value = time_seed_value;
	initMarking();
	constructIncidence();
	charVector.resize(transitionCount, 1);
	charVector.clear();
	CalculateConflictMatrix();
}

void HLPN::printExprMatrix() {
	std::cout << "Expr: " << exprMatrix << std::endl;
}

void HLPN::printTransitions(bool enabled_only) {
	if (enabled_only) {
		std::vector<int> enabled_list = getEnabledList();
		for (unsigned int i = 0; i < enabled_list.size(); i++) {
			transitionList[enabled_list[i]].printTransition();
		}
		return;
	}
	for (unsigned int i = 0; i < transitionList.size(); i++) {
		transitionList[i].printTransition();
	}
}

void HLPN::initMarking() {
	marking.resize(placeCount, 1);
	marking.clear();
	Token token;
	token.value = time_seed_value;
	placeList[0].addToken(token);
	marking(0,0) = 1;
	for (unsigned int i = 1 ; i < placeCount ; i++){
		marking(i,0) = placeList[i].init_token_cnt;
		for (unsigned int j = 0 ; j < placeList[i].init_token_cnt ; j++){
			placeList[i].addToken(Token());
		}
	}

}

void HLPN::printState() {
	printMarking();
	printPlaces();
	printTransitions();
}

} // namespace PN
