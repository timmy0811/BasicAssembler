#include "Parser.h"

const int Parser::isInstruction(const std::string& instruction) const
{
    for (unsigned int i = 0; i < m_InstructionsMachine.size(); i++) {
        if (instruction == std::get<0>(m_InstructionsMachine[i])) {
            return i;
        }
    }
    return -1;
}

const int Parser::isInline(const std::string& terminal) const
{
    unsigned int i = 0;
    for (auto function : m_InlineFunctions) {
        if (terminal == std::get<0>(function)) return i;
        i++;
    }

    return -1;
}

void Parser::CreateASM(const std::string& name)
{
    // Create output file
    m_Out = std::ofstream(name);  
}

const std::string Parser::BinToHex(std::string& bin)
{
    std::bitset<4> set(bin);

    std::stringstream res;
    res << std::hex << std::uppercase << set.to_ulong();
    return res.str();
}

Parser::~Parser()
{
}

void Parser::LoadInstructions(const std::string& path)
{
    std::ifstream instr(path);

    std::string line;
    while (std::getline(instr, line)) {
        m_InstructionsMachine.push_back({});

        std::istringstream lineStream(line);
        std::string terminal;

        unsigned int termIndex = 0;
        while (std::getline(lineStream, terminal, ' ')) {
            unsigned int index = m_InstructionsMachine.size() - 1;
            if (termIndex == 0) std::get<0>(m_InstructionsMachine[index]) = terminal;
            if (termIndex == 1) std::get<1>(m_InstructionsMachine[index]) = terminal;
            if (termIndex == 2) std::get<2>(m_InstructionsMachine[index]) = std::atoi(terminal.c_str());
            termIndex++;
        }
    }
}

void Parser::LoadFromFile(const std::string& path)
{
	m_Source = std::ifstream(path);
}

const bool Parser::ParseSource()
{
    enum class State{INST, OP, INLHEAD, INLBODY, DEFAULT};
    
    bool srcValid = false;

    State state = State::DEFAULT;
    State stateInl = State::DEFAULT;

    int instrIndex = 0;
    int instrIndexInl = 0;
    int instrOperants = 0;

    int inlineIndex = 0;

    std::string line;

    while (std::getline(m_Source, line, '\n')) {
        std::istringstream lineStream(line);
        std::string terminal;

        while (std::getline(lineStream, terminal, ' ')) {
            if (line.at(0) == '#') break;
            if (line == "endp") break;
            if (line == "@begin" && !srcValid) {
                srcValid = true;
                continue;
            }

            switch (state)
            {
                // Default State fetches lines outside inline functions
            case State::DEFAULT: {
                if ((instrIndex = isInstruction(terminal)) != -1) {
                    m_Instructions.push_back({ std::get<1>(m_InstructionsMachine[instrIndex]) });
                    if (instrOperants = std::get<2>(m_InstructionsMachine[instrIndex]) > 0) state = State::OP;
                }
                else if (terminal == "inline") {
                    m_InlineFunctions.push_back({});
                    state = State::INLHEAD;
                }
                else if (terminal.at(0) == ':' && (inlineIndex = isInline(terminal.substr(1))) >= 0) {
                    m_Instructions.push_back({ "~", std::to_string(inlineIndex)});
                    continue;
                }
                break;
            }
            case State::OP:
                m_Instructions[m_Instructions.size() - 1].push_back(terminal.substr(0, 4));
                m_Instructions[m_Instructions.size() - 1].push_back(terminal.substr(4, 4));
                state = State::DEFAULT;
                break;

            case State::INLHEAD:
                std::get<0>(m_InlineFunctions[m_InlineFunctions.size() - 1]) = terminal;
                state = State::INLBODY;
                break;

                // Entering Inline function introduces new inline state
            case State::INLBODY:
                if (terminal == "endf") {
                    state = State::DEFAULT;
                    continue;
                }
                switch (stateInl)
                {
                case State::DEFAULT:
                    if ((instrIndexInl = isInstruction(terminal)) != -1) {
                        std::get<1>(m_InlineFunctions[m_InlineFunctions.size() - 1]).push_back({ std::get<1>(m_InstructionsMachine[instrIndexInl]) });
                        if (instrOperants = std::get<2>(m_InstructionsMachine[instrIndexInl]) > 0) stateInl = State::OP;
                    }
                    break;
                case State::OP:
                    unsigned int arrInd = m_InlineFunctions.size() - 1;
                    std::get<1>(m_InlineFunctions[arrInd])[std::get<1>(m_InlineFunctions[arrInd]).size() - 1].push_back(terminal.substr(0, 4));
                    std::get<1>(m_InlineFunctions[arrInd])[std::get<1>(m_InlineFunctions[arrInd]).size() - 1].push_back(terminal.substr(4, 4));
                    stateInl = State::DEFAULT;
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

    m_Source.close();
	return srcValid;
}

const bool Parser::Build(const std::string& file, int codeSize, bool compileHex)
{
    _mkdir("out");
    CreateASM("out/" + file);

    bool writingFunc = false;
    unsigned int inlFuncIndex = 0;
    size_t linesWritten = 0;

    for (auto instructionStream : m_Instructions) {
        for (std::string& expression : instructionStream) {
            // Detect Inline function
            if (expression.at(0) == '~') {
                writingFunc = true;
                continue;
            }

            // Writing inline function
            if (writingFunc) {
                inlFuncIndex = std::atoi(expression.c_str());
                for (auto instructionStreamInl : std::get<1>(m_InlineFunctions[inlFuncIndex])) {
                    for (std::string& expressionInl : instructionStreamInl) {
                        m_Out << ((compileHex) ? BinToHex(expressionInl) : expressionInl) << "\n";
                        linesWritten++;
                    }
                }
            }
            // Writing expression
            else {
                m_Out << ((compileHex) ? BinToHex(expression) : expression) << "\n";
                linesWritten++;
            }
        }
    }

    // Fill empty code with ends
    if (codeSize - linesWritten >= 0) {
        std::string str = "1111";
        for (size_t i = 0; i < codeSize - linesWritten - 1; i++) {
            m_Out << ((compileHex) ? BinToHex(str) : str) << "\n";

        }

        if(codeSize - linesWritten > 0) m_Out << ((compileHex) ? BinToHex(str) : str);
    }
    else {
        m_Out.close();
        return false;
    }

    m_Out.close();
    return true;
}
