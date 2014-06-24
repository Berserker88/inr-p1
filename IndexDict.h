#ifndef __INDEXDICT_H__
#define __INDEXDICT_H__

#include "Document.h"
#include "Index.h"
#include "Posting.h"
#include <map>
#include <vector>
#include <list>

class IndexDict {

	private:
		map<Index, list<Posting> > _dict;
		map<Document *, vector<int> > _docVec;
		map<Document *, vector<Document *> > _cluster;
		vector<Document *> _leader;
		int _topk;
		map<string, list<string> > _kgrams;
		list<Document *> _docs;
		

		void addToDict(string token, Posting post);
		
		double _thresh_j, _thresh_o;
		int _k;
		bool _ctwf;
	
	public:
		IndexDict(bool combineTolWithFuz = false);

		void setThresholdJaccard(double thresh);
		void setThresholdOgawa(double thresh);

		void makeIndexFromList(list<Document *> doclist);
		void makeIndexFromDoc(Document *doc);
		
		void makeFuzzyIndexFromList(list<Document *> doclist);
		void makeFuzzyIndexFromDoc(Document *doc);
		
		void makeTolerantIndexFromList(list<Document *> doclist, int k);
		void makeTolerantIndexFromDoc(Document *);

		void makeVectorIndexFromList(list<Document *> doclist);
		void makeVectorIndexFromDoc(Document *);
	
		void makeTopkIndexFromList(list<Document *> doclist);
		void makeTopkIndexFromDoc(Document *);
		
		
		int getNumber(string token);
		string getToken(int number);
		
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


		list<Posting> intersectFuzzy(list<string> terms);
		list<Posting> intersectFuzzy(list<Posting> pl1, list<Posting> pl2);
		list<Posting> unionListsFuzzy(list<Posting> pl1, list<Posting> pl2);
		list<Posting> notListFuzzy(list<Posting> pl);
		list<Posting> mergeAndNotFuzzy(list<Posting> pl1, list<Posting> pl2);
	


		list<Posting> intersectTolerant(list<string> terms, int r);
		void doCorrection(string token);
		string getCorrToken(string token);
		int dist(string tok1, string tok2);
		int min3(int a, int b, int c);
		
		
		list<Posting> intersectVector(list<string> terms);
		list<Posting> intersectTopk(list<string> terms);
		list<Posting> cosineScore(vector<int> qv, vector<Document *> docs);
		list<Posting> fastCosineScore(vector<int> q);
};











#endif
