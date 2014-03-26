#include <iostream>
#include "RequestHandler.h"
using namespace std;

RequestHandler::RequestHandler(){
	_input = "";
}

RequestHandler::RequestHandler(string input){
	_input = input;
}

list<string> RequestHandler::parseToList(){

	int pos;
	list<string> l;
	pos = _input.find("OR");
		if (pos != string::npos)
			

	return l;

}
int RequestHandler::getLength(){
	return _input.length;
}
string RequestHandler::toString(){
	return _input;
}

