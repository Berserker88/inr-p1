#ifndef __QUERYPARSER_H__
#define __QUERYPARSER_H__


#include "IndexDict.h"
#include <list>
#include "Posting.h"



class QueryParser {

	private:
		IndexDict *_dict;
		
		list<Posting> andQuery(string query);
		string trim(string token);
		void printResult(list<Posting> pl);
		list<Posting> fuzzyAndQuery(string query);
		list<Posting> tolerantAndQuery(string query, int r);
		
	public:
		QueryParser(IndexDict *dict);
		list<Posting> parseAndExecute(string query);
		list<Posting> positionalParseAndExecute(string query);
		list<Posting> fuzzyParseAndExecute(string query);
		list<Posting> tolerantParseAndExecute(string query, int r);
};



















#endif
















