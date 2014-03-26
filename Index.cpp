



#include "Index.h"
#include <sstream>


using namespace std;


Index::Index(string token, int freq) {
	_token = token;
	_ndocs = 1;
	_tfreq = freq;
}


void Index::addDoc(int freq) {
	_ndocs++;
	_tfreq += freq;
}


void Index::remDoc() {
	
}


void Index::addTotFreq(int freq) {
	_tfreq += freq;
}


void Index::addNumDocs(int ndocs) {
	_ndocs += ndocs;
}


string Index::getToken() const {
	return _token;
}


int Index::getNumDocs() const {
	return _ndocs;
}


int Index::getTotFreq() const {
	return _tfreq;
}


bool Index::operator==(const Index& index) const {
	return _token == index.getToken();
}

bool Index::operator<(const Index& index) const {
	return _token < index.getToken();
}

bool Index::operator>(const Index& index) const {
	return _token > index.getToken();
}

bool Index::operator() (const Index& left, const Index& right) const {

}

string Index::toString() const {
	ostringstream os;
	
	os << _token << "; " << _ndocs << "; " << _tfreq;
	
	return os.str();
}




