#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <vector>

using namespace std;

struct BookingEvent {
	long int time;
	int client_id;
	int room_count;
};

class BookingManager {
public:
	void Book(long int time, const string& hotel_name, int client_id, int room_count) {
		while (!global_times.empty() && global_times.front() <= time - 86400) {
			global_times.pop();
		}

		global_times.push(time);
		Adjust(hotel_name);

		times[hotel_name].push({ time, client_id, room_count });

		rooms_count[hotel_name] += room_count;
		unique_clients[hotel_name].insert(client_id);
	}
	int ClientsCount(const string& hotel_name) {
		if (unique_clients.count(hotel_name) == 0) {
			return 0;
		}
		Adjust(hotel_name);
		return unique_clients.at(hotel_name).size();
	}
	int RoomsCount(const string& hotel_name) {
		if (rooms_count.count(hotel_name) == 0) {
			return 0;
		}
		Adjust(hotel_name);
		return rooms_count.at(hotel_name);
	}

private:
	queue<long int> global_times;
	map<string, queue<BookingEvent>> times;
	map<string, set<int>> unique_clients;
	map<string, int> rooms_count;

	void Adjust(const string& hotel_name) {
		queue<BookingEvent>& hotel_times = times[hotel_name];
		set<int>& hotel_unique_clients = unique_clients[hotel_name];
		while (!hotel_times.empty() && hotel_times.front().time < global_times.front()) {
			rooms_count[hotel_name] -= hotel_times.front().room_count;
			hotel_unique_clients.erase(hotel_times.front().client_id);
			hotel_times.pop();
		}
	}
};

int main() {
	int q;
	cin >> q;

	BookingManager bm;

	for (int i = 0; i < q; ++i) {
		string type_question;
		cin >> type_question;
		if (type_question == "BOOK") {
			long int time;
			string hotel_name;
			int client_id, room_count;
			cin >> time >> hotel_name >> client_id >> room_count;
			bm.Book(time, hotel_name, client_id, room_count);
		} else if (type_question == "CLIENTS") {
			string hotel_name;
			cin >> hotel_name;
			cout << bm.ClientsCount(hotel_name) << endl;
		} else if (type_question == "ROOMS") {
			string hotel_name;
			cin >> hotel_name;
			cout << bm.RoomsCount(hotel_name) << endl;
		}
	}

	return 0;
}
