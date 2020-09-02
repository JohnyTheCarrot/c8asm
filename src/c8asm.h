#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#define N_PREFIXED_INSTRUCTIONS 3

#pragma warning(disable:4018)
using namespace std;


class c8asm{
private:
	// reading files
	FILE* fp;
	int fsize;
	char* fmem;

	// text or data
	string current_section;

	// for the error messages
	int line_number = 1;

	struct address_name {
		string name;
		long unsigned int return_address;
	};

	// .data bytes go here
	vector<int> data;
	// compiled .text ints go here
	vector<int> compilation;

	// this one is where the addresses for the .data section go
	vector<address_name> memory_addresses;
	// subroutine names & their respective addresses go here
	vector<address_name> subroutines;

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

	// currently unused, see the comment at it's only usage
	stack<int> preprocessor_addresses;
	// when a parser of a command returns an integer value, it'll be pushed to this stack for later use
	stack<int> parser_return_values;


	// opcode enums
	enum opcodes{
		
		opcode00E0 = 0x00E0, //CLS
		opcode00EE = 0x00EE, //RET
		opcode0nnn = 0x0000, //SYS addr
		opcode1nnn = 0x1000, //JP addr
		opcode2nnn = 0x2000, //CALL addr
		opcode3xkk = 0x3000, //SE Vx, byte
		opcode4xkk = 0x4000, //SNE Vx, byte
		opcode5xy0 = 0x5000, //SE Vx, Vy
		opcode6xkk = 0x6000, //LD Vx, byte
		opcode7xkk = 0x7000, //ADD Vx, byte
		opcode8xy0 = 0x8000, //LD Vx, Vy
		opcode8xy1 = 0x8001, //OR Vx, Vy
		opcode8xy2 = 0x8002, //AND Vx, Vy
		opcode8xy3 = 0x8003, //XOR Vx, Vy
		opcode8xy4 = 0x8004, //ADD Vx, Vy
		opcode8xy5 = 0x8005, //SUB Vx, Vy
		opcode8xy6 = 0x8006, //SHR Vx {, Vy}
		opcode8xy7 = 0x8007, //SUBN Vx, Vy
		opcode8xyE = 0x800E, //SHL Vx {, Vy}
		opcode9xy0 = 0x9000, //SNE Vx, Vy
		opcodeAnnn = 0xA000, //LD I, addr
		opcodeBnnn = 0xB000, //JP V0, addr
		opcodeCxkk = 0xC000, //RND Vx, byte
		opcodeDxyn = 0xD000, //DRW Vx, Vy, nibble
		opcodeEx9E = 0xE09E, //SKP Vx
		opcodeExA1 = 0xE0A1, //SKNP Vx
		opcodeFx07 = 0xF007, //LD Vx, DT
		opcodeFx0A = 0xF00A, //LD Vx, K
		opcodeFx15 = 0xF015, //LD DT, Vx
		opcodeFx18 = 0xF018, //LD ST, Vx
		opcodeFx1E = 0xF01E, //ADD I, Vx
		opcodeFx29 = 0xF029, //LD F, Vx
		opcodeFx33 = 0xF033, //LD B, Vx
		opcodeFx55 = 0xF055, //LD [I], Vx
		opcodeFx65 = 0xF065, //LD Vx, [I]
		

	};

	/**
	##############################################################################################
	#																							 #
	#									  Literal Validators									 #
	#																							 #
	##############################################################################################
	**/

	// check if:
	//	literal starts with v
	//	literal is 2 chars long, including the 'v'
	// TODO:
	//	Limit literal to v[0-9a-fA-F]
	void is_register_literal_valid(string literal, int param_number) {
		if (literal[0] != 'v' || literal.length() != 2)
		{
			const char format_string[104] = "Invalid argument. Argument %d must be a register. (format 'vx' where x is a hexadecimal value from 0-F)";
			char buffer[sizeof(format_string)];
			sprintf(buffer, format_string, param_number);
			fatal(buffer, line_number);
		}
	}


	// starts with 0x?
	//	Yes: hexadecimal
	//	No: decimal
	//
	// hexadecimal:
	//	check if 0x[0-9a-fA-F]
	// decimal:
	//	check if [0-9]
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

	// check if:
	//	[a-z0-9_-]
	// TODO:
	//	add support for [A-Z]
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

