/*
 * PetriNet.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#include "PetriNet.h"

PetriNet::PetriNet() :
		placeCount(0), transitionCount(0) {
}

PetriNet::PetriNet(int p, int t) :
		placeCount(p), transitionCount(t) {
	marking.resize(placeCount,1);
	marking.clear();
	incidence.resize(placeCount, transitionCount);
	incidence.clear();
	charVector.resize(transitionCount,1);
	charVector.clear();
}

PetriNet::PetriNet(int p, int t, const matrix<int>& m, const matrix<int>& w) : placeCount(p), transitionCount(t){
	setMarking(m);
	setIncidence(w);
}

void PetriNet::advance(const matrix<int>& charVector){
	this->charVector = charVector;
	marking = marking + prod(incidence, charVector);	// fundamental equation of PN
}

PetriNet::~PetriNet() {
	// TODO Auto-generated destructor stub
}

void PetriNet::printMarking() {
	std::cout << "Marking: " << getMarking() << std::endl;
}

std::vector<int> PetriNet::getEnabledList () const{
	std::vector<int> enabledList;
	for (int j=0; j<transitionCount; j++){	//first examine each column of incidence matrix, corresponding to transitions
		bool enabled = true;	//assume the transition is enabled by default
		for (int i=0; i<placeCount; i++){	//examine each element corresponding to a place
			if(incidence(i,j) < 0){	//if place i is an input place for transition j
				if(marking(i,0) < abs(incidence(i,j))){	//if place i does not have enough tokens
					enabled = false;	// transition j is not enabled
					break;	//no need to examine the rest of places for this transition
				}
			}
		}
		if(enabled){	//if transition j turns out to be indeed enabled
			enabledList.push_back(j);
		}
	}
	return enabledList;
}

bool PetriNet::advance() {
	charVector.clear();
	std::vector<int> fireList(getFireList());	// a list of transitions to fire initialized with enabled transitions
	// now check for conflicts in the fire list and resolve them
	for (std::vector<int>::iterator it1 = fireList.begin() ; it1 != fireList.end()-1; ++it1){	//for each enabled transition in list
//		printList(fireList);
		for (std::vector<int>::iterator it2 = it1+1 ; it2 != fireList.end(); ++it2) {	//test against following entries
			if(conflict(*it1,*it2)){
				int transitionToErase = resolveConflict(*it1,*it2);
				if(transitionToErase == *it2){
					fireList.erase(it2);
				} else {
					fireList.erase(it1);
				}
				it1--;	//restart this iteration of outer loop since deleting  entry may lead to skipping one or bad indexing
				break;
			}
		}
	}
	// convert fireList into characteristic vector
	for (unsigned int i = 0 ; i<fireList.size() ; i++){
		charVector(fireList[i],0) = 1;
	}
	advance(charVector);
	return true;
}

void PetriNet::CalculateConflictMatrix(){
	conflict.resize(transitionCount,transitionCount);
	conflict.clear();
	for (int i=0 ; i<placeCount ; i++){	//examine each place of incidence matrix
		std::vector <int> conflictList;	//vector to store list of conflicting transition for each place
		for(int j=0 ; j<transitionCount ; j++){	//examine for each transition
			if(incidence(i,j)<0){
				conflictList.push_back(j);
			}
		}
		if(conflictList.size()>1){	// if there is a conflict from input place i
			for(unsigned int k=0; k<conflictList.size()-1; k++){
				for(unsigned int l = k+1 ; l<conflictList.size(); l++){
					conflict(conflictList[k],conflictList[l]) = 1;
					conflict(conflictList[l],conflictList[k]) = 1;	//conflict matrix is symmetric with 0 diagonal
				}
			}
		}
	}
}

void PetriNet::printIncidence() {
	std::cout << "incidence: " << getIncidence() << std::endl;
}

void PetriNet::printConflict() {
	std::cout << "Conflict: " << conflict << std::endl;
}

std::vector<int> PetriNet::getFireList() {
	//trivial implementation here, just returns enabled list
	return getEnabledList();
}

int PetriNet::resolveConflict(int t1, int t2) {	//returns loser of conflict
	// trivial implementation here, just return t2
	return t2;
}

void PetriNet::printEnabledList() {
	std::vector<int> enabled = getEnabledList();
	std::cout << "Enabled: ";
	for (unsigned int i=0; i<enabled.size(); i++){
		std::cout << enabled[i] << " , ";
	}
	std::cout << std::endl;
}

void PetriNet::printList(std::vector<int> list) {
	std::cout << "list: ";
	for (unsigned int i=0; i<list.size(); i++){
		std::cout << list[i] << " , ";
	}
	std::cout << std::endl;
}
