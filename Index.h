#ifndef __INDEX_H__
#define __INDEX_H__

#include <string>


using namespace std;

class Index {

	private:
		string _token;
		int _ndocs;
		int _tfreq;
		
	public:
		bool operator==(const Index& index) const;
		bool operator<(const Index& index) const;
		bool operator>(const Index& index) const;
	
		Index(string token, int freq = 0);
		void addDoc(int freq);
		void addTotFreq(int freq);
		void addNumDocs(int ndocs);
		void remDoc();
		
		string getToken() const;
		int getNumDocs() const;
		int getTotFreq() const;
		
		string toString() const;

};











#endif
