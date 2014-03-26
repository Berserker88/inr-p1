#include "tokens.h"
using namespace std;
list<string> tokenize(string filename)
{
    list<string> tokens;
    string line,text;
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
    text=input.str();
    replace( text.begin(), text.end(), '.', ' ');
    replace( text.begin(), text.end(), ',', ' ');
    replace( text.begin(), text.end(), ';', ' '); 
    replace( text.begin(), text.end(), ':', ' ');
    replace( text.begin(), text.end(), '!', ' ');
    replace( text.begin(), text.end(), '?', ' ');
    replace( text.begin(), text.end(), '"', ' ');
    cout<<text;// replace all 'x' to 'y'
    return tokens;
}