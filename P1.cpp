#include <fstream>
#include <string>
#include <algorithm>
#include "tinydir.h"
#include <list>
#include <iostream>
#include "tokens.hpp"

using namespace std;


int main( int argc, char **argv )
{
    if(argc < 2)
        cout<<"useage:"<<endl<<"P1 mode"<<endl<<"mode = bool/positional"<<endl;
    list<string> liste = tokenize("CorpusUTF8/Rumpelstilzchen.txt");
    return 0;
}