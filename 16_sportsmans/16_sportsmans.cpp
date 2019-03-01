#include <iostream>
#include <list>
#include <vector>

using namespace std;

int main() {
	const int MAX_COUNT = 100000;

	list<int> sportsmans;
	vector<list<int>::iterator> iterators(MAX_COUNT, sportsmans.end());

	int n;
	cin >> n;
	for (int i = 0; i < n; ++i) {
		int first, second;
		cin >> first >> second;
		iterators[first] = sportsmans.insert(iterators[second], first);
	}

	for (const auto& sportsman : sportsmans) {
		cout << sportsman << ' ';
	}

	return 0;
}
