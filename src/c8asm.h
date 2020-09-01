#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#define CLEAR_SCREEN 0x00E0
#define RETURN_SUBROUTINE 0x00EE

using namespace std;

class c8asm{
private:
	FILE* fp;
	int fsize;
	char* fmem;
	string current_section;
	int address;
	vector<int> compilation;
	int line_number = 1;


	struct address_name {
		string name;
		long unsigned int return_address;
	};

	vector<address_name> memory_addresses;
	vector<address_name> subroutines;
	int subroutine_depth = 0;


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

	stack<string> address_references;
	stack<int> parser_return_values;

	void is_register_literal_valid(string literal, int param_number) {
		if (literal[0] != 'v' || literal.length() != 2)
		{
			const char format_string[104] = "Invalid argument. Argument %d must be a register. (format 'vx' where x is a hexadecimal value from 0-F)";
			char buffer[sizeof(format_string)];
			sprintf(buffer, format_string, param_number);
			fatal(buffer, line_number);
		}
	}

	void parse_register_literal(string literal) {
		int reg = stoi(string(1, literal[1]), nullptr, 16);
		parser_return_values.push(reg);
	}

	void is_integer_literal_valid(string literal, int param_number) {
		for (int i = 0; i < literal.length(); i++) {
			char c = literal[i];
			const char* decimal_characters = "0123456789";
			if (strchr(decimal_characters, c) == nullptr) {
				char error_message_buffer[100];
				sprintf(error_message_buffer, "Illegal character '%c' in integer literal at parameter %d", c, param_number);
				fatal(error_message_buffer, line_number);
			}
		}
	}

	void parse_integer_literal(string literal) {

	}

	struct LiteralType {
		const char* name;
		void (c8asm::*is_valid)(string, int);
		void (c8asm::*parser)(string);
	};

	vector<LiteralType> literal_types = {
		{
			"register",
			&c8asm::is_register_literal_valid,
			&c8asm::parse_register_literal
		},
		{
			"integer",
			&c8asm::is_integer_literal_valid,
			&c8asm::parse_integer_literal
		}
	};

	LiteralType get_literal_type(string name) {
		for (int i = 0; i < literal_types.size(); i++) {
			LiteralType literal_type = literal_types[i];
			if (literal_type.name == name) return literal_type;
		}
		cout << "Unknown LiteralType" << endl;
		exit(1);
	}

	void fatal(const char* message, int line_number) {
		char buffer[200];
		sprintf(buffer, "FATAL at %d: %s", line_number, message);
		cout << buffer << endl;
		exit(0);
	}

	void require_args(vector<string> args, vector<LiteralType> types) {
		for (int i = 0; i < types.size(); i++) {
			string arg = args[i];
			// remove carriage return
			if (arg[arg.length() - 1] == (char)13) arg = arg.substr(0, arg.length() - 1);
			cout << "arg: " << arg << endl;
			LiteralType type = types[i];
			// will exit the assembler if the literal isn't valid
			(this->*type.is_valid)(arg, i);
			(this->*type.parser)(arg);
		}
	}

	void clear_screen(vector<string> args) {
		compilation.push_back(CLEAR_SCREEN);
	}

	void set(vector<string> args) {
		string arg0 = args[0];
		string arg1 = args[1];
		require_args(args, { get_literal_type("register"), get_literal_type("integer") });
		int value, instruction;
		if (arg1[0] == '0' && arg1[1] == 'x') {
			value = stoi(arg1.substr(2, string::npos), nullptr, 16);
		}
		else {
			size_t sz;
			value = stoi(arg1, &sz);
		}
		//instruction |= reg << 8;
		instruction |= value;
		compilation.push_back(instruction);
	}

	const string COMMAND_NAMES[2] = {
		"CLRSCRN",
		"SET"
	};

	void (c8asm::*COMMAND_CALLBACKS[2])(vector<string>) = {
		&c8asm::clear_screen,
		&c8asm::set
	};

	void handle_command(string command_name, vector<string> arguments) {
		const char length = sizeof(COMMAND_NAMES) / sizeof(COMMAND_NAMES[0]);
		for (int i = 0; i < length; i++) {
			const string name = COMMAND_NAMES[i];
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
			cout << current_section << endl;
		}
		else if (first_char == '>') return;
		else if (first_char == ':') {
			subroutines.push_back(
				address_name {
					line.substr(1, string::npos),
					compilation.size() * 2
				}
			);
			subroutine_depth++;
		}
		else if (first_char == ';') {
			if (subroutine_depth == 0) fatal("Illegal return statement.", line_number);
			compilation.push_back(RETURN_SUBROUTINE);
			subroutine_depth--;
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
				line_number += 1;
				continue;
			}
			if (c != '\t' && c != ' ')
				line_buffer += c;
		}
	}

};