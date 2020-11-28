/*

*/

#include <iostream>
#include <TlHelp32.h>

using namespace std;

void printUsage();
void procNameToPID();

int main(int argc, char **argv) {
	if (argc != 3)
	{
		printUsage();
	}
}