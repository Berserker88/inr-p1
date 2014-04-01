

#include "QueryParser.h"
#include <iostream>

using namespace std;


string QueryParser::trim(string token) {
	string whitespace = " \t\n";
    size_t strBegin = token.find_first_not_of(whitespace);
    
    if (strBegin == string::npos)
        return "";

    size_t strEnd = token.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;

    return token.substr(strBegin, strRange);	
}



void QueryParser::printResult(list<Posting> pl) {
	cout << "Result:" << endl << "num of docs = " << pl.size() << endl;
	list<Posting>::iterator p_iter;
	for(p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
		cout << p_iter->toString() << endl;
	cout << endl;
}


list<Posting> QueryParser::andQuery(string query) {
	size_t prev = 0;
    size_t next = 0;
    string token;
    
    list<string> tokens;
    list<Posting> result;
    
    //tokens bei , trennen
    while ((next = query.find_first_of(',', prev)) != string::npos)
    {
        if (next - prev != 0)
        {
            token = query.substr(prev, next - prev);
            token=trim(token);
            tokens.push_back(token);
            cout << "\t\t" << token << endl;
        }
        prev = next + 1;
        
       
    }

	 //letzter term hat kein , will aber auch in die Liste
        if (prev < query.size())
        {
            token = query.substr(prev);
            token.erase(token.find_last_not_of(" \n\r\t") + 1);
            token=trim(token);
            tokens.push_back(token);
            cout << "\t\t" << token << endl;
        }
        /*
	if(query != "" && tokens.size() == 0)
	{
		token = query;
		token = trim(token);
		tokens.push_back(token);
		cout << "\t\t" << token << endl;
	}*/
	
	result = _dict->intersect(tokens);
	
	
	return result;

   
}
		


QueryParser::QueryParser(IndexDict *dict) {
	_dict = dict;
}



list<Posting> QueryParser::parseAndExecute(string query) {
	size_t begin = 0;
	size_t end = 0;
	
	list< list<Posting> > res;	
	list<Posting> result;
	
	//einzelner string aus ANDs
	string token;
	
	cout << "OR" << endl;
	//Solange neue AND-Tokens gefunden werden
	while ((begin = query.find_first_of('(', end)) != string::npos)
	{
		//Damit ( nicht mitgezählt wird
		begin += 1;
		
		//Finde Ende des AND-Tokens
		end = query.find_first_of(')', begin);
		if (end - begin != 0)
		{
			token = query.substr(begin, end - begin);
			cout << "\tAND " << endl;
			res.push_back(andQuery(token));
		}
		begin = end + 1;
	}
	
	if(res.empty())
		return result;
	
	//OR mit postinglisten der and-token
	list< list<Posting> >::iterator iter = res.begin();
	result = *(iter);
	iter++;
	
	for(iter; iter != res.end(); iter++)
	{
		result = _dict->unionLists(result, *(iter));
	}
	
	
	
	return result;
}
















