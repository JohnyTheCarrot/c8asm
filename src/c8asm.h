#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#define CLEAR_SCREEN 0x00E0
#define RETURN_SUBROUTINE 0x00EE
#define N_PREFIXED_INSTRUCTIONS 2

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
		int start_index = 0;
		bool hexadecimal = false;
		if (literal.length() > 2 && literal[0] == '0' && literal[1] == 'x') {
			start_index = 2;
			hexadecimal = true;
		}
		for (int i = start_index; i < literal.length(); i++) {
			char c = tolower(literal[i]);
			const char* decimal_characters;
			if (hexadecimal)
				decimal_characters = "0123456789abcdef";
			else
				decimal_characters = "0123456789";
			if (strchr(decimal_characters, c) == nullptr) {
				char error_message_buffer[100];
				sprintf(error_message_buffer, "Illegal character '%c' in integer literal at parameter %d", c, param_number);
				fatal(error_message_buffer, line_number);
			}
		}
	}

	void parse_integer_literal(string literal) {
		int number;
		if (literal.length() > 2 && literal[0] == '0' && literal[1] == 'x') {
			number = stoi(literal.substr(2, string::npos), nullptr, 16);
		} else {
			number = stoi(literal, nullptr);
		}
		parser_return_values.push(number);
	}

	void is_address_name_literal_valid(string literal, int param_number) {
		const char* allowed_characters = "abcdefghijklmnopqrstuvwxyz0123456789_-";
		for (int i = 0; i < literal.length(); i++) {
			char c = literal[i];
			if (strchr(allowed_characters, c) == nullptr) {
				char buffer[100];
				sprintf(buffer, "Illegal character '%c' at %d", c, i);
				fatal(buffer, line_number);
			}
		}
	}

	void parse_address_name_literal(string literal) {
		address_references.push(literal);
	}

	struct LiteralType {
		const char* name;
		void (c8asm::*is_valid)(string, int);
		void (c8asm::*parser)(string);
	};

	vector<LiteralType> literal_types = {
		{
			"address_name",
			&c8asm::is_address_name_literal_valid,
			&c8asm::parse_address_name_literal
		},
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

	address_name get_subroutine(string subroutine_name) {
		for (int i = 0; i < subroutines.size(); i++) {
			address_name subroutine = subroutines[i];
			if (subroutine.name == subroutine_name) return subroutine;
		}
		char buffer[50];
		sprintf(buffer, "Unknown subroutine %s", subroutine_name.c_str());
		fatal(buffer, line_number);
	}

	void require_args(vector<string> args, vector<LiteralType> types) {
		for (int i = 0; i < types.size(); i++) {
			string arg = args[i];
			// remove carriage return
			if (arg[arg.length() - 1] == (char)13) arg = arg.substr(0, arg.length() - 1);
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
		require_args(args, { get_literal_type("register"), get_literal_type("integer") });
		int value = parser_return_values.top();
		parser_return_values.pop();
		int reg = parser_return_values.top();
		parser_return_values.pop();
		int instruction = 0x6000;
		instruction |= reg << 8;
		instruction |= value;
		compilation.push_back(instruction);
	}

	void call_subroutine(vector<string> args) {
		require_args(args, { get_literal_type("address_name") });
		string arg = args[0];
		address_name subroutine = get_subroutine(arg);
		compilation.push_back(0x2000 | subroutine.return_address);
	}

	// where A is the instruction
	void AXNN(vector<string> args, int instruction) {
		require_args(args, { get_literal_type("register"), get_literal_type("integer") });
		int value = parser_return_values.top();
		parser_return_values.pop();
		int reg = parser_return_values.top();
		parser_return_values.pop();
		instruction |= reg << 8;
		instruction |= value;
		compilation.push_back(instruction);
	}

	void skip_eq(vector<string> args) {
		AXNN(args, 0x3000);
	}

	void skip_ne(vector<string> args) {
		AXNN(args, 0x4000);
	}

	void add(vector<string> args) {
		AXNN(args, 0x7000);
	}

	void font(vector<string> args) {
		require_args(args, { get_literal_type("register") });
		int reg = parser_return_values.top();
		parser_return_values.pop();
		int instruction = 0xF029;
		instruction |= reg << 8;
		compilation.push_back(instruction);
	}

	void draw(vector<string> args) {
		require_args(args, { get_literal_type("register"), get_literal_type("register"), get_literal_type("integer") });
		int amount = parser_return_values.top();
		parser_return_values.pop();
		int reg_y = parser_return_values.top();
		parser_return_values.pop();
		int reg_x = parser_return_values.top();
		parser_return_values.pop();
		int instruction = 0xD000;
		instruction |= reg_x << 8;
		instruction |= reg_y << 4;
		instruction |= amount;
		compilation.push_back(instruction);
	}

	struct Command {
		string name;
		void (c8asm::*handler)(vector<string>);
	};

	const vector<Command> COMMANDS = {
		{
			"CLRSCRN",
			&c8asm::clear_screen
		},
		{
			"SET",
			&c8asm::set
		},
		{
			"ADD",
			&c8asm::add
		},
		{
			"CALL",
			&c8asm::call_subroutine
		},
		{
			"SKIP_EQ",
			&c8asm::skip_eq
		},
		{
			"SKIP_NE",
			&c8asm::skip_ne
		},
		{
			"FONT",
			&c8asm::font
		},
		{
			"DRAW",
			&c8asm::draw
		}
	};

	void handle_command(string command_name, vector<string> arguments) {
		for (int i = 0; i < COMMANDS.size(); i++) {
			const Command command = COMMANDS[i];
			const string name = command.name;
			if (command_name == name) {
				void (c8asm::*callback_ptr)(vector<string>) = command.handler;
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
			string name = line.substr(1, string::npos);
			// param number doesn't matter here because it never mentions it
			is_address_name_literal_valid(name, 0);
			subroutines.push_back(
				address_name {
					name,
					// plus N_PREFIXED_INSTRUCTIONS for the prefixed instructions
					512 + (compilation.size() + N_PREFIXED_INSTRUCTIONS) * 2
				}
			);
			subroutine_depth++;
		}
		else if (first_char == ';') {
			// idk, sometimes you want to return early, and that would make this line annoying.
			// if (subroutine_depth == 0) fatal("Illegal return statement.", line_number);
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

	void save(const char* filename) {
		FILE *hs;

		/* create the file and write the value */
		printf("Writing bytes..\n");
		hs = fopen(filename,"w");
		if (hs == NULL)
		{
			fprintf(stderr,"Error writing to %s\n",filename);
			return;
		}
		for (int i = 0; i < compilation.size(); i++) {
			short instruction = compilation[i];
			char byte0 = (instruction & 0xFF00) >> 8;
			char byte1 = instruction & 0xFF;
			fwrite(&byte0, sizeof(char), 1, hs);
			fwrite(&byte1, sizeof(char), 1, hs);
		}
		fclose(hs);
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
			if (c != '\t' && c != ' ' && c != 13)
				line_buffer += c;
		}
		call_subroutine({ "main" });
		compilation.insert(compilation.begin(), compilation[compilation.size()-1]);
		compilation.pop_back();
		compilation.insert(compilation.begin()+1, 0x1000 | 512+(compilation.size()*2)+2);
	}
};
