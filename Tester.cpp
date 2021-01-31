#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "lex.h"

using namespace std;
extern bool Prog(istream& in, int& line);
extern int GetErrorCount();
void RemoveDuplicates(std::vector<LexItem> &tokens);
void OrganizeTokens(std::vector<LexItem> &tokens);
void DetermineArgument(std::vector<std::string> &arguments, int index);
std::vector<std::string> CommandArguments(int argc, char**argv);

int main(int argc, char** argv) {
    std::vector<std::string> arguments = CommandArguments(argc, argv);
    
    LexItem token;
    std::ifstream file;
    file.open(arguments[0]);
    std::vector<LexItem> tokens;
    int lineNumber = 1;
    int tokenCount = 0;
    
    if (file.is_open()) {
        // Loop will break at error or end of file
        if (Prog(file, lineNumber)) {
            cout << endl;
            cout << "Successful Execution" << endl;
        } else {
            cout << endl;
            cout << "Unsuccessful Interpretation" << endl;
            cout << "Number of Error: "  << GetErrorCount() << endl;
        }
    }
    else {
        // Invalid file directory path
        std::cout << "CANNOT OPEN THE FILE " << arguments[0] << std::endl;
    }
}

std::vector<std::string> CommandArguments(int argc, char** argv) {
    std::vector<std::string> properties {"file", "false", "false", "false", "false" , "false"};
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-v")
            DetermineArgument(properties, 1);
        else if (arg == "-iconsts")
            DetermineArgument(properties, 2);
        else if (arg == "-rconsts")
            DetermineArgument(properties, 3);
        else if (arg == "-sconsts")
            DetermineArgument(properties, 4);
        else if (arg == "-ids")
            DetermineArgument(properties, 5);
        else if (arg.find(".txt") != -1) {
            if (properties[0] != "file") {
                std::cout << "ONLY ONE FILE NAME ALLOWED" << std::endl;
                std::exit(1);
            }
            properties[0] = arg;
        }
        else {
            std::cout << "UNRECOGNIZED FLAG " << arg << std::endl;
            std::exit(1);
        }
    }
    return properties;
}


void RemoveDuplicates(std::vector<LexItem> &tokens) {
    for (int i = 0; i < tokens.size() - 1; i++) {
        for (int j = i + 1; j < tokens.size(); j++) {
            if (tokens.at(i).GetLexeme() == tokens.at(j).GetLexeme()) {
                tokens.erase(tokens.begin() + j);
                i--;
                break;
            }
        }
    }
}


void OrganizeTokens(std::vector<LexItem> &tokens) {
    if (tokens.size() == 1)
        return;
    RemoveDuplicates(tokens);
    for (int i = 0; i < tokens.size(); i++) {
        for (int j = i; j < tokens.size(); j++) {
            LexItem remove = tokens.at(j);
            bool erase = false;
            if (tokens.at(i).GetToken() == ICONST) {
                int alpha = std::stoi(tokens.at(j).GetLexeme());
                int beta = std::stoi(tokens.at(i).GetLexeme());
                
                if (alpha < beta)
                    erase = true;
            }
            else if (tokens.at(i).GetToken() == RCONST) {
                int alpha = std::stof(tokens.at(j).GetLexeme());
                int beta = std::stof(tokens.at(i).GetLexeme());
                
                if (alpha < beta)
                    erase = true;
            }
            else if (tokens.at(j).GetLexeme() < tokens.at(i).GetLexeme())
                erase = true;
            
            if (erase) {
                tokens.erase(tokens.begin() + j);
                tokens.insert(tokens.begin() + i, remove);
            }
        }
    }
}


void DetermineArgument(std::vector<std::string> &arguments, int index) {
    for (int i = 1; i < arguments.size(); i++)
    arguments[i] = i == index ? "true" : "false";
}
