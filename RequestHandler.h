#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include <list>
#include <string>

using namespace std;

class RequestHandler{
private:
	string _input;
public:	
	RequestHandler();
	RequestHandler(string input);
	string toString();
	int getLength();
	list<string> parseToList();

};
#endif // !REQUESTHANDLER_H