	// convert hexadecimal part of register literal (v[0-9a-fA-F]) to usable integer
	// push usable integer to parser return value stack
	void parse_register_literal(string literal) {
		int reg = stoi(string(1, literal[1]), nullptr, 16);
		parser_return_values.push(reg);
	}
	
	// is hexadecimal? convert to int
	// push int to parser return value stack
	void parse_integer_literal(string literal) {
		int number;
		if (literal.length() > 2 && literal[0] == '0' && literal[1] == 'x') {
			number = stoi(literal.substr(2, string::npos), nullptr, 16);
		} else {
			number = stoi(literal, nullptr);
		}
		parser_return_values.push(number);
	}
	
	void parse_address_name_literal(string literal) {}

	/**
	##############################################################################################
	#																							 #
	#									  Literal Definitions									 #
	#																							 #
	##############################################################################################
	**/

	struct LiteralType {
		const char* name;
		void (c8asm::*is_address_name_literal_valid)(string, int);
		void (c8asm::*parse_address_name_literal)(string);
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

	/**
	##############################################################################################
	#																							 #
	#											  Errors										 #
	#																							 #
	##############################################################################################
	**/

	void fatal(const char* message, int line_number) {
		char buffer[200];
		sprintf(buffer, "FATAL at %d: %s", line_number, message);
		cout << buffer << endl;
		exit(0);
	}

	/**
	##############################################################################################
	#																							 #
	#											  Utils											 #
	#																							 #
	##############################################################################################
	**/

	// input: literal name
	// output: LiteralType object tied to that name
	LiteralType get_literal_type(string name) {
		for (int i = 0; i < literal_types.size(); i++) {
			LiteralType literal_type = literal_types[i];
			if (literal_type.name == name) return literal_type;
		}
		cout << "Unknown LiteralType" << endl;
		exit(1);
	}

	// resolve address name to actual address number
	// this one is for .data
	address_name get_data_address(string data_address_name) {
		for (int i = 0; i < memory_addresses.size(); i++) {
			address_name data_address = memory_addresses[i];
			if (data_address.name == data_address_name) return data_address;
		}
		char buffer[50];
		sprintf(buffer, "Unknown data address name %s", data_address_name.c_str());
		fatal(buffer, line_number);
	}

	// resolve address name to actual address number
	// this one is for .text
	address_name get_subroutine(string subroutine_name) {
		for (int i = 0; i < subroutines.size(); i++) {
			address_name subroutine = subroutines[i];
			if (subroutine.name == subroutine_name) return subroutine;
		}
		char buffer[50];
		sprintf(buffer, "Unknown subroutine %s", subroutine_name.c_str());
		fatal(buffer, line_number);
	}

	// enforce requirement for specified argument types, use before handling said arguments
	// TODO: enforce argument count
	void require_args(vector<string> args, vector<LiteralType> types) {
		for (int i = 0; i < types.size(); i++) {
			string arg = args[i];
			// remove carriage return
			if (arg[arg.length() - 1] == (char)13) arg = arg.substr(0, arg.length() - 1);
			LiteralType type = types[i];
			// will exit the assembler if the literal isn't valid
			(this->*type.is_address_name_literal_valid)(arg, i);
			(this->*type.parse_address_name_literal)(arg);
		}
	}

	/**
	##############################################################################################
	#																							 #
	#										 Command Handlers									 #
	#																							 #
	##############################################################################################
	**/

	void clear_screen(vector<string> args) {
		compilation.push_back(opcode00EE);
	}

	void set(vector<string> args) {
		require_args(args, { get_literal_type("register"), get_literal_type("integer") });
		int value = parser_return_values.top();
		parser_return_values.pop();
		int reg = parser_return_values.top();
		parser_return_values.pop();
		int instruction = opcode6xkk;
		instruction |= reg << 8;
		instruction |= value;
		compilation.push_back(instruction);
	}

	void address_based_instruction(vector<string> args, int instruction) {
		require_args(args, { get_literal_type("address_name") });
		string arg = args[0];
		address_name subroutine = get_subroutine(arg);
		compilation.push_back(instruction | subroutine.return_address);
	}

	void call_subroutine(vector<string> args) {
		address_based_instruction(args, opcode2nnn);
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
		AXNN(args, opcode3xkk);
	}

	void skip_ne(vector<string> args) {
		AXNN(args, opcode4xkk);
	}

	void add(vector<string> args) {
		AXNN(args, opcode7xkk);
	}

