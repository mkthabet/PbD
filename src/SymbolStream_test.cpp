/*
 * SymbolStream_test.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#include "StreamBundle.h"
#include <iostream>

using namespace PN;
using namespace std;

void testStream(){
	StreamBundle streamBundle(2);
	SymbolStream armStream(1,"arm");
	SymbolStream handStream(2, "hand");
	Symbol a1(1, 3, 7, *(new vector<double>(3, 150)));
	Symbol a2(2, 8, 10, *(new vector<double>(2, 100)));
	Symbol a3(3, 12, 14, *(new vector<double>(3, 150)));
	Symbol h1(11, 4, 6, *(new vector<double>(3, 150)));
	Symbol h2(12, 6, 9, *(new vector<double>(3, 150)));
	Symbol a4(3, 12, 13.9, *(new vector<double>(3, 150)));
	if (a3 == a4){
		cout << "a3 == a4\n";
	}
//	armStream.addSymbol(a1);
//	armStream.addSymbol(a2);
//	armStream.addSymbol(a3);
//	handStream.addSymbol(h1);
//	handStream.addSymbol(h2);
//	streamBundle.addStream(armStream);
//	streamBundle.addStream(handStream);
//	streamBundle.printAll();
//	streamBundle.getNextSymbol().printAll();
//	streamBundle.getNextSymbol().printAll();
//	streamBundle.getNextSymbol().printAll();
//	streamBundle.getNextSymbol().printAll();
//	streamBundle.getNextSymbol().printAll();
}
