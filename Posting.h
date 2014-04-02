#ifndef __POSTING_H__
#define __POSTING_H__

#include "Document.h"

class Posting {

	private:
		Document *_doc;
		int _freq;
		list<int> _positions;
		
	public:
		Posting(Document *doc, int freq, list<int> pos);
		Document *getDoc() const;
		int getFreq() const;
		list<int> getPositions();
		void addPosition(int pos);
		
		string toString() const;
		
		bool operator==(const Posting& p) const;
		bool operator<(const Posting& p) const;
		bool operator>(const Posting& p) const;

};











#endif
