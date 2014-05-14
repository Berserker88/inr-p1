


#include "Posting.h"
#include <sstream>

using namespace std;





Posting::Posting(Document *doc, int freq) {
	_doc = doc;
	_freq = freq;
	_degree = 1;
}



Posting::Posting(Document *doc, list<int> poss) {
	_doc = doc;
	_positions = poss;
	_freq = _positions.size();
	_degree = 1;
}


void Posting::setDegree(double deg) {
	_degree = deg;
}

double Posting::getDegree() {
	return _degree;
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
	
	os << _doc->toString() << "; " << _freq << "; <";
	list<int>::const_iterator iter;
	for(iter = _positions.begin(); iter != _positions.end(); iter++)
	{
		os << (*iter) << "; ";
	}
	os << ">; deg = " << _degree;
	
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
















