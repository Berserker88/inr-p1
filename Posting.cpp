


#include "Posting.h"
#include <sstream>

using namespace std;





Posting::Posting(Document *doc, int freq) {
	_doc = doc;
	_freq = freq;
}





Document *Posting::getDoc() const {
	return _doc;
}


int Posting::getFreq() const {
	return _freq;
}



string Posting::toString() const {
	ostringstream os;
	
	os << _doc->toString() << "; " << _freq;
	
	return os.str();
}



bool Posting::operator==(const Posting& p) const {
	return _doc == p.getDoc();
}


bool Posting::operator<(const Posting& p) const {
	return _doc < p.getDoc();
}


bool Posting::operator>(const Posting& p) const {
	return _doc > p.getDoc();
}
















