#include "stats.h"

using namespace std;

Stats::Stats() {
	method_stats[UNKNOWN_METHOD];
	for (const string_view& x : METHODS) {
		method_stats[x];
	}
	uri_stats[UNKNOWN_URI];
	for (const string_view& x : URIS) {
		uri_stats[x];
	}
}
void Stats::AddMethod(string_view method) {
	auto it = find(METHODS.begin(), METHODS.end(), method);
	if (it == METHODS.end()) {
		++method_stats[UNKNOWN_METHOD];
	} else {
		++method_stats[*it];
	}
}
void Stats::AddUri(string_view uri) {
	auto it = find(URIS.begin(), URIS.end(), uri);
	if (it == URIS.end()) {
		++uri_stats[UNKNOWN_URI];
	} else {
		++uri_stats[*it];
	}
}
const map<string_view, int>& Stats::GetMethodStats() const {
	return method_stats;
}
const map<string_view, int>& Stats::GetUriStats() const {
	return uri_stats;
}

HttpRequest ParseRequest(string_view line) {
	const size_t count = 3;
	vector<string_view> reqs(count, "");
	line.remove_prefix(min(line.find_first_not_of(' '), line.size()));
	for (size_t i = 0; i < count; ++i) {
		const size_t space = line.find(' ');
		reqs[i] = line.substr(0, space);

		if (space == line.npos) {
			break;
		} else {
			line.remove_prefix(space + 1);
		}
	}
	return {reqs[0], reqs[1], reqs[2]};
}

