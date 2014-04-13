




#include <iostream>
#include <ctime>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"
#include "Posting.h"
#include "QueryParser.h"
#include <sys/time.h>



using namespace std;


void printRes(list<Posting> pl, float time) {
	cout.precision(3);
	cout << pl.size() << " results  -  took " << time << " s" << endl;
	list<Posting>::iterator p_iter;
	for(p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
		cout << p_iter->toString() << endl;
	cout << endl;
}



double second()
{
    struct timeval tm;
    double t ;
    gettimeofday(&tm,NULL);
    t = (double) (tm.tv_sec) + ((double) (tm.tv_usec))/1.0e6;
    return t;
}


int main( int argc, char** argv )
{

	int tnum = 1;
	srand(time(NULL));
	// create documents from directory path
	list<Document *> docs = Document::getDocsFromDir("CorpusUTF8");
	
	// print documents loaded
	cout << "num of docs = " << docs.size() << endl;
	list<Document *>::iterator iter;
	for(iter = docs.begin(); iter != docs.end(); iter++)
		cout << (*iter)->toString() << endl;
	//cout << endl << docs.front()->toString() << endl;

	// create index dictionary
	IndexDict dict;
	
	
	
	// build index of document list
	dict.makeIndexFromList(docs);
	//cout << "dict = " << dict.toString() << endl;
	
	//printRes(dict.get("hexe"), 0.0);
	/*
	QueryParser qp(&dict);
	string query;
	
	
	double start, stop;
	while(1)
	{
		cout << "\n\nenter your query: AND=(,) OR=()() q=exit" << endl;
		getline(cin, query);
		if(query == "q")
			break;
		
		list<Posting> result;
		start = second();
		result = qp.parseAndExecute(query);
		stop = second();
		//cout << start << endl;
		//cout << stop << endl;
		printRes(result, (stop-start));
	}
	
	

	
	
	list<string> l1, l2, l3;
	
	
	l1.push_back("hexe");
	l1.push_back("könig");
	//l2.push_back("NOT aladdin");
	//l2.push_back("NOT prinzessin");
	//l3.push_back("hänsel");
	//l3.push_back("gretel");
	//l3.push_back("wald");
	
	
	list<Posting> res1 = dict.intersect(l1);
	list<Posting> res2 = dict.intersect(l2);
	//list<Posting> res3 = dict.intersect(l3);
	
	list<Posting> res12 = dict.unionLists(res1, res2);
	//list<Posting> res123 = dict.unionLists(res12, res3);
	
	
	cout << "query: hexe AND könig" << endl;
	printRes(res1, 0.0);
	*/
	
	list<Posting> res = dict.positionalIntersect("es", "einmal", 2); 

	printRes(res, 1234.56);
	
	return 0;
}






































