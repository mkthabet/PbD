#include "HLPN.h"
#include "StreamBundle.h"
using namespace PN;

namespace PN {
void testHLPN() {
	//ex1
//	StreamBundle streamBundle1(2);
//	SymbolStream armStream1(1,"arm");
//	SymbolStream handStream1(2, "hand");
//	Symbol a1_1(11, 0, 4, std::vector<double>(3, 150));
//	Symbol a1_2(0, 4, 6, std::vector<double>(3, 0));
//	Symbol a1_3(12, 6, 10, std::vector<double>(3, 150));
//	Symbol h1_1(21, 0, 3, std::vector<double>(3, 150));
//	Symbol h1_2(22, 3, 5, std::vector<double>(3, 150));
//	Symbol h1_3(0, 5, 7, std::vector<double>(3, 150));
//	Symbol h1_4(23, 7, 10, std::vector<double>(3, 150));
//	armStream1.addSymbol(a1_1);
//	armStream1.addSymbol(a1_2);
//	armStream1.addSymbol(a1_3);
//	handStream1.addSymbol(h1_1);
//	handStream1.addSymbol(h1_2);
//	handStream1.addSymbol(h1_3);
//	handStream1.addSymbol(h1_4);
//	streamBundle1.addStream(armStream1);
//	streamBundle1.addStream(handStream1);

//ex2
//	StreamBundle streamBundle2(2);
//	SymbolStream armStream2(1,"arm");
//	SymbolStream handStream2(2, "hand");
//	Symbol a2_1(11, 0, 2, std::vector<double>(3, 150));
//	Symbol a2_2(12, 2, 4, std::vector<double>(3, 150));
//	Symbol a2_3(0, 4, 5, std::vector<double>(3, 150));
//	Symbol a2_4(11, 5, 8, std::vector<double>(3, 150));
//	Symbol a2_5(13, 8, 11, std::vector<double>(3, 150));
//	Symbol h2_1(21, 3, 6, std::vector<double>(3, 150));
//	Symbol h2_2(0, 6, 9, std::vector<double>(3, 150));
//	Symbol h2_3(22, 9, 13, std::vector<double>(3, 150));
//	armStream2.addSymbol(a2_1);
//	armStream2.addSymbol(a2_2);
//	armStream2.addSymbol(a2_3);
//	armStream2.addSymbol(a2_4);
//	armStream2.addSymbol(a2_5);
//	handStream2.addSymbol(h2_1);
//	handStream2.addSymbol(h2_2);
//	handStream2.addSymbol(h2_3);
//	streamBundle2.addStream(armStream2);
//	streamBundle2.addStream(handStream2);

	//ex3
	StreamBundle streamBundle1(2);
	SymbolStream armStream1(1, "arm");
	SymbolStream handStream1(2, "hand");
	Symbol a1_1(11, 0, 2, std::vector<double>(3, 150));
	Symbol a1_2(12, 2, 4, std::vector<double>(3, 0));
	Symbol a1_3(0, 4, 6, std::vector<double>(3, 150));
	Symbol a1_4(11, 6, 8, std::vector<double>(3, 150));
	Symbol a1_5(12, 8, 10, std::vector<double>(3, 0));
	Symbol a1_6(0, 10, 12, std::vector<double>(3, 150));
	Symbol a1_7(13, 12, 14, std::vector<double>(3, 150));
	Symbol a1_8(14, 14, 16, std::vector<double>(3, 150));
	Symbol a1_9(13, 16, 18, std::vector<double>(3, 150));
	Symbol a1_10(14, 18, 20, std::vector<double>(3, 150));
	Symbol a1_11(13, 20, 22, std::vector<double>(3, 150));
	Symbol a1_12(14, 22, 24, std::vector<double>(3, 150));
	Symbol h1_1(21, 1, 3, std::vector<double>(3, 150));
	Symbol h1_2(22, 3, 5, std::vector<double>(3, 150));
	Symbol h1_3(0, 5, 7, std::vector<double>(3, 150));
	Symbol h1_4(21, 7, 9, std::vector<double>(3, 150));
	Symbol h1_5(22, 9, 11, std::vector<double>(3, 150));
	Symbol h1_6(0, 11, 13, std::vector<double>(3, 150));
	Symbol h1_7(23, 13, 15, std::vector<double>(3, 150));
	Symbol h1_8(24, 15, 17, std::vector<double>(3, 150));
	Symbol h1_9(23, 17, 19, std::vector<double>(3, 150));
	Symbol h1_10(24, 19, 21, std::vector<double>(3, 150));
	Symbol h1_11(23, 21, 23, std::vector<double>(3, 150));
	Symbol h1_12(24, 23, 25, std::vector<double>(3, 150));
	Symbol h1_13(25, 25, 27, std::vector<double>(3, 150));
	armStream1.addSymbol(a1_1);
	armStream1.addSymbol(a1_2);
	armStream1.addSymbol(a1_3);
	armStream1.addSymbol(a1_4);
	armStream1.addSymbol(a1_5);
	armStream1.addSymbol(a1_6);
	armStream1.addSymbol(a1_7);
	armStream1.addSymbol(a1_8);
	armStream1.addSymbol(a1_9);
	armStream1.addSymbol(a1_10);
	armStream1.addSymbol(a1_11);
	armStream1.addSymbol(a1_12);
	handStream1.addSymbol(h1_1);
	handStream1.addSymbol(h1_2);
	handStream1.addSymbol(h1_3);
	handStream1.addSymbol(h1_4);
	handStream1.addSymbol(h1_5);
	handStream1.addSymbol(h1_6);
	handStream1.addSymbol(h1_7);
	handStream1.addSymbol(h1_8);
	handStream1.addSymbol(h1_9);
	handStream1.addSymbol(h1_10);
	handStream1.addSymbol(h1_11);
	handStream1.addSymbol(h1_12);
	handStream1.addSymbol(h1_13);
	streamBundle1.addStream(armStream1);
	streamBundle1.addStream(handStream1);
	HLPN pn;
	pn.constructPN(streamBundle1);
	pn.initPN(1.0);
	pn.printIncidence();
	pn.printConflict();
	pn.printExprMatrix();
	pn.printState();
	std::vector<Symbol> exec_syms;
	while (pn.advance()){
		pn.printState();
		if (pn.getExecSymbols(exec_syms)){
			for (unsigned int i = 0 ; i < exec_syms.size() ; i++){
				std::cout << " exec syms: \n";
				exec_syms[i].printAll();
				pn.signalExecEnd(exec_syms[i].getSymbolId());
			}
		}

	}
	std::cout << " DONE!: \n";


//	std::cout << pn.getEnabledList() << std::endl;
//	std::cout << s1 << std::endl;
//	pn.advance(s1);
//	pn.printMarking();
//	std::cout << s2 << std::endl;
//	pn.advance(s2);
//	pn.printMarking();
}

} // namespace PN

