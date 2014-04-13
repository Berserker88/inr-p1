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
		list<Document *> _docs;
		void addToDict(string token, Posting post);
	
	
	public:
		void makeIndexFromList(list<Document *> doclist);
		void makeIndexFromDoc(Document *doc);
		
		void clear();
		string toString() const;
		
		bool isNot(string token, bool removeIfFound = false);
		
		list<Posting> get(string token);
		
		list<Posting> getPositional(string token1, string token2, string token3);
		list<Posting> positionalIntersect(string token1, string token2, int proximity);
		
		int getTotFreq(string token);
		Index getIndex(string token);
		
		list<Posting> intersect(list<string> terms);
		list<Posting> intersect(list<Posting> pl1, list<Posting> pl2);
		list<Posting> unionLists(list<Posting> pl1, list<Posting> pl2);
		list<Posting> notList(list<Posting> pl);
		list<Posting> mergeAndNot(list<Posting> pl1, list<Posting> pl2);
};











#endif
