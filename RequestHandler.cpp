#include <string.h>
#include <iostream>
#include "RequestHandler.h"
using namespace std;
class RequestHandler{
public:
	char input;
	RequestHandler();
	RequestHandler(char input);
	
	void printInputString(char input);
	char parseToArray(char input);
};

