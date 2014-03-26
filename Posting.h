#ifndef __POSTING_H__
#define __POSTING_H__

#include "Document.h"

class Posting {

	private:
		Document *_doc;
		int _freq;
		
	public:
		Posting(Document *doc, int freq);
		Document *getDoc() const;
		int getFreq() const;
		
		string toString() const;
		
		bool operator==(const Posting& p) const;
		bool operator<(const Posting& p) const;
		bool operator>(const Posting& p) const;

};











#endif