	void font(vector<string> args) {
		require_args(args, { get_literal_type("register") });
		int reg = parser_return_values.top();
		parser_return_values.pop();
		int instruction = opcodeFx29;
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
		int instruction = opcodeDxyn;
		instruction |= reg_x << 8;
		instruction |= reg_y << 4;
		instruction |= amount;
		compilation.push_back(instruction);
	}

	void jump(vector<string> args) {
		address_based_instruction(args, opcode1nnn);
	}

	void set_index(vector<string> args) {
		require_args(args, { get_literal_type("address_name") });
		string arg = args[0];
		address_name data_address = get_data_address(arg);
		compilation.push_back(opcodeAnnn | data_address.return_address);
	}

	struct Command {
		string name;
		void (c8asm::*handler)(vector<string>);
	};

	const vector<Command> COMMANDS = {
		
		{
			"CLS",
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
			"SE",
			&c8asm::skip_eq
		},
		{
			"SNE",
			&c8asm::skip_ne
		},
		{
			"FNT",
			&c8asm::font
		},
		{
			"DRW",
			&c8asm::draw
		},
		{
			"JP",
			&c8asm::jump
		},
		{
			"SETI",
			&c8asm::set_index
		}
	};

	void handle_command(string command_name, vector<string> arguments) {
		for (int i = 0; i < COMMANDS.size(); i++) {
			const Command command = COMMANDS[i];
			const string name = command.name;
			if (command_name == name) {
				void (c8asm::*callback_ptr)(vector<string>) = command.handler;
				(this->*callback_ptr)(arguments);
				return;
			}
		}
		char buffer[100];
		sprintf(buffer, "Unknown command '%s'", command_name.c_str());
		fatal(buffer, line_number);
	}

	void parse_line(string line) {
		char first_char = line[0];
		if (first_char == '.') {
			current_section = line.substr(1, string::npos);
		}
		else if (first_char == '>') return;
		else if (first_char == ':') {
			string name = line.substr(1, string::npos);
			// param number doesn't matter here because it never mentions it
			is_address_name_literal_valid(name, 0);
			if (current_section == "data") {
				memory_addresses.push_back(
					address_name{
						name,
						512 + data.size()
					}
				);
			} else {
				subroutines.push_back(
					address_name {
						name,
						// plus N_PREFIXED_INSTRUCTIONS for the prefixed instructions
						512 + (compilation.size() + N_PREFIXED_INSTRUCTIONS) * 2
					}
				);
			}
		}
		else if (first_char == ';') {
			if (current_section == "data") fatal("Cannot return in data section.", line_number);
			compilation.push_back(opcode00EE);
		}
		else {
			if (current_section == "data") {
				// TODO: support decimal & hexadecimal
				if (line.length() > 2 && line[0] == '0' && line[1] == 'b') {
					string binary_literal = line.substr(2, string::npos);
					for (int i = 0; i < binary_literal.length(); i++) {
						char c = binary_literal[i];
						const char* allowed_chars = "01";
						if (strchr(allowed_chars, c) == nullptr)
							fatal("Illegal character in binary literal.", line_number);
					}
					int binary_value = stoi(binary_literal, nullptr, 2);
					data.push_back(binary_value);
				}
				return;
			}
			vector<string> command_parts;
			// currently unused
			// this will be for storing an address in a stack in the assembler, and then when it's used resolve it.
			// example:
			// %ADD: v0, 1
			// JP: %
			if (first_char == '%') {
				int address_next = 512 + 2 + (compilation.size() + N_PREFIXED_INSTRUCTIONS) * 2;
				preprocessor_addresses.push(address_next);
				command_parts = split_string(line.substr(1, string::npos), ':');
			}
			else {
				command_parts = split_string(line, ':');
			}
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
			// offset all set index commands by the size of the compiled byte code
			if ((byte0 & 0xF0) == 0xA0) {
				int address = instruction & 0x0FFF;
				cout << "b: " << address << endl;
				address += compilation.size() * 2;
				cout << "a: " << address << endl;
				byte0 = 0xA0 | (address >> 8);
				byte1 = address & 0xFF;
			}
			fwrite(&byte0, sizeof(char), 1, hs);
			fwrite(&byte1, sizeof(char), 1, hs);
		}
		for (int i = 0; i < data.size(); i++) {
			char sprite_line = data[i];
			fwrite(&sprite_line, sizeof(char), 1, hs);
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
		compilation.insert(compilation.begin() + 1, 0x1000 | 512 + 2);
		compilation.insert(compilation.begin() + 1, 0x1000 | 512 + 4);
	}
};
