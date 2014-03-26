




#include <iostream>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"
#include "Posting.h"



using namespace std;






int main( int argc, char** argv )
{

	int tnum = 1;
	
	list<Document *> docs = Document::getDocsFromDir("CorpusUTF8");
	
	list<Document *>::iterator iter;
	for(iter = docs.begin(); iter != docs.end(); iter++)
		cout << (*iter)->toString() << endl;


	cout << endl << docs.front()->toString() << endl;


	IndexDict dict;
	
	/*
	dict.makeIndexFromDoc(docs.front());
	
	cout << "dict = " << dict.toString() << endl;
	
	
	cout << "Test " << tnum++ << ": " << endl;
	string token = "laber";
	list<Posting> plist = dict.get(token);
	if(!plist.empty())
	{
		cout << "Success: " << token << " found!" << endl;
		list<Posting>::iterator iter;
		for(iter = plist.begin(); iter != plist.end(); iter++)
			cout << iter->toString() << endl;
	}
	else
	{
		cout << "FAIL: " << token << " not found!" << endl;
	}
	
	
	cout << "Test " << tnum++ << ": " << endl;
	token = "labers";
	plist = dict.get(token);
	if(plist.empty())
	{
		cout << "Success: " << token << " not found!" << endl;
	}
	else
	{
		cout << "FAIL: " << token << " found!" << endl;
		list<Posting>::iterator iter;
		for(iter = plist.begin(); iter != plist.end(); iter++)
			cout << iter->toString() << endl;
	}
	*/
	
	dict.makeIndexFromList(docs);
	cout << "dict = " << dict.toString() << endl;
	
	string token = "oder";
	list<Posting> plist = dict.get(token);
	if(!plist.empty())
	{
		cout << "Success: " << token << " found!" << endl;
		list<Posting>::iterator iter;
		for(iter = plist.begin(); iter != plist.end(); iter++)
			cout << iter->toString() << endl;
	}
	else
	{
		cout << "FAIL: " << token << " not found!" << endl;
	}
	
	cout << "num of docs = " << docs.size() << endl;
	
	return 0;
}






































