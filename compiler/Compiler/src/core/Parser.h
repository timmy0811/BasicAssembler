#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <bitset>
#include <direct.h>

#include <vector>
#include <tuple>

class Parser
{
private:
	std::ifstream m_Source;
	std::ofstream m_Out;

	std::vector < std::tuple<std::string, std::string, unsigned int>> m_InstructionsMachine;

	std::vector<std::vector<std::string>> m_Instructions;
	std::vector<std::tuple<std::string, std::vector<std::vector<std::string>>>> m_InlineFunctions;

	const int isInstruction(const std::string& instruction) const;
	const int isInline(const std::string& terminal) const;

	void CreateASM(const std::string& name);

	const std::string BinToHex(std::string& bin);

public:
	Parser() = default;
	~Parser();

	void LoadInstructions(const std::string& path);

	void LoadFromFile(const std::string& path);
	const bool ParseSource();

	const bool Build(const std::string& file, int codeSize, bool compileHex);
};

