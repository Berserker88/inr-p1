#ifndef __INDEXDICT_H__
#define __INDEXDICT_H__

#include "Document.h"
#include "Index.h"
#include "Posting.h"
#include <map>
#include <list>

class IndexDict {
	
	private:
		map<Index, list<Posting> > _dict;
		
		void addToDict(string token, Posting post);
	
	
	public:
		void makeIndexFromList(list<Document *> doclist);
		void makeIndexFromDoc(Document *doc);
		
		void clear();
		string toString() const;
		
		list<Posting> get(string token); 
		int getTotFreq(string token);
};











#endif
