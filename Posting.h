#ifndef __POSTING_H__
#define __POSTING_H__

#include "Document.h"

class Posting {

	private:
		Document *_doc;
		int _freq;
		
	public:
		Posting(Document *doc, int freq);
		Document *getDoc();
		int getFreq();
		
		string toString() const;

};











#endif