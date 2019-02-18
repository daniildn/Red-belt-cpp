#include "airline_ticket.h"
#include "test_runner.h"

#include <iostream>

using namespace std;

bool operator < (const Date& lhs, const Date& rhs) {
	return tie(lhs.year, lhs.month, lhs.day) < tie(rhs.year, rhs.month, rhs.day);
}
bool operator == (const Date& lhs, const Date& rhs) {
	return tie(lhs.year, lhs.month, lhs.day) == tie(rhs.year, rhs.month, rhs.day);
}
ostream& operator << (ostream& out, const Date& date) {
	return out << date.year << '-' << date.month << '-' << date.day;
}
istream& operator >> (istream& in, Date& date) {
	in >> date.year;
	in.ignore(1);
	in >> date.month;
	in.ignore(1);
	in >> date.day;
	return in;
}

bool operator < (const Time& lhs, const Time& rhs) {
	return tie(lhs.hours, lhs.minutes) < tie(rhs.hours, rhs.minutes);
}
bool operator == (const Time& lhs, const Time& rhs) {
	return tie(lhs.hours, lhs.minutes) == tie(rhs.hours, rhs.minutes);
}
ostream& operator << (ostream& out, const Time& time) {
	return out << time.hours << ':' << time.minutes;
}
istream& operator >> (istream& in, Time& time) {
	in >> time.hours;
	in.ignore(1);
	in >> time.minutes;
	return in;
}

#define UPDATE_FIELD(ticket, field, values) {	\
	auto it = values.find(#field);				\
	if (it != values.end()) {					\
	  istringstream is(it->second);				\
	  is >> ticket.field;						\
	}											\
}

void TestUpdate() {
  AirlineTicket t;
  t.price = 0;

  const map<string, string> updates1 = {
    {"departure_date", "2018-2-28"},
    {"departure_time", "17:40"},
  };
  UPDATE_FIELD(t, departure_date, updates1);
  UPDATE_FIELD(t, departure_time, updates1);
  UPDATE_FIELD(t, price, updates1);

  ASSERT_EQUAL(t.departure_date, (Date{2018, 2, 28}));
  ASSERT_EQUAL(t.departure_time, (Time{17, 40}));
  ASSERT_EQUAL(t.price, 0);

  const map<string, string> updates2 = {
    {"price", "12550"},
    {"arrival_time", "20:33"},
  };
  UPDATE_FIELD(t, departure_date, updates2);
  UPDATE_FIELD(t, departure_time, updates2);
  UPDATE_FIELD(t, arrival_time, updates2);
  UPDATE_FIELD(t, price, updates2);

  // updates2 не содержит ключей "departure_date" и "departure_time", поэтому
  // значения этих полей не должны измениться
  ASSERT_EQUAL(t.departure_date, (Date{2018, 2, 28}));
  ASSERT_EQUAL(t.departure_time, (Time{17, 40}));
  ASSERT_EQUAL(t.price, 12550);
  ASSERT_EQUAL(t.arrival_time, (Time{20, 33}));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestUpdate);
}
