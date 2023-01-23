#include <sstream>
#include <iostream>

#include "core/Parser.h"

int main(int argc, char *argv[]) {
	Parser c_Parser{};

	/*c_Parser.LoadInstructions("res/MachineInstr.inst");
	c_Parser.LoadFromFile("res/source.gt");
	c_Parser.ParseSource();

	if (!c_Parser.Build("program.asm", 256, false)) {
		std::cout << "Could not compile successfully! Output exceeds RAM capacity!" << std::endl;
	}*/

	if (argc != 5) {
		std::cout << "Missing arguments!" << std::endl;
		return -1;
	}

	c_Parser.LoadInstructions(argv[1]);
	c_Parser.LoadFromFile(argv[2]);
	c_Parser.ParseSource();

	bool b;
	std::istringstream(argv[4]) >> b;

	if (!c_Parser.Build("program.asm", std::atoi(argv[3]), b)) {
		std::cout << "Could not compile successfully! Output exceeds RAM capacity!" << std::endl;
		return -1;
	}

	return 0;
}