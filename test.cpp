




#include <iostream>
#include "Document.h"
#include "tinydir.h"
#include "IndexDict.h"



using namespace std;















int main( int argc, char** argv )
{
	list<Document *> docs = Document::getDocsFromDir("CorpusUTF8");
	
	list<Document *>::iterator iter;
	for(iter = docs.begin(); iter != docs.end(); iter++)
		cout << (*iter)->toString() << endl;

	cout << endl << docs.front()->toString() << endl;


	IndexDict dict;
	
	dict.makeIndexFromDoc(docs.front());
	
	cout << "dict = " << dict.toString() << endl;

	return 0;
}






































