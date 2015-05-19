/*
 * SymbolStream.h
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#ifndef SRC_SYMBOLSTREAM_H_
#define SRC_SYMBOLSTREAM_H_

#include <string>
#include "Symbol.h"

namespace PN {

class SymbolStream {
private:
	int componentID;	//ID of the robot component that generated the stream
	int symbolIndex;	// which symbol is currently being processed in this stream
	std::string componentName;
	std::vector<Symbol> stream;
public:
	SymbolStream();
	SymbolStream(int componentID, std::string componentName);
	virtual ~SymbolStream();

	void addSymbol(Symbol& symbol);
	const Symbol& getNextSymbol();
	void printAll();

	int getComponentId() const {
		return componentID;
	}

	void setComponentId(int componentId) {
		componentID = componentId;
	}

	const std::string& getComponentName() const {
		return componentName;
	}

	void setComponentName(const std::string& componentName) {
		this->componentName = componentName;
	}

	const std::vector<Symbol>& getStream() const {
		return stream;
	}

	int getSymbolIndex() const {
		return symbolIndex;
	}

	void setSymbolIndex(int symbolIndex) {
		this->symbolIndex = symbolIndex;
	}
};

} /* namespace PN */

#endif /* SRC_SYMBOLSTREAM_H_ */
