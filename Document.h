#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <list>
#include <string>

using namespace std;

class Document {

	private:
		string _filename;
		int _id;
		list<string> _content;
		list<string> _fullContent;
		
	public:
		Document(string filename);
		Document(Document &doc);

		string getFilename() const;
		int getId() const;

		list<string> getContent();
		void addContent(string w);
		void setFullContent(list<string> c);
		list<string> getFullContent();
		void printFullContent();
		
		static Document *getDocFromFile(string filename);
		static list<Document *> getDocsFromDir(string dirpath);
		
		string toString() const;
		
		bool operator==(const Document& doc) const;
		bool operator<(const Document& doc) const;
		bool operator>(const Document& doc) const;
};











#endif
