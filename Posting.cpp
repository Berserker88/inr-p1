


#include "Posting.h"
#include <sstream>

using namespace std;





Posting::Posting(Document *doc, int freq, list<int> pos) {
	_doc = doc;
	_freq = freq;
	_positions = pos;
}



list<int> Posting::getPositions() {
	return _positions;
}



void Posting::addPosition(int pos) {
	_positions.push_back(pos);
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
















