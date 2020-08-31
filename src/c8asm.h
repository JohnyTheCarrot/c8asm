#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class c8asm{
private:
	FILE* fp;
	int fsize;
	char* fmem;
	// 0 = data
	// 1 = text
	string current_section;

	vector<string> split_string(string str, char token) {
		string split_string_buffer;
		vector<string> split_string_vector;
		for (int i = 0; i < str.size(); i++) {
			if (str[i] == token)
			{
				split_string_vector.push_back(split_string_buffer);
				split_string_buffer.clear();
				continue;
			}
			split_string_buffer += str[i];
		}
		split_string_vector.push_back(split_string_buffer);
		split_string_buffer.clear();
		return split_string_vector;
	}

	void clear_screen(vector<string> args) {
		cout << "clear screen, yay!!!" << endl;
	}

	void set(vector<string> args) {
		cout << "nice nice, a set command" << endl;
	}

	const char COMMAND_NAMES[2][10] = {
		"CLRSCRN",
		"SET"
	};

	void (c8asm::*COMMAND_CALLBACKS[2])(vector<string>) = {
		&c8asm::clear_screen,
		& c8asm::set
	};

	void handle_command(string command_name, vector<string> arguments) {
		const char length = sizeof(COMMAND_NAMES) / sizeof(COMMAND_NAMES[0]);
		for (int i = 0; i < length; i++) {
			const char* name = COMMAND_NAMES[i];
			if (command_name == name) {
				void (c8asm::*callback_ptr)(vector<string>) = COMMAND_CALLBACKS[i];
				(this->*callback_ptr)(arguments);
			}
		}
	}

	void parse_line(string line) {
		char first_char = line[0];
		if (first_char == '.') {
			current_section = line.substr(1, string::npos);
			return;
		}
		else {
			vector<string> command_parts = split_string(line, ':');
			string command_name = command_parts[0];
			vector<string> arguments;
			if (command_parts.size() > 1) {
				arguments = split_string(command_parts[1], ',');
			}
			handle_command(command_name, arguments);
		}
	}

public:
	c8asm(const char* str) {
		if (!(fp = fopen(str, "r"))) {
			fprintf(stderr, "failed to open %s.\n", str);
			exit(1);
		}
		//file size
		fseek(fp, 0l, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0l, SEEK_SET);

		//allocate memory
		fmem = (char*)malloc(sizeof(char) * fsize);
		for (int i = 0; i < fsize; i++) {
			fmem[i] = fgetc(fp);
		}
		fclose(fp);
	}
	~c8asm() {
		free(fmem);
	}

	void parse(){
		string line_buffer;
		for (int i = 0; i < fsize; i++) {
			char c = fmem[i];
			if (c == '\n') {
				
				parse_line(line_buffer);
				line_buffer.clear();
				continue;
			}
			if (c != '\t' && c != ' ')
				line_buffer += c;
		}
	}

};