



#include "IndexDict.h"
#include "tokens.hpp"

#include <iostream>
#include <sstream>
#include <queue>
#include <fstream>
#include <climits>



static string NOT_PREFIX = "NOT ";
static string DEBUG = "hexe";

using namespace std;






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



// ==================================  Constructor  ==================================================

IndexDict::IndexDict(bool combineTolWithFuz) {
	
	// just relevant for fuzzy
	_thresh_j = 0.05;
	_thresh_o = 0.05;
	
	// just relevant for tolerant
	_k = 0;
	_ctwf = combineTolWithFuz;
}




void IndexDict::setThresholdOgawa(double thresh) {
	_thresh_j = thresh;
}


void IndexDict::setThresholdJaccard(double thresh) {
	_thresh_o = thresh;
}




// ============================  tolerant  ===========================================================


void IndexDict::makeTolerantIndexFromDoc(Document *doc) {
	makeIndexFromDoc(doc);
}


void IndexDict::makeTolerantIndexFromList(list<Document *> doclist, int k) {
	
	
	if(_ctwf)
		makeFuzzyIndexFromList(doclist);
	else
		makeIndexFromList(doclist);

	_k = k;


	cout << "computing kgram index ...\n";
	map< Index, list<Posting> >::iterator iterT = _dict.begin();
	for(iterT; iterT != _dict.end(); iterT++)
	{
		Index t = iterT->first;
		string token = t.getToken();
		int len = token.length();

		if(len < k)
		{
			
			_kgrams[token].push_back(token);
		}
		else
		{
			string kgram = token.substr(0, k-1);
			_kgrams[kgram].push_back(token);
			for(int i = 0; i < (len - k + 1); i++)
			{
				kgram = token.substr(i, k);
				_kgrams[kgram].push_back(token);
			}
			kgram = token.substr(len - k + 1);
			_kgrams[kgram].push_back(token);
		}
	}
	cout << "done!\n";

	
	map<string, list<string> >::iterator iter = _kgrams.begin();
	for(int i = 0; i < 100; i++)
		iter++;

	for(int i = 0; i < 5; i++)
	{
		cout << iter->first << " ====>> ";
		list<string>::iterator str = iter->second.begin();
		for(str; str != iter->second.end(); str++)
		{
			cout << *str << " -> ";
		}
		cout << endl << endl;
		iter++;
	}
	

}




void IndexDict::doCorrection(string token) {
	cout << "rechtschreibkorrektur!!!" << endl;
	string corrToken = getCorrToken(token);
	if(!corrToken.empty())
		cout << "\nDid you mean '" << corrToken << "' instead of '" << token << "' ?\n";
}


list<Posting> IndexDict::intersectTolerant(list<string> tlist, int r) {
	
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
	string term = terms.top().getToken();
	string token, corrToken;
	if(isNot(term))
	{
		token = term.substr(NOT_PREFIX.length(), string::npos);
		if(_ctwf)
			result = notListFuzzy(get(token));
		else
			result = notList(get(token));
	}
	else
	{
		token = term;
		result = get(token);
	}
	terms.pop();

	// do a correction if needed
	if(result.size() < r)
	{
		doCorrection(token);
	}
	

	list<Posting> plToken;
	// call the appropriate algorithm for result and next term in list
	while(!terms.empty() && !result.empty())
	{
		if(isNot(terms.top().getToken()))
		{
			string token = terms.top().getToken();
			token = token.substr(NOT_PREFIX.length(), string::npos);
			plToken = get(token);
			
			if(_ctwf)
				result = mergeAndNotFuzzy(result, plToken);
			else
				result = mergeAndNot(result, plToken);
		}
		else
		{
			token = terms.top().getToken();
			plToken = get(token);
			
			if(_ctwf)
				result = intersectFuzzy(result, plToken);
			else
				result = intersect(result, plToken);
		}
		
		// do a correction if needed
		if(plToken.size() < r)
		{
			doCorrection(token);
		}

		terms.pop();
	}
	
	
	return result;
}








