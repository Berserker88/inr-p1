#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

class RequestHandler{
	char input;
	RequestHandler();
	RequestHandler(char input);

	void printInputString(char input);
	char parseToArray(char input);

};
#endif // !REQUESTHANDLER_H
