#include "test_runner.h"
#include "profile.h"

#include <map>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

struct Stats {
	map<string, int> word_frequences;

	void operator +=(const Stats& other) {
		for (const auto& word : other.word_frequences) {
			word_frequences[word.first] += word.second;
		}
	}
};

vector<string> ParseLine(const string& line) {
	vector<string> words;
	auto it_prev = line.begin();
	for (auto it_cur = find(line.begin(), line.end(), ' '); it_cur != line.end(); it_cur = find(it_cur, line.end(), ' ')) {
		words.push_back({ it_prev, it_cur });
		it_prev = ++it_cur;
	}
	words.push_back({ it_prev, line.end() });
	return words;
}

Stats ExploreLine(const set<string>& key_words, const string& line) {
	map<string, int> word_frequences;
	vector<string> parsed_line = ParseLine(line);
	for (const auto& word : parsed_line) {
		if (key_words.find(word) != key_words.end()) {
			++word_frequences[word];
		}
	}
	return {word_frequences};
}

Stats ExploreKeyWordsSingleThread(const set<string>& key_words, istream& input) {
	Stats result;
	for (string line; getline(input, line);) {
		result += ExploreLine(key_words, line);
	}
	return result;
}
Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
	vector<future<Stats>> futures;
	Stats result;
	for (int i = 0; i < 5; ++i) {
		string in;
		string line;
		for (int i = 0; i < 10000 && getline(input, line); ++i) {
			in += line;
		}
		istringstream ss(in);
		futures.push_back(async(ExploreKeyWordsSingleThread, ref(key_words), ref(ss)));
	}
	string in;
	for (string line; getline(input, line);) {
		in += line;
	}
	istringstream ss(in);
	futures.push_back(async(ExploreKeyWordsSingleThread, ref(key_words), ref(ss)));
	for (auto& f : futures) {
	    result += f.get();
	}
	return result;
}

void TestBasic() {
	const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

	stringstream ss;
	ss << "this new yangle service really rocks\n";
	ss << "It sucks when yangle isn't available\n";
	ss << "10 reasons why yangle is the best IT company\n";
	ss << "yangle rocks others suck\n";
	ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

	const auto stats = ExploreKeyWords(key_words, ss);
	const map<string, int> expected = {
								{"yangle", 6},
								{"rocks", 2},
								{"sucks", 1}
								};
	ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestBasic);
}