string IndexDict::getCorrToken(string token) {
	string corrToken;	

	vector<string> kgrams;

	int len = token.length();
	
	// compute kgrams of the token
	if(len < _k)
	{	
		kgrams.push_back(token);
	}
	else
	{
		
		string kgram;
		//kgram = token.substr(0, _k-1);
		//kgrams.push_back(kgram);
		for(int i = 0; i < (len - _k + 1); i++)
		{
			kgram = token.substr(i, _k);
			kgrams.push_back(kgram);
		}
		//kgram = token.substr(len - _k + 1);
		//kgrams.push_back(kgram);
	}

	/*
	cout << "kgrams(" << token << ") = {\n";
	for(int i = 0; i < kgrams.size(); i++)
		cout << kgrams[i] << "\n";
	cout << "}\n";
	*/

	// count the matching kgrams for each token
	map<string, int> tokenCount;
	for(int i = 0; i < kgrams.size(); i++)
	{	
		list<string> tokens;
		tokens = _kgrams[kgrams[i]];
		
		//cout << kgrams[i] << "  ===>  ";

		for(list<string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
		{
			//cout << *iter << ",  ";
			if(tokenCount.find(*iter) == tokenCount.end())
				tokenCount[*iter] = 1;
			else
				tokenCount[*iter]++;
		}
		//cout << endl;
	}
	

	// just choose the tokens, that have > 2/3 of matching kgrams
	//double ref = 2.0/3.0;
	double ref = 0.4;
	vector<string> possibilities;
	int minDist = INT_MAX;
	for(map<string, int>::iterator iter = tokenCount.begin(); iter != tokenCount.end(); iter++)
	{
		
		//double val = (double) iter->second / (double) kgrams.size();
		double val = (double) iter->second / (double) ((iter->first.length() - _k + 1) 
				+ kgrams.size() - iter->second);
		//cout << "new candidate: " << iter->first << "(val = " << val << ")\n";
		if(val >= ref)
		{
			
			// choose the token with the lowest dist
			cout << "new possibility: " << iter->first << "(val = " << val << " >= " << ref << ")\n";
			int levenDist = dist(token, iter->first);
			if(levenDist < minDist)
			{
				minDist = levenDist;
				corrToken = iter->first;
				cout << "new mininmum = " << corrToken << " (dist = " << minDist << ")\n";
			}
			else
			{
				cout << "dist of " << iter->first << "(" << levenDist << ") is too high\n";
			}
			cout << endl;
		}
	}


	cout << "corrected '" << token << "' to '" << corrToken << "'\n";
	

	return corrToken;
}





int IndexDict::min3(int a, int b, int c) {
    if (a < b) {
        if (a < c)
            return a;
        return c;
    }
    if (b < c)
        return b;
    return c;
}





int IndexDict::dist(string tok1, string tok2) {
	
	
	int i = tok1.size();
	int j = tok2.size();


	// compute levenshtein distance
	
	int cost;

	if (i == 0)
		return j;
	if (j == 0)
		return i;

	if (tok1[i-1] == tok2[j-1])
		cost = 0;
	else 
		cost = 1;

	return min3(dist(tok1.substr(0, i-1), tok2.substr(0, j-1)) + cost,  /* ersetzen o. gleich */
	    	dist(tok1.substr(0, i-1), tok2) + 1,           /* loeschen */
	    	dist(tok1, tok2.substr(0, j-1)) + 1);          /* einfuegen */	
}


















// =====================================  normal  ==================================================


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
		/*else
		{
			cout << "neeeeeeeetttt\n";
			cout << "docid1 = " << docid1 << endl;
			cout << "docid2 = " << docid2 << endl;
		}*/
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




















// ==================================  positional  ==================================================

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
















// =====================================  fuzzy  =====================================================


void IndexDict::makeFuzzyIndexFromList(list<Document *> doclist) {
	makeIndexFromList(doclist);
	

	// check if jaccard file already exists
	string filename = "jaccard";
	bool file_exists = false;
	fstream file(filename.c_str(), fstream::in);
	if(file.is_open())
	{
		file_exists = true;
		file.close();
	}
	
	// open jaccard to read/write according to i it exists
	if(file_exists)
	{
		file.open(filename.c_str(), fstream::in);
		cout << "reading jaccard ...\n";
	}
	else
	{
		file.open(filename.c_str(), fstream::out);
		cout << "computing jaccard ...\n";
	}

	
	// create jaccard map
	list<Index> indexterms = getIndexList();
	list<Index>::iterator iterT, iterU;
	map< pair<Index, Index> , double > jaccard;
	int cnt = 1;


	// for every indexterm t ...
	for(iterT = indexterms.begin(); iterT != indexterms.end(); iterT++)
	{	
		iterU = iterT;
		iterU++;
		// for every indexterm u ...
		for(iterU; iterU != indexterms.end(); iterU++)
		{
				
			double c;
			string tok1 = iterT->getToken();
			string tok2 = iterU->getToken();
			
			if(file_exists)
			{	
				// read jaccard(t, u) value from file
				file >> c;
			}
			else
			{
				// calculate jaccard(t, u) value ...
				list<Posting> pl1, pl2, andlist, orlist;
				pl1 = get(tok1);
				pl2 = get(tok2);

				andlist = intersect(pl1, pl2);
				orlist = unionLists(pl1, pl2);
				c = (double) andlist.size() / (double) orlist.size();
				
				// ... and write it to file
				file << c << endl;
			}
			
			// check jaccard value to met the requirements
			if(c > _thresh_j)
			{
				jaccard.insert(pair< pair<Index, Index>, double >(pair<Index, Index>(*iterT, *iterU), c));
			}
		}
		cnt++;
	}
	
	// close jaccard file
	file.close();
	cout << "done!\n";



	// check if ogawa file already exists
	filename = "ogawa";
	file_exists = false;
	file.open(filename.c_str(), fstream::in);
	if(file.is_open())
	{
		file_exists = true;
		file.close();
	}
	
	// open ogawa file in read/write mode according to its existance
	if(file_exists)
	{
		file.open(filename.c_str(), fstream::in);
		cout << "reading ogawa ...\n";
		
	}
	else
	{
		file.open(filename.c_str(), fstream::out);
		cout << "comuting ogawa ...\n";	
	}

	cnt = 1;
	
	// for every indexterm m ...
	for(map<Index, list<Posting> >::iterator iterM = _dict.begin(); iterM != _dict.end(); iterM++)
	{	
		int docnum = 1;
		Index t = iterM->first;

		// get all docs, that m not contains
		list<Posting> notContained = notList(iterM->second);
		
		// for every document d, that m not contains
		for(list<Posting>::iterator iterD = notContained.begin(); iterD != notContained.end(); iterD++)
		{
			double ogawa = 0.0;
			if(file_exists)
			{
				// read ogawa value from file
				file >> ogawa;
			}
			else
			{
				double prod = 1.0;
				list<string> content = iterD->getDoc()->getContent();
				int toknum = 1;
				
				// for every token u in document d
				for(list<string>::iterator iterU = content.begin(); iterU != content.end(); iterU++)
				{
					// check if jaccard(u, t) value exists
					Index u = getIndex(*iterU);
					map< pair<Index, Index>, double >::iterator iterJ = jaccard.find(pair<Index, Index>(t, u));
					if(iterJ == jaccard.end())
					{
						iterJ = jaccard.find(pair<Index, Index>(u, t));
					}

					if(iterJ != jaccard.end())
					{
						// compute ogawa
						prod = prod * (1.0 - iterJ->second);
					}
				}
				ogawa = 1.0 - prod;
				
				// write ogawa value to file
				file << ogawa << endl;
			}

			// check ogawa value to met the requirements
			if(ogawa > _thresh_o)
			{
				iterD->setDegree(ogawa);
				addToDict(t.getToken(), *iterD);
			}
		}
	}


	file.close();
	cout << "done!\n";

}


void IndexDict::makeFuzzyIndexFromDoc(Document *doc) {
	makeIndexFromDoc(doc);
}



list<Posting> IndexDict::intersectFuzzy(list<string> tlist) {
	
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
		result = notListFuzzy(get(token));
		
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
			result = mergeAndNotFuzzy(result, get(token));
		}
		else
			result = intersectFuzzy(result, get(terms.top().getToken()));
		terms.pop();
	}
	
	
	return result;
}



