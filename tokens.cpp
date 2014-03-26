#include "tokens.hpp"
using namespace std;
list<string> tokenize(string filename)
{
    list<string> tokens;
    string line, text;
    ifstream file(filename.c_str());
    stringstream input;

    if (file.is_open())
    {
        while ( getline (file, line) )
        {
            input << line << '\n';
        }
        file.close();
    }
    text = input.str();
    replace( text.begin(), text.end(), '\n', ' ');
    replace( text.begin(), text.end(), '\r', ' ');
    replace( text.begin(), text.end(), ',', ' ');
    replace( text.begin(), text.end(), ';', ' ');
    replace( text.begin(), text.end(), ':', ' ');
    replace( text.begin(), text.end(), '!', ' ');
    replace( text.begin(), text.end(), '?', ' ');
    replace( text.begin(), text.end(), '"', ' ');
    replace( text.begin(), text.end(), '-', ' ');
    replace( text.begin(), text.end(), '.', ' ');

    size_t prev = 0;
    size_t next = 0;
    string token;
    while ((next = text.find_first_of(' ', prev)) != string::npos)
    {
        if (next - prev != 0)
        {
            token = text.substr(prev, next - prev);
            if(token.find_first_not_of(' ') != string::npos)
            {
                
                transform(token.begin(), token.end(), token.begin(), ::tolower);
                tokens.push_back(token);
            }
        }
        prev = next + 1;
    }

    if (prev < text.size())
    {
        token=text.substr(prev);
        string::iterator end_pos = remove(token.begin(), token.end(), ' ');
        token.erase(end_pos, token.end());
        if(token.find_first_not_of(' ') != std::string::npos)
            {
                
                transform(token.begin(), token.end(), token.begin(), ::tolower);
                tokens.push_back(token);
            }
    }
    return tokens;
}