#include <string>
#include <vector>
using namespace std;

#define UNIQ_ID3(line) line_ ## line
#define UNIQ_ID2(line) UNIQ_ID3(line)
#define UNIQ_ID UNIQ_ID2(__LINE__)// Реализуйте этот макрос так, чтобы функция main компилировалась

int main() {
	int UNIQ_ID = 0;
	string UNIQ_ID = "hello";
	vector<string> UNIQ_ID = {"hello", "world"};
	vector<int> UNIQ_ID = {1, 2, 3, 4};
}