list<Posting> IndexDict::mergeAndNotFuzzy(list<Posting> pl1, list<Posting> pl2) {
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
			double zp1 = p1->getDegree();
			double zp2 = p2->getDegree();
			if(zp1 < (1 - zp2))
				answer.push_back(*p1);
			else
			{
				Posting post(*p2);
				post.setDegree(1 - zp2);
				answer.push_back(post);
			}
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





list<Posting> IndexDict::intersectFuzzy(list<Posting> pl1, list<Posting> pl2) {
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
		

		double zp1 = p1->getDegree();
		double zp2 = p2->getDegree();

		// check what to do
		if(docid1 == docid2)
		{
			if(zp1 < zp2)
				answer.push_back(*p1);
			else
				answer.push_back(*p2);
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




list<Posting> IndexDict::notListFuzzy(list<Posting> pl) {
	
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
			p1->setDegree(1 - p1->getDegree());
			answer.push_back(*p1);
			p1++;
			p2++;
		}
		else if(docid2 < docid1)
		{
			answer.push_back(Posting(*p2));
			p2++;
		}
		/*else
		{
			cout << "neeeeeeeetttt\n";
			cout << "docid1 = " << docid1 << endl;
			cout << "docid2 = " << docid2 << endl;
		}*/
	}
	
	// concatenate rest of second list and answer
	while(p2 != _docs.end())
	{
		answer.push_back(Posting(*p2));
		p2++;
	}
	
	return answer;
}




list<Posting> IndexDict::unionListsFuzzy(list<Posting> pl1, list<Posting> pl2) {
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
			if(p1->getDegree() > p2->getDegree())
				answer.push_back(*p1);
			else
				answer.push_back(*p2);
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











// ===================================  general functions  =============================================


void IndexDict::clear() {
	_dict.clear();
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
		iter->second.sort();
	
		/*
		pair<Index, list<Posting> > p(idx, iter->second);
		_dict.erase(iter);
		_dict.insert(p);
		*/
	}
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







/********************************************************************/






