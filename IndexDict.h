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
		
		void makeFuzzyIndexFromList(list<Document *> doclist);
		void makeFuzzyIndexFromDoc(Document *doc);
		
		void clear();
		string toString() const;
		
		bool isNot(string token, bool removeIfFound = false);
		
		list<Posting> get(string token);
		

		list<Posting> phraseQuery(string token1, string token2, string token3);
		list<Posting> proximityQuery(string token1, string token2, int proximity);
		list<Posting> positionalIntersect(list<Posting> p1, list<Posting> p2, int proximity);
		
		int getTotFreq(string token);
		Index getIndex(string token);
		list<Index> getIndexList();
		
		list<Posting> intersect(list<string> terms);
		list<Posting> intersect(list<Posting> pl1, list<Posting> pl2);
		list<Posting> unionLists(list<Posting> pl1, list<Posting> pl2);
		list<Posting> notList(list<Posting> pl);
		list<Posting> mergeAndNot(list<Posting> pl1, list<Posting> pl2);
};











#endif
