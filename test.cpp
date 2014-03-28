




#include <iostream>
#include <ctime>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"
#include "Posting.h"



using namespace std;


void printRes(list<Posting> pl) {
	cout << "Result:" << endl << "num of docs = " << pl.size() << endl;
	list<Posting>::iterator p_iter;
	for(p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
		cout << p_iter->toString() << endl;
	cout << endl;
}



int main( int argc, char** argv )
{

	int tnum = 1;
	
	// create documents from directory path
	list<Document *> docs = Document::getDocsFromDir("CorpusUTF8");
	
	// print documents loaded
	cout << "num of docs = " << docs.size() << endl;
	list<Document *>::iterator iter;
	for(iter = docs.begin(); iter != docs.end(); iter++)
		cout << (*iter)->toString() << endl;
	cout << endl << docs.front()->toString() << endl;

	// create index dictionary
	IndexDict dict;
	
	
	// build index of document list
	dict.makeIndexFromList(docs);
	//cout << "dict = " << dict.toString() << endl;
	
	
	/*
	// query ONE string
	string token = "oder";
	list<Posting> plist = dict.get(token);
	if(!plist.empty())
	{
		// print result
		cout << "Success: " << token << " found!" << endl;
		list<Posting>::iterator iter;
		for(iter = plist.begin(); iter != plist.end(); iter++)
			cout << iter->toString() << endl;
		cout << "num of matches = " << plist.size() << endl;
		cout << "total frequency of '" << token << "' = " << dict.getTotFreq(token) << endl;
	}
	else
	{
		cout << "FAIL: " << token << " not found!" << endl;
	}
	*/
	
	
	list<string> l1, l2, l3;
	l1.push_back("hexe");
	l1.push_back("NOT könig");
	l2.push_back("NOT aladdin");
	l2.push_back("NOT prinzessin");
	//l3.push_back("hänsel");
	//l3.push_back("gretel");
	l3.push_back("wald");
	

	list<Posting> res1 = dict.intersect(l1);
	list<Posting> res2 = dict.intersect(l2);
	list<Posting> res3 = dict.intersect(l3);
	
	list<Posting> res12 = dict.unionLists(res1, res2);
	list<Posting> res123 = dict.unionLists(res12, res3);
	
	/*
	string query;
	list<string>::iterator s_iter;
	for(s_iter = l.begin(); s_iter != l.end(); s_iter++)
	{
		query += *s_iter + ",";
	}
	cout << "query result for '" << query << "':" << endl;
	*/
	
	printRes(res1);
	printRes(res2);
	printRes(res3);
	printRes(res12);
	printRes(res123);
	
	


	
	return 0;
}






































