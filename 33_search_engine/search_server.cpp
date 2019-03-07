#include "search_server.h"
#include "iterator_range.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <functional>
#include <utility>

vector<string> SplitIntoWords(const string& line) {
	istringstream words_input(line);
	return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
	UpdateDocumentBaseAsync(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
	//UpdateDocumentBaseAsync(document_input);
	futures.push_back(async(launch::async, &SearchServer::UpdateDocumentBaseAsync, this, ref(document_input)));
}

void SearchServer::UpdateDocumentBaseAsync(istream& document_input) {
	InvertedIndex new_index;

	for (string current_document; getline(document_input, current_document);) {
		new_index.Add(move(current_document));
	}

	auto access = index.GetAccess();
	access.ref_to_value = move(new_index);
	//this_thread::sleep_for(3s);
	//auto access = index.GetAccess();
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
	//SearchServer::AddQueriesStreamAsync(query_input, search_results_output);
	futures.push_back(async(launch::async, &SearchServer::AddQueriesStreamAsync, this, ref(query_input), ref(search_results_output)));
}

void SearchServer::AddQueriesStreamAsync(istream& query_input, ostream& search_results_output) {
	//auto access = index.GetAccess();
	//InvertedIndex& idx = index.GetAccess().ref_to_value;
	size_t index_size = index.GetAccess().ref_to_value.Size();

	pair<size_t, int64_t> p = { 0, 0 };
	vector<pair<size_t, int64_t>> search_results;
	search_results.reserve(index_size);
	for (string current_query; getline(query_input, current_query);) {
		const auto words = SplitIntoWords(move(current_query));

		search_results.assign(index_size, p);
		for (const auto& word : words) {
			//cout << word << " Add" <<endl;
			//for (const auto& [docid, hit_count] : idx.Lookup(word)) {
			//auto access = index.GetAccess();
			for (const auto& item : index.GetAccess().ref_to_value.Lookup(word)) {
				//cout << word << ' ' << item.first <<endl;
				search_results[item.first].first += item.second;
				search_results[item.first].second = -item.first;
			}
		}

		auto middle = index_size <= 5 ? end(search_results) : begin(search_results) + 5;
		partial_sort(begin(search_results), middle, end(search_results),
				[](const pair<size_t, int64_t>& lhs, const pair<size_t, int64_t>& rhs) {
					return lhs > rhs;
				});

		search_results_output << current_query << ':';
		for (auto [hitcount, docid] : Head(search_results, 5)) {
			if (hitcount != 0) {
				search_results_output << " {"
				<< "docid: " << -docid << ", "
				<< "hitcount: " << hitcount << '}';
			}
		}
		search_results_output << endl;
	}
}

void InvertedIndex::Add(const string& document) {
	docs.push_back(document);

	const size_t docid = docs.size() - 1;
	for (const auto& word : SplitIntoWords(document)) {
		//cout << word << ' ' << docid <<endl;
		if (index[word].count(docid) == 1) {
			size_t id = index[word][docid];
			++accumulated_index[word][id].second;
		} else {
			pair<size_t, size_t> new_pair = {docid, 1};
			accumulated_index[word].push_back(move(new_pair));
			index[word][docid] = accumulated_index[word].size() - 1;
		}
	}
}

vector<pair<size_t, size_t>> InvertedIndex::Lookup(const string& word) const {
	if (auto it = accumulated_index.find(word); it != accumulated_index.end()) {
		return it->second;
	} else {
		return {};
	}
}
