#include "test_runner.h"
#include <string>
#include <string_view>
#include <vector>

using namespace std;

class Translator {
public:
	void Add(string_view source, string_view target) {
		data_forward[source].push_back(target);
		data_backward[target].push_back(source);
	}
	string_view TranslateForward(string_view source) const {
		if (data_forward.count(source) == 1) {
			return data_forward.at(source).back();
		} else {
			return {};
		}
	}
	string_view TranslateBackward(string_view target) const {
		if (data_backward.count(target) == 1) {
			return data_backward.at(target).back();
		} else {
			return {};
		}
	}

private:
	map<string_view, vector<string_view>> data_forward;
	map<string_view, vector<string_view>> data_backward;
};

void TestSimple() {
	Translator translator;
	translator.Add(string("okno"), string("window"));
	translator.Add(string("stol"), string("table"));

	ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
	ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
	ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSimple);
	return 0;
}
