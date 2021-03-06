#ifndef __POSTING_H__
#define __POSTING_H__

#include "Document.h"
#include <list>

class Posting {

	private:
		Document *_doc;
		int _freq;
		list<int> _positions;
		double _degree;
		float _score;
		
	public:
		Posting(Document *doc, list<int> pos);
		Posting(Document *doc, int freq = 0, float score = 0);
		
		Document *getDoc() const;
		int getFreq() const;
		list<int> getPositions();
		void addPosition(int pos);
	
		void setDegree(double deg);
		double getDegree() const;

		string toString() const;
		
		bool operator==(const Posting& p) const;
		bool operator<(const Posting& p) const;
		bool operator>(const Posting& p) const;

};











#endif
