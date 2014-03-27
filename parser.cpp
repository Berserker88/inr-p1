#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

string trim(const std::string &str,  //entfernt whitespaces vom Anfang und ende eines tokens
            const std::string &whitespace = " \t\n")
{
    const size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const size_t strEnd = str.find_last_not_of(whitespace);
    const size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
void andtoken(string query)
{
    size_t prev = 0;
    size_t next = 0;
    string token;
    while ((next = query.find_first_of(',', prev)) != string::npos)//tokens bei , trennen
    {
        if (next - prev != 0)
        {
            token = query.substr(prev, next - prev);
            token=trim(token);
            cout << "\t\t" << token << endl;
        }
        prev = next + 1;
        if (prev < query.size()) //letzter term hat kein , will aber auch in die Liste
        {
            token = query.substr(prev);
            token.erase(token.find_last_not_of(" \n\r\t") + 1);
            token=trim(token);
            cout << "\t\t" << token << endl;
        }
    }

    //Jetzt haben wir ne theorethisch eine tokenliste und müssen uns noch um das NOT kümmern
    //Bei der Termsortierung nach größe muss NOT "drinbleiben", da zuviele/wenig treffer da sind
    //für jeden Term
    if (token.find_first_of(' ', 0) != string::npos)
    {
    	string temp;
    	temp=token.substr(4,token.length()-4);//NOT entfernen
    	transform(temp.begin(), temp.end(), temp.begin(), ::tolower);//Term in kleinbuchstaben
    	//Anfrage, ergebnisse fürs sortieren (gesamt-treffer)

    	cout<<"NOTAnfrage "<<temp<<endl;//Gesamtdocs-treffer
    }
    else{
    	//Anfrage
    }
    bool nottoken=false;

    //Terme aufsteigend sortieren
    //
    if (token.find_first_of(' ', 0) != string::npos)
    {
    	//tausche not token mit nächstem token, damit intersect immer gleich aufgerufen wird
    	nottoken=true;
    }
    //resul=postings(first(terms))
    //terms=rest(terms)
    //while blah blah
    if(nottoken){
    	//notand-intersect
    	nottoken=false;
    }
    else{
    	//and-intersect
    }
    //terms=rest(terms)
    //endwhile
    //return result

}

void ortoken(string query)
{
    size_t begin = 0; //start und endpositionen
    size_t end = 0;
    string token; //einzelner string aus ANDs
    while ((begin = query.find_first_of('(', end)) != string::npos) //Solange neue AND-Tokens gefunden werden
    {
        begin += 1; //Damit ( nicht mitgezählt wird
        end = query.find_first_of(')', begin); //Finde Ende des AND-Tokens
        if (end - begin != 0)
        {
            token = query.substr(begin, end - begin);
            andtoken(token); //bzw and intersekt mit string als termliste und postingliste zurück
            cout << "\tand-intersect: " << token << endl;
        }
        begin = end + 1;
    }
    cout << "or-intersect" << endl;
    //OR-Intersekt mit postinglisten der and-token
    //return result

}

int main( int argc, char **argv )
{

    string test = "(Hexe, Prinzessin)(König, NOT Hexe)";
    ortoken(test);


    return 0;
}
