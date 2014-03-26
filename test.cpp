




#include <iostream>
#include <ctime>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"
#include "Posting.h"



using namespace std;






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
	
	
	list<string> l;
	//l.push_back("aladdin");
	//l.push_back("prinzessin");
	l.push_back("hexe");
	//l.push_back("könig");
	

	list<Posting> res = dict.intersect(l);
	
	string query;
	list<string>::iterator s_iter;
	for(s_iter = l.begin(); s_iter != l.end(); s_iter++)
	{
		query += *s_iter + ",";
	}


	cout << "query result for '" << query << "':" << endl;
	cout << "num of docs = " << res.size() << endl;
	list<Posting>::iterator p_iter;
	for(p_iter = res.begin(); p_iter != res.end(); p_iter++)
		cout << p_iter->toString() << endl;
	


	
	return 0;
}






































