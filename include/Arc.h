/*
 * Arc.h
 *
 *  Created on: Apr 20, 2015
 *      Author: thabetm1
 */

#ifndef SRC_ARC_H_
#define SRC_ARC_H_

namespace PN {

class Arc {
	friend class HLPN;
private:
	int ID;
	int placeID;
	int transitionID;
	int weight;
	double expression;
	bool marked4Deletion;
public:
	Arc();
	Arc(int ID, int pID, int tID, int weight, double expr);

	virtual ~Arc();
};

} /* namespace PN */

#endif /* SRC_ARC_H_ */
