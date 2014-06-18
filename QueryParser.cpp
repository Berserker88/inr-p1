

#include "QueryParser.h"
#include <iostream>
#include <cstdlib>

using namespace std;




list<Posting> QueryParser::andQuery(string query)
{
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
            token = trim(token);
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
        token = trim(token);
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



QueryParser::QueryParser(IndexDict *dict)
{
    _dict = dict;
}




bool compareResults(const Posting& left, const Posting& right) {
	return left.getDegree() > right.getDegree();
}













// ===============================  vector  =============================================



list<Posting> QueryParser::vectorAndQuery(string query)
{
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
			token = trim(token);
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
		token = trim(token);
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

	result = _dict->intersectVector(tokens);
	return result;
}


list<Posting> QueryParser::vectorParseAndExecute(string query)
{
	size_t begin = 0;
	size_t end = 0;

	list< list<Posting> > res;
	list<Posting> result;

	//einzelner string aus ANDs
	string token;

	//cout << "OR" << endl;
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
		    res.push_back(vectorAndQuery(token));
		}
		begin = end + 1;
	}

	if (res.empty())
		return result;

	//OR mit postinglisten der and-token
	list< list<Posting> >::iterator iter = res.begin();
	result = *(iter);
	iter++;

	/*
	for (iter; iter != res.end(); iter++)
	{
		result = _dict->unionListsFuzzy(result, *(iter));
	}
	*/

	result.sort(compareResults);



    return result;
}








// ============================  normal  ======================================0


list<Posting> QueryParser::parseAndExecute(string query)
{
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

    if (res.empty())
        return result;

    //OR mit postinglisten der and-token
    list< list<Posting> >::iterator iter = res.begin();
    result = *(iter);
    iter++;

    for (iter; iter != res.end(); iter++)
    {
        result = _dict->unionLists(result, *(iter));
    }



    return result;
}






// =============================  positional  =============================================0


list<Posting> QueryParser::positionalParseAndExecute(string query)
{
    size_t begin = 0;
    size_t end = 0;

    list< list<Posting> > res;
    list<Posting> result;

    //einzelner Term
    string term1, term2, term3;
    //positional oder proximity
    if ((begin = query.find_first_of('/', end)) != string::npos)
    {
        int space;
        string term1, term2;
        begin++;
        //Finde space nach Zahl.
        end = query.find_first_of(' ', begin);
        //Zahl ausschneiden
        space = atoi(query.substr(begin, end - begin).c_str());
        term1 = query.substr(0, begin - 1);
        term1 = trim(term1);
        term2 = query.substr(end, string::npos);
        term2 = trim(term2);
        result = _dict->proximityQuery(term1, term2, space);
    }
    // Anfrage
    else
    {
        //da auf 3 Terme beschränkt finde die 2 Spaces
        string term1, term2, term3;
        begin = query.find_first_of(' ', 0);
        end = query.find_last_of(' ', string::npos);
        //Findet begin kein Leerzeichen muss das query aus nur einem Term bestehen
        if (begin == string::npos)
            term1 = trim(query);
        else
        {
        	//Anfang bis erstes Leerzeichen
            term1 = query.substr(0, begin);
            term1 = trim(term1);
            //Erstes Leerzeichen über Distanz zwischen erstem und zweitem leerzeichen
            term2 = query.substr(begin, end - begin);
            term2 = trim(term2);
            //Rest
            term3 = query.substr(end, string::npos);
            term3 = trim(term3);
        }
        //Wenn nur 1 Leerzeichen gefunden wird umkopieren der Terme, sodass leere Terme hinten stehen
        if(term2.compare("")==0){
        	term2=term3;
        	term3="";
        }
        result = _dict->phraseQuery(term1,term2,term3);
    }
    return result;
}
















// ================================  tolerant  ==================================



list<Posting> QueryParser::tolerantParseAndExecute(string query, int r) {


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
		    res.push_back(tolerantAndQuery(token, r));
		}
		begin = end + 1;
	}

	if (res.empty())
		return result;

	//OR mit postinglisten der and-token
	list< list<Posting> >::iterator iter = res.begin();
	result = *(iter);
	iter++;

	for (iter; iter != res.end(); iter++)
	{
		result = _dict->unionListsFuzzy(result, *(iter));
	}


	result.sort(compareResults);

	return result;
}







list<Posting> QueryParser::tolerantAndQuery(string query, int r)
{
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
            token = trim(token);
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
        token = trim(token);
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

    result = _dict->intersectTolerant(tokens, r);


    return result;


}
















// ==================================  fuzzy  =========================================


list<Posting> QueryParser::fuzzyAndQuery(string query)
{
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
            token = trim(token);
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
        token = trim(token);
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

    result = _dict->intersectFuzzy(tokens);


    return result;


}





list<Posting> QueryParser::fuzzyParseAndExecute(string query)
{
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
		    res.push_back(fuzzyAndQuery(token));
		}
		begin = end + 1;
	}

	if (res.empty())
		return result;

	//OR mit postinglisten der and-token
	list< list<Posting> >::iterator iter = res.begin();
	result = *(iter);
	iter++;

	for (iter; iter != res.end(); iter++)
	{
		result = _dict->unionListsFuzzy(result, *(iter));
	}


	result.sort(compareResults);



    return result;
}





// ================================  general methods  ====================================



string QueryParser::trim(string token)
{
    string whitespace = " \t\n";
    size_t strBegin = token.find_first_not_of(whitespace);

    if (strBegin == string::npos)
        return "";

    size_t strEnd = token.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;

    return token.substr(strBegin, strRange);
}



void QueryParser::printResult(list<Posting> pl)
{
    cout << "Result:" << endl << "num of docs = " << pl.size() << endl;
    list<Posting>::iterator p_iter;
    for (p_iter = pl.begin(); p_iter != pl.end(); p_iter++)
        cout << p_iter->toString() << endl;
    cout << endl;
}
