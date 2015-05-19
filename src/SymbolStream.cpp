/*
 * SymbolStream.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#include "SymbolStream.h"
#include <iostream>

namespace PN {

SymbolStream::SymbolStream() {
	// TODO Auto-generated constructor stub

}

SymbolStream::SymbolStream(int componentID, std::string componentName) :
		componentID(componentID), componentName(componentName), symbolIndex(0) {
}

SymbolStream::~SymbolStream() {
	// TODO Auto-generated destructor stub
}

void SymbolStream::addSymbol(Symbol& symbol) {
	symbol.setSymbolId(stream.size());
	symbol.setComponentId(componentID);
	stream.push_back(symbol);
}

void SymbolStream::printAll() {
	std::cout << "symbol stream for " << componentName << ":\n";
	for (unsigned int i = 0; i < stream.size(); i++) {
		stream[i].printAll();
	}
}

} /* namespace PN */

const Symbol& PN::SymbolStream::getNextSymbol() {
	return stream[symbolIndex++];
}
