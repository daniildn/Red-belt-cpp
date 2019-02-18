#include "test_runner.h"

#include <ostream>
using namespace std;

#define PRINT_VALUES(out, x, y) out << (x) << endl << (y) << endl

/*struct Time {
  int hours, minutes;
};
ostream& operator << (ostream& out, const Time& time) {
	return out << time.hours << ':' << time.minutes;
}*/

void Test() {
	{
		ostringstream output;
		PRINT_VALUES(output, 5, "red belt");
		ASSERT_EQUAL(output.str(), "5\nred belt\n");
	}
/*	{
		ostringstream output;
		PRINT_VALUES(output, (Time{17, 20}), (Time{17, 20}));
		ASSERT_EQUAL(output.str(), "17:20\n17:20\n");
	}*/
}

int main() {
  TestRunner tr;
  tr.RunTest(Test, "PRINT_VALUES usage example");
}
