


#include "Posting.h"
#include <sstream>

using namespace std;





Posting::Posting(Document *doc, int freq) {
	_doc = doc;
	_freq = freq;
}



Document *Posting::getDoc() {
	return _doc;
}


int Posting::getFreq() {
	return _freq;
}



string Posting::toString() const {
	ostringstream os;
	
	os << _doc->toString() << "; " << _freq;
	
	return os.str();
}









