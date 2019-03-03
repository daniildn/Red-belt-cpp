#include <string>
#include <string_view>
#include <list>
#include "test_runner.h"
#include "profile.h"

using namespace std;

class Editor {
public:
	Editor() {}

	// сдвинуть курсор влево
	void Left() {
		if (cursor != text.begin()) {
			--cursor;
		}
	}

	// сдвинуть курсор вправо
	void Right() {
		if (cursor != text.end()) {
			++cursor;
		}
	}

	// вставить символ token
	void Insert(char token) {
		text.insert(cursor, token);
		++size;
		Right();
	}

	// cкопировать не более tokens символов, начиная с текущей позиции курсора
	void Copy(size_t tokens) {
		buffer.clear();
		buffer.insert(buffer.begin(), cursor, next(cursor, tokens - 1));
	}

	// вырезать не более tokens символов, начиная с текущей позиции курсора
	void Cut(size_t tokens) {
		Copy(tokens);
		//buffer.clear();
		cursor = text.erase(cursor, next(cursor, tokens - 1));
		size -= min(size, tokens);
	}

	// вставить содержимое буфера в текущую позицию курсора
	void Paste() {
		text.insert(cursor, buffer.begin(), buffer.end());
		size += buffer.size();
		advance(cursor, buffer.size());
	}

	// получить текущее содержимое текстового редактора
	string GetText() const {
		string result;
		for (const auto& x : text) {
			result += x;
		}
		return result;
	}

private:
	list<char> text;
	list<char> buffer;
	list<char>::iterator cursor = text.begin();
	size_t size = 0;
};

void TypeText(Editor& editor, const string& text) {
	for (char c : text) {
		editor.Insert(c);
	}
}

void TestEditing() {
	{
		Editor editor;

		const size_t text_len = 12;
		const size_t first_part_len = 7;
		TypeText(editor, "hello, world");
		for (size_t i = 0; i < text_len; ++i) {
			editor.Left();
		}
		editor.Cut(first_part_len);
		for (size_t i = 0; i < text_len - first_part_len; ++i) {
			editor.Right();
		}
		TypeText(editor, ", ");
		editor.Paste();
		editor.Left();
		editor.Left();
		editor.Cut(3);

		ASSERT_EQUAL(editor.GetText(), "world, hello");
	}
	{
		Editor editor;

		TypeText(editor, "misprnit");
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(1);
		editor.Right();
		editor.Paste();

		ASSERT_EQUAL(editor.GetText(), "misprint");
	}
}

void TestReverse() {
	Editor editor;

	const string text = "esreveR";
	for (char c : text) {
		editor.Insert(c);
		editor.Left();
	}

	ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
	Editor editor;
	ASSERT_EQUAL(editor.GetText(), "");

	editor.Left();
	editor.Left();
	editor.Right();
	editor.Right();
	editor.Copy(0);
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
	Editor editor;

	editor.Paste();
	TypeText(editor, "example");
	editor.Left();
	editor.Left();
	editor.Paste();
	editor.Right();
	editor.Paste();
	editor.Copy(0);
	editor.Paste();
	editor.Left();
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "example");
}

void TestSpeed() {
	const int COUNT_OPERAION = 10000000;
	const int SIZE_TEXT = 10000000;
	const int COUNT_CHAR = 500000;

	LOG_DURATION("editor")

	Editor editor;
	for (int i = 0; i < SIZE_TEXT; ++i) {
		editor.Insert('a');
	}

	string s = editor.GetText();

	editor.Right();
	editor.Right();

	for (int i = 0; i < COUNT_OPERAION; ++i) {
		editor.Cut(COUNT_CHAR);
		editor.Paste();
		editor.Cut(0);
	}
	ASSERT_EQUAL(s, editor.GetText());
}

int main() {
	LOG_DURATION("total")
	TestRunner tr;
	RUN_TEST(tr, TestEditing);
	RUN_TEST(tr, TestReverse);
	RUN_TEST(tr, TestNoText);
	RUN_TEST(tr, TestEmptyBuffer);
	//RUN_TEST(tr, TestSpeed);
	return 0;
}
