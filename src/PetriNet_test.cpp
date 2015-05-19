/*
 * PetriNet_test.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#include "PetriNet.h"

void testPN(){
	int p=5 , t=4;
	matrix<int> m(p,1);
	m.clear();
	m.insert_element(1,0,1);
	m.insert_element(3,0,2);
	matrix<int>w(p,t);
	w.clear();
	w.insert_element(0,0,-1);
	w.insert_element(0,3,1);
	w.insert_element(1,0,1);
	w.insert_element(1,1,-1);
	w.insert_element(2,1,1);
	w.insert_element(2,3,-1);
	w.insert_element(3,0,1);
	w.insert_element(3,2,-1);
	w.insert_element(4,2,1);
	w.insert_element(4,3,-1);
	w.insert_element(3,1,-1);
	PetriNet pn(p,t,m,w);
	matrix<int> s1(t,1);
	s1.clear();
	s1.insert_element(1,0,1);
	matrix<int> s2(t,1);
	s2.clear();
	s2.insert_element(0,0,1);
	s2.insert_element(2,0,2);
	s2.insert_element(3,0,1);
	pn.printMarking();
	pn.printIncidence();
	pn.printConflict();
	pn.printEnabledList();
	pn.advance();
	pn.printMarking();
	pn.advance();
	pn.printMarking();
//	std::cout << pn.getEnabledList() << std::endl;
//	std::cout << s1 << std::endl;
//	pn.advance(s1);
//	pn.printMarking();
//	std::cout << s2 << std::endl;
//	pn.advance(s2);
//	pn.printMarking();
}
