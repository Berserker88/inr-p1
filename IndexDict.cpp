



#include "IndexDict.h"
#include "tokens.hpp"

#include <iostream>
#include <sstream>
#include <queue>


static string NOT_PREFIX = "NOT ";


using namespace std;



void IndexDict::makeIndexFromList(list<Document *> doclist) {
	
	list<Document *>::iterator iter;
	for(iter = doclist.begin(); iter != doclist.end(); iter++)
		makeIndexFromDoc(*iter);
}



void IndexDict::makeIndexFromDoc(Document *doc) {
	
	
	_docs.push_back(doc);
	list<string> tokens = tokenize(doc->getFilename());

	
	tokens.sort();
	
	
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
	
	// create priority queue to sort by increasing freaquency
	priority_queue<Index, vector<Index>, CompareByFreq> terms(true);
	

	// get Index of strings
	list<string>::iterator strIter;
	for(strIter = tlist.begin(); strIter != tlist.end(); strIter++)
	{
		terms.push(getIndex(*strIter));
	}
	
	
	
	// check if first in list is a NOT and fill result with first postinglist
	if(isNot(terms.top().getToken()))
	{
		string token = terms.top().getToken();
		token = token.substr(NOT_PREFIX.length(), string::npos);
		result = notList(get(token));
		
	}
	else
		result = get(terms.top().getToken());
	terms.pop();
	
	
	// call the appropriate algorithm for result and next term in list
	while(!terms.empty() && !result.empty())
	{
		if(isNot(terms.top().getToken()))
		{
			string token = terms.top().getToken();
			token = token.substr(NOT_PREFIX.length(), string::npos);
			result = mergeAndNot(result, get(token));
		}
		else
			result = intersect(result, get(terms.top().getToken()));
		terms.pop();
	}
	
	
	return result;
}




bool IndexDict::isNot(string token, bool removeIfFound) {
	size_t pos = token.find(NOT_PREFIX);
	if(pos == 0)
		return true;
	return false;
}






list<Posting> IndexDict::mergeAndNot(list<Posting> pl1, list<Posting> pl2) {
	list<Posting> answer;
	
	// create pointer to first listelements
	list<Posting>::iterator p1 = pl1.begin(), p2 = pl2.begin();
	int docid1, docid2;
	
	// do until one is at the end of the list
	while(p1 != pl1.end() && p2 != pl2.end())
	{
		// get doc ids
		docid1 = p1->getDoc()->getId();
		docid2 = p2->getDoc()->getId();
		
		// check what to do
		if(docid1 == docid2)
		{
			p1++;
			p2++;
		}
		else if(docid1 < docid2)
		{
			answer.push_back(*p1);
			p1++;
		}
		else
		{
			p2++;
		}
	}
	
	// concatenate rest of first list and answer
	while(p1 != pl1.end())
	{
		answer.push_back(*p1);
		p1++;
	}
	
	return answer;
}





list<Posting> IndexDict::intersect(list<Posting> pl1, list<Posting> pl2) {
	list<Posting> answer;
	
	// create pointer to first listelements
	list<Posting>::iterator p1 = pl1.begin(), p2 = pl2.begin();
	int docid1, docid2;
	
	// do until one is at the end of the list
	while(p1 != pl1.end() && p2 != pl2.end())
	{
		// get doc ids
		docid1 = p1->getDoc()->getId();
		docid2 = p2->getDoc()->getId();
		
		// check what to do
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




list<Posting> IndexDict::notList(list<Posting> pl) {
	
	list<Posting> answer;
	
	// create pointer to first listelements
	list<Document *>::iterator p2 = _docs.begin();
	list<Posting>::iterator p1 = pl.begin();
	int docid1, docid2;
	
	// do until one is at the end of the list
	while(p1 != pl.end() && p2 != _docs.end())
	{
		// get doc ids
		docid1 = p1->getDoc()->getId();
		docid2 = (*p2)->getId();
		
		
		// check what to do
		if(docid1 == docid2)
		{
			p1++;
			p2++;
		}
		else if(docid2 < docid1)
		{
			answer.push_back(Posting(*p2));
			p2++;
		}
	}
	
	// concatenate rest of second list and answer
	while(p2 != _docs.end())
	{
		answer.push_back(Posting(*p2));
		p2++;
	}
	
	return answer;
}




list<Posting> IndexDict::unionLists(list<Posting> pl1, list<Posting> pl2) {
	list<Posting> answer;
	
	// create pointer to first listelements
	list<Posting>::iterator p1 = pl1.begin(), p2 = pl2.begin();
	int docid1, docid2;
	

	// do until one is at the end of the list
	while(p1 != pl1.end() && p2 != pl2.end())
	{
		// get doc ids
		docid1 = p1->getDoc()->getId();
		docid2 = p2->getDoc()->getId();
		
		
		// check what to do
		if(docid1 == docid2)
		{
			answer.push_back(*p1);
			p1++;
			p2++;
		}
		else if(docid1 < docid2)
		{
			answer.push_back(*p1);
			p1++;
		}
		else
		{
			answer.push_back(*p2);
			p2++;
		}
	}
	
	// concatenate rest of first list and answer
	while(p1 != pl1.end())
	{
		answer.push_back(*p1);
		p1++;
	}
	
	
	// concatenate rest of second list and answer
	while(p2 != pl2.end())
	{
		answer.push_back(*p2);
		p2++;
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













