



#include "IndexDict.h"
#include "tokens.hpp"

#include <iostream>
#include <sstream>
#include <queue>


using namespace std;



void IndexDict::makeIndexFromList(list<Document *> doclist) {
	
	list<Document *>::iterator iter;
	for(iter = doclist.begin(); iter != doclist.end(); iter++)
		makeIndexFromDoc(*iter);
}



void IndexDict::makeIndexFromDoc(Document *doc) {
	
	list<string> tokens = tokenize(doc->getFilename());
	
	/*
	tokens.push_back("bla");
	tokens.push_back("laber");
	tokens.push_back("simon");
	tokens.push_back("hans");
	tokens.push_back("kuh");
	tokens.push_back("laber");
	tokens.push_back("bla");
	tokens.push_back("laber");
	tokens.push_back("gas");
	*/
	
	tokens.sort();
	
	/*
	list<string>::iterator iter = tokens.begin();
	for(iter; iter != tokens.end(); iter++)
		cout << *iter << endl;
	*/
	
	map<string, int> freq;
	
	
	list<string>::iterator outer_iter, inner_iter;
	string token;
	int cnt;
	outer_iter = tokens.begin();
	while(outer_iter != tokens.end())
	{
		cnt = 1;
		token = *outer_iter;

		inner_iter = outer_iter;
		inner_iter++;
		while(inner_iter != tokens.end() && *inner_iter == token)
		{
			cnt++;
			inner_iter++;
		}
		outer_iter = inner_iter;
		
		Posting p(doc, cnt);
		addToDict(token, p);
	}
}


void IndexDict::addToDict(string token, Posting post) {
	Index idx(token, post.getFreq());
	
	map<Index, list<Posting> >::iterator iter = _dict.find(idx);
	if(iter == _dict.end())
	{
		list<Posting> postings;
		postings.push_back(post);
		_dict.insert(pair<Index, list<Posting> >(idx, postings));
	}
	else
	{
		idx.addTotFreq(iter->first.getTotFreq());
		idx.addNumDocs(iter->first.getNumDocs());
		iter->second.push_back(post);
		
		pair<Index, list<Posting> > p(idx, iter->second);
		_dict.erase(iter);
		_dict.insert(p);
	}
}



void IndexDict::clear() {
	_dict.clear();
}



list<Posting> IndexDict::get(string token) {
	Index idx(token);
	
	map<Index, list<Posting> >::iterator iter = _dict.find(idx);
	
	
	if(iter == _dict.end())
	{
		list<Posting> plist;
		return plist;
	}
	
	return iter->second;	
}


int IndexDict::getTotFreq(string token) {
	Index idx(token);
	
	map<Index, list<Posting> >::iterator iter = _dict.find(idx);
	
	if(iter == _dict.end())
	{
		return 0;
	}
	
	return iter->first.getTotFreq();
}



Index IndexDict::getIndex(string token) {
	Index idx(token);
	
	map<Index, list<Posting> >::iterator iter = _dict.find(idx);
	
	if(iter == _dict.end())
	{
		return idx;
	}
	
	return iter->first;
}



class CompareByFreq {

	private:
		bool _reverse;
		
	public:
		CompareByFreq(bool rev = false) {
			_reverse = rev;
		}
		
		bool operator()(const Index& idx1, const Index& idx2) {
			
			if(_reverse)
				return idx1.getTotFreq() > idx2.getTotFreq();
			return idx1.getTotFreq() < idx2.getTotFreq();
		}

};


list<Posting> IndexDict::intersect(list<string> tlist) {
	
	list<Posting> result;
	
	if(tlist.empty())
		return result;
	
	// get indices of terms
	priority_queue<Index, vector<Index>, CompareByFreq> terms(true);
	
	// sort by increasing frequency
	list<string>::iterator strIter;
	for(strIter = tlist.begin(); strIter != tlist.end(); strIter++)
	{
		terms.push(getIndex(*strIter));
	}
	
	result = get(terms.top().getToken());
	terms.pop();
	
	
	while(!terms.empty() && !result.empty())
	{
		result = intersect(result, get(terms.top().getToken()));
		terms.pop();
	}
	
	
	return result;
}



list<Posting> IndexDict::intersect(list<Posting> pl1, list<Posting> pl2) {
	list<Posting> answer;
	
	list<Posting>::iterator p1 = pl1.begin(), p2 = pl2.begin();
	int docid1, docid2;
	
	
	while(p1 != pl1.end() && p2 != pl2.end())
	{
		docid1 = p1->getDoc()->getId();
		docid2 = p2->getDoc()->getId();
		
		if(docid1 == docid2)
		{
			answer.push_back(*p1);
			p1++;
			p2++;
		}
		else if(docid1 < docid2)
		{
			p1++;
		}
		else
		{
			p2++;
		}
	}
	
	return answer;
}



string IndexDict::toString() const {
	ostringstream os;
	
	map<Index, list<Posting> >::const_iterator iter;
	
	for(iter = _dict.begin(); iter != _dict.end(); iter++)
	{
		os << "{ " << iter->first.toString() << " } : { ";
		list<Posting>::const_iterator it;
		for(it = iter->second.begin(); it != iter->second.end(); it++)
			os << " { " << it->toString() << " } ;";
		os << " } " << endl;
	}
	
	return os.str();
}













