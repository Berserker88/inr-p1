#include <iostream>
#include <ctime>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"
#include "Posting.h"
#include "QueryParser.h"
#include <sys/time.h>



using namespace std;


void printRes(list<Posting> pl, float time)
{
    cout.precision(3);
    cout << pl.size() << " results  -  took " << time << " ms" << endl;
    list<Posting>::iterator p_iter;
    for (p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
        cout << p_iter->toString() << endl;
    cout << endl;
}



double second()
{
    struct timeval tm;
    double t ;
    gettimeofday(&tm, NULL);
    t = (double) (tm.tv_sec) + ((double) (tm.tv_usec)) / 1.0e6;
    return t;
}


int main( int argc, char **argv )
{
    if (argc < 2)
    {
        cout << "usage: P1 <mode>" << endl << "\t <mode>= bool or positional or fuzzy" << endl;
        exit(0);
    }
    string mode = argv[1];
    // create documents from directory path
    list<Document *> docs = Document::getDocsFromDir("CorpusUTF8");
    // print documents loaded
    cout << "num of docs = " << docs.size() << endl;
    // create index dictionary
    IndexDict dict;


    double start, stop;
    if (mode.compare("bool") == 0)
    {
    
	    // build index of document list
	    dict.makeIndexFromList(docs);
	    QueryParser qp(&dict);
	    string query;
        while (1)
        {
            cout << "\n\nenter your query: AND=(,) OR=()() q=exit" << endl;
            getline(cin, query);
            if (query == "q")
                break;

            list<Posting> result;
            start = second();
            result = qp.parseAndExecute(query);
            stop = second();
            cout << (float) start << endl;
            cout << (float) stop << endl;
            printRes(result, (stop - start) * 1000);
        }
    }
    else if(mode.compare("positional") == 0){
	    // build index of document list
	    dict.makeIndexFromList(docs);
	    QueryParser qp(&dict);
	    string query;
    	while (1)
        {
            cout << "\n\nenter your query: q=exit" << endl;
            getline(cin, query);
            if (query == "q")
                break;
            list<Posting> result;
            start = second();
            result = qp.positionalParseAndExecute(query);
            stop = second();
            cout << (float) start << endl;
            cout << (float) stop << endl;
            printRes(result, (stop - start) * 1000);
        }
    }
    else if(mode.compare("fuzzy") == 0){
	    // build index of document list
	    dict.makeFuzzyIndexFromList(docs);
	    QueryParser qp(&dict);
	    string query;
    	while (1)
        {
            cout << "\n\nenter your query: q=exit" << endl;
            getline(cin, query);
            if (query == "q")
                break;
            list<Posting> result;
            start = second();
            result = qp.fuzzyParseAndExecute(query);
            stop = second();
            cout << (float) start << endl;
            cout << (float) stop << endl;
            printRes(result, (stop - start) * 1000);
        }
    }
    else{
    	cout << "usage: P1 <mode>" << endl << "\t <mode>= bool or positional" << endl;
        exit(0);
    }
    return 0;
}
