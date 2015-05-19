#include <iostream>

#include "HLPN.h"
using namespace std;
using namespace PN;

extern void testPN();
namespace PN{
extern void testHLPN();
}

extern void testStream();

int main() {

//	testuBlas();
//	testPN();
	PN::testHLPN();
//	testStream();
	return 0;
	}

