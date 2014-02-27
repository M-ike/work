#include <iostream>
#include <fstream>

#include "pl0lexer.h"
#include "pl0token.h"

int main(int argc, char *argv[])
{
    std::ifstream file;
    file.open(argv[1], std::ios::in);

    if (argc < 2)
    {
        std::cerr << "no file specified." << std::endl;
        return 1;
    }


    if (file.good())
    {
        file.seekg(0L, std::ios::beg);
        std::string _file = std::string();

        char line[4096];
        while (! file.eof())
        {
            // Die Datei zeilenweise auslesen
            file.getline(line, 4096);
            _file += std::string(line) + "\n";
        }
        file.close();
        PL0Lexer *pl0lexer = new PL0Lexer();
        pl0lexer->setInput(_file);
        if (!pl0lexer->lex())
        {
           std::cerr << "could not lex." << std::endl;
           return 1;
        }
        Token tok;
        while (pl0lexer->nextToken(&tok))
           std::cout << tok.type << "|" << tok.row << "|" << tok.column << "|" << tok.value << std::endl;
    }
    else
    {
        std::cerr << "file \""<< argv[1] <<"\"not found." << std::endl;
        return 1;
    }


  return 0;
}
