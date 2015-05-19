/*
 * symbol.h
 *
 *  Created on: Apr 14, 2015
 *      Author: thabetm1
 */

#ifndef SRC_SYMBOL_H_
#define SRC_SYMBOL_H_

#include <vector>

class Symbol {
private:
	int symbolID;	// ID of the symbol in the stream
	int primitiveID;	// ID of the primitive associated with that symbol,
						// 0 is reserved for the stop primitive
	int componentID;	// ID of the robot component that generated this primitive
	double startTime;
	double endTime;
	std::vector<double> goalState;

public:
	Symbol();
	Symbol(int PrimitiveID, double startTime, double endTime , const std::vector<double>& goal);
	virtual ~Symbol();
	double getDuration() const;
	std::vector<double> getGoalState();
	bool operator== (const Symbol& s);
	void operator= (const Symbol& s);
	void printAll() const;

	void setSymbolId(int symbolId) {
		symbolID = symbolId;
	}

	int getSymbolId() const {
		return symbolID;
	}

	double getEndTime() const {
		return endTime;
	}

	int getPrimitiveId() const {
		return primitiveID;
	}

	double getStartTime() const {
		return startTime;
	}

	int getComponentId() const {
		return componentID;
	}

	void setComponentId(int componentId) {
		componentID = componentId;
	}
};

#endif /* SRC_SYMBOL_H_ */
