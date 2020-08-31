#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void read_file(string file_name)
{
	fstream file;
	file.open(file_name.c_str(), ios_base::in);
	if (file.is_open())
	{
		cout << "open" << endl;
		string line;
		while (getline(file, line))
		{
			cout << line << endl;
		}
	}
	else
	{
		cout << "bork" << endl;
	}
}

int main()
{
	read_file("../test/test.asm");
	return 0;
}