/*
 * StreamBundle.h
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#ifndef SRC_STREAMBUNDLE_H_
#define SRC_STREAMBUNDLE_H_

#include"SymbolStream.h"

namespace PN {

class StreamBundle {
	int streamCount;
	int symbolCount;	//number of symbols in all streams
	std::vector<SymbolStream> streams;
public:
	StreamBundle();
	StreamBundle(int streamCount);
	virtual ~StreamBundle();

	void addStream(const SymbolStream& stream);
	void printAll();

	int getStreamCount() const {
		return streamCount;
	}

	const Symbol& getNextSymbol();

	const std::vector<SymbolStream>& getStreams() const {
		return streams;
	}

	int getSymbolCount() const {
		return symbolCount;
	}
};

} /* namespace PN */

#endif /* SRC_STREAMBUNDLE_H_ */
