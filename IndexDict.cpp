



#include "IndexDict.h"
#include "tokens.hpp"

#include <iostream>
#include <sstream>
#include <queue>
#include <fstream>

#define Sj 0.05



static string NOT_PREFIX = "NOT ";
static string DEBUG = "hexe";

using namespace std;



void IndexDict::makeIndexFromList(list<Document *> doclist) {
	
	list<Document *>::iterator iter;
	for(iter = doclist.begin(); iter != doclist.end(); iter++)
		makeIndexFromDoc(*iter);
}



void IndexDict::makeIndexFromDoc(Document *doc) {
	
	
	_docs.push_back(doc);
	list<string> tokens = tokenize(doc->getFilename());
	
	// with positions
	map<string, list<int> > positions;
	map<string, list<int> >::iterator find_iter;
	
	// initializing the positionlists
	list<string>::iterator iter;
	int pos = 1;
	for(iter = tokens.begin(); iter != tokens.end(); iter++)
	{
		find_iter = positions.find(*iter);
			
		if(find_iter == positions.end())
		{
			list<int> firstpos;
			firstpos.push_back(pos);
			positions.insert(pair<string, list<int> >(*iter, firstpos));
		}
		else
		{
			find_iter->second.push_back(pos);
		}
		pos++;
	}
	
	// adding Postings to dict
	for(find_iter = positions.begin(); find_iter != positions.end(); find_iter++)
	{
		doc->addContent(find_iter->first);
		string token = find_iter->first;
		list<int> tok_pos = find_iter->second;
		Posting p(doc, tok_pos);
		addToDict(token, p);
	}
	
	
	
	/*
	// without positions
	tokens.sort();

	list<string>::iterator outer_iter, inner_iter;
	string token;
	int cnt;
	outer_iter = tokens.begin();
	while(outer_iter != tokens.end())
	{
		cnt = 1;
		token = *outer_iter;
		list<int> positions;

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
	*/
}



list<Index> IndexDict::getIndexList() {
	list<Index> res;
	for(map<Index, list<Posting> >::iterator iter = _dict.begin(); iter != _dict.end(); iter++)
		res.push_back(iter->first);
	return res;
}



void IndexDict::makeFuzzyIndexFromList(list<Document *> doclist) {
	makeIndexFromList(doclist);
	


	string filename = "jaccard";
	bool file_exists = false;
	fstream file(filename.c_str(), fstream::in);
	if(file.is_open())
	{
		file_exists = true;
		file.close();
	}
	
	if(file_exists)
	{
		file.open(filename.c_str(), fstream::in);
	}
	else
	{
		file.open(filename.c_str(), fstream::out);
	}

	
	list<Index> indexterms = getIndexList();
	cout << "number of indices = " << indexterms.size() << endl;
	list<Index>::iterator iterT, iterU;
	map< pair<Index, Index> , double > jaccard;
	int cnt = 1;

	//for(iterT = indexterms.begin(); iterT != indexterms.end() && cnt < 500; iterT++)
	for(iterT = indexterms.begin(); iterT != indexterms.end(); iterT++)
	{	
		//cout  << "token number " << cnt << " (" << iterT->getToken()  <<  ") start ...\n";
		iterU = iterT;
		iterU++;
		for(iterU; iterU != indexterms.end(); iterU++)
		{
						
			double c;
			string tok1 = iterT->getToken();
			string tok2 = iterU->getToken();
			
			if(file_exists)
			{
				file >> c;

				//cout << "c = " << c << endl;
			}
			else
			{
				list<Posting> pl1, pl2, andlist, orlist;
				pl1 = get(tok1);
				pl2 = get(tok2);

				andlist = intersect(pl1, pl2);
				orlist = unionLists(pl1, pl2);
				c = (double) andlist.size() / (double) orlist.size();
				file << c << endl;
			}
			

			if(c > Sj)
			{
				jaccard.insert(pair< pair<Index, Index>, double >(pair<Index, Index>(*iterT, *iterU), c));
			}
		}
		cout  << "token number " << cnt << " (" << iterT->getToken()  <<  ") end ...\n";
		cnt++;
	}

	file.close();
	/*
	list<Document *>::iterator iterD;
	for(iterT = indexterms.begin(); iterT != indexterms.end(); iterT++)
	{
		for(iterD = _docs.begin(); iterD != _docs.end(); iterD++)
		{
			// TODO: query content from document and compute ogawa W(D, t)

		}
	}
	*/
}


void IndexDict::makeFuzzyIndexFromDoc(Document *doc) {
	makeIndexFromDoc(doc);
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








list<Posting> IndexDict::phraseQuery(string token1, string token2, string token3) {
	
	cout << token1 << " " << token2 << " " << token3 << endl;		
	// get the postinglists of the three tokens
	list<Posting> pl1 = get(token1);
	list<Posting> pl2 = get(token2);
	list<Posting> pl3 = get(token3);

	list<Posting> res12 = positionalIntersect(pl1, pl2, 1);
	list<Posting> res23 = positionalIntersect(pl2, pl3, 1);
	list<Posting> res13 = positionalIntersect(pl1, pl3, 2);
	
	list<Posting> res = intersect(res12, res23);
	list<Posting> answer = intersect(res, res13);
	
	if(token1 == "")
		return pl1;
	else if(token2 == "")
		return pl1;
	else if(token3 == "")
		return res12;
	else
		return answer;
}



list<Posting> IndexDict::proximityQuery(string token1, string token2, int proximity) {
	
	// get the postinglists of the two tokens
	list<Posting> pl1 = get(token1);
	list<Posting> pl2 = get(token2);

	list<Posting> answer = positionalIntersect(pl1, pl2, proximity);
	
	return answer;
}






list<Posting> IndexDict::positionalIntersect(list<Posting> pl1, list<Posting> pl2, int k) {
	
	
	list<Posting>::iterator p1 = pl1.begin();
	list<Posting>::iterator p2 = pl2.begin();
	
	list<Posting> answer;

	int docid1, docid2;
	
	while(p1 != pl1.end() && p2 != pl2.end())
	{
		docid1 = (*p1).getDoc()->getId();
		docid2 = (*p2).getDoc()->getId();
		
		if(docid1 == docid2)
		{
			list<int> pos1 = (*p1).getPositions();
			list<int> pos2 = (*p2).getPositions();
			list<int> l;
			
			list<int>::iterator pp1 = pos1.begin();
			list<int>::iterator pp2 = pos2.begin();
			list<int>::iterator ps;
			
			while(pp1 != pos1.end())
			{
				while(pp2 != pos2.end())
				{
					if(abs(*pp1 - *pp2) <= k)
						l.push_back(*pp2);
					else if(*pp2 > *pp1)
						break;
					pp2++;
				}
				while(!l.empty() && abs(l.front() - *pp1) > k)
					l.pop_front();
				
				for(ps = l.begin(); ps != l.end(); ps++)
				{
					list<int> positions;
					positions.push_back(*pp1);
					positions.push_back(*ps);
					answer.push_back(Posting(p1->getDoc(), positions));
				}

				pp1++;					
			}

			p1++;
			p2++;	
		}
		else if(docid1 < docid2)
			p1++;
		else
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













