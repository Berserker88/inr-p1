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
	cout << pl.size() << " results  -  took " << time << " ms" << endl << endl;
	list<Posting>::iterator p_iter;
	int num = 1;
	for (p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
	{
		cout << num++ << ".\t" << p_iter->toString() << endl;
	}
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
		cout << "usage: P1 <mode> [-j <value>]" << endl << "\t <mode>= bool or positional or fuzzy" << endl;
		exit(0);
	}
	string mode = argv[1];
	string corpus = "CorpusUTF8";

	double thresh_j = 0.7;
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			
			if(strcmp(argv[i], "-j") == 0 && argc >= i+1)
			{
				thresh_j = atof(argv[++i]);
			}
			else if(strcmp(argv[i], "-corpus") == 0 && argc >= i+1)
			{
				corpus = argv[++i];
			}
		}
	}

	// create documents from directory path
	list<Document *> docs = Document::getDocsFromDir(corpus);
	// print documents loaded
	cout << "num of docs = " << docs.size() << endl;
	// create index dictionary
	IndexDict dict;

	dict.setThresholdJaccard(thresh_j);

	double start, stop;
	if (mode.compare("bool") == 0)
	{

		// build index of document list
		start = second();
		dict.makeIndexFromList(docs);
		stop = second();
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;

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
			//cout << (float) start << endl;
			//cout << (float) stop << endl;
			printRes(result, (stop - start) * 1000);
		}
	}
	else if(mode.compare("positional") == 0){
	    
		// build index of document list
		start = second();
		dict.makeIndexFromList(docs);
		stop = second();
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;
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
			//cout << (float) start << endl;
			//cout << (float) stop << endl;
			printRes(result, (stop - start) * 1000);
		}
	}
	else if(mode.compare("fuzzy") == 0){
		// build index of document list 
		start = second();
		dict.makeFuzzyIndexFromList(docs);
		stop = second();
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;

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
			//cout << (float) start << endl;
			//cout << (float) stop << endl;
			printRes(result, (stop - start) * 1000);
		}
	} 
	else if(mode.compare("tolerant") == 0){
		// build index of document list 

		string kstr;
		cout << "choose k: ";
		getline(cin, kstr);
		start = second();
		dict.makeTolerantIndexFromList(docs, atoi(kstr.c_str()));
		stop = second();
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;

		QueryParser qp(&dict);
		string query;
		while (1)
		{
			string rstr;
			cout << "choose r: ";
			getline(cin, rstr);
			if (rstr == "q")
				break;
			int r = atoi(rstr.c_str());

			cout << "\n\nenter your query: q=exit" << endl;
			getline(cin, query);
			if (query == "q")
				break;
			list<Posting> result;
			start = second();
			result = qp.tolerantParseAndExecute(query, r);
			stop = second();
			//cout << (float) start << endl;
			//cout << (float) stop << endl;
			printRes(result, (stop - start) * 1000);
		}
	}
	else if(mode.compare("vector") == 0)
	{
		start = second();
		dict.makeVectorIndexFromList(docs);
		stop = second();
		
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;

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
			result = qp.vectorParseAndExecute(query);
			stop = second();
			printRes(result, (stop - start) * 1000);
		}

	}	
	else if(mode.compare("topk") == 0)
	{
		start = second();
		dict.makeTopkIndexFromList(docs);
		stop = second();
		
		cout << "generating dict was done in " << (stop - start) << " seconds" << endl;

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
			result = qp.topkParseAndExecute(query);
			stop = second();
			printRes(result, (stop - start) * 1000);
		}

	}
	else
	{
		cout << "usage: P1 <mode>" << endl << "\t <mode>= bool or positional" << endl;
		exit(0);
	}
	return 0;
}

