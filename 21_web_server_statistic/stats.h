#pragma once

#include "http_request.h"

#include <string_view>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class Stats {
public:
	Stats();
	void AddMethod(string_view method);
	void AddUri(string_view uri);
	const map<string_view, int>& GetMethodStats() const;
	const map<string_view, int>& GetUriStats() const;

private:
	const vector<string_view> METHODS = { "GET", "POST", "PUT", "DELETE" };
	const vector<string_view> URIS = { "/", "/order", "/product", "/basket", "/help" };
	const string_view UNKNOWN_METHOD = "UNKNOWN";
	const string_view UNKNOWN_URI = "unknown";

	map<string_view, int> method_stats;
	map<string_view, int> uri_stats;
};

HttpRequest ParseRequest(string_view line);
