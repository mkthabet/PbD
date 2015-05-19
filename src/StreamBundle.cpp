/*
 * StreamBundle.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: thabetm1
 */

#include "StreamBundle.h"
#include <iostream>

namespace PN {

StreamBundle::StreamBundle() :
		streamCount(0), symbolCount(0) {
	// TODO Auto-generated constructor stub

}

StreamBundle::StreamBundle(int streamCount) {
	streams.reserve(streamCount);
	this->streamCount = 0;
	symbolCount = 0;
}

StreamBundle::~StreamBundle() {
	// TODO Auto-generated destructor stub
}

void StreamBundle::addStream(const SymbolStream& stream) {
	streams.push_back(stream);
	streamCount++;
	symbolCount += stream.getStream().size();
}

void StreamBundle::printAll() {
	for (unsigned int i = 0; i < streams.size(); i++) {
		streams[i].printAll();
	}
}

} /* namespace PN */

const Symbol& PN::StreamBundle::getNextSymbol() {//returns next earliest symbol
	int streamIndex = 0;// which stream has the earliest next symbol, initialized with the first stream
	double earliestTime;
	bool checked = false; // to check if there is still a next symbol
	// initialize earliest symbol by the start time of the next symbol in the first available stream
	for (int i = 0; i < streamCount; i++) {	// go thru all streams
		if (streams[i].getSymbolIndex() < streams[i].getStream().size()) {// check if stream still has unporcessed symbols
				// initialize earliest symbol start time with that of the next symbol in this stream
			earliestTime =
					streams[i].getStream()[streams[i].getSymbolIndex()].getStartTime();
			streamIndex = i;
			checked = true;	// there was at least one stream with unprocessed symbols
			break;	// already initialized, no need to continue
		}
	}
	for (int i = streamIndex; i < streamCount; i++) {// go thru remaining streams
		if (streams[i].getSymbolIndex() < streams[i].getStream().size()) {
			// get the start time of the next symbol of this stream
			double time2Compare =
					streams[i].getStream()[streams[i].getSymbolIndex()].getStartTime();
			if (time2Compare < earliestTime) {// if next symbol in this stream is earlier
				streamIndex = i;	//choose this stream
				earliestTime = time2Compare;//  compare the rest to this time
			}
		}
	}
	if (!checked) {
		std::cout
				<< "[StreamBundle.getNextSymbol()] WARNING: no more symbols to process, returning dummy symbol \n";
		Symbol s;
		return s;
	}
	return streams[streamIndex].getNextSymbol();
}
