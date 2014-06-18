



#include "Document.h"
#include "tinydir.h"
#include <sstream>


using namespace std;


Document::Document(string filename) {
	static int id = 0;
	_id = id++;
	_filename = filename;
}

Document::Document(Document &doc) {
	_id = doc.getId();
	_filename = doc.getFilename();
	_content = doc.getContent();
}
		
string Document::getFilename() const {
	return _filename;
}

int Document::getId() const {
	return _id;
}


list<string> Document::getContent() {
	return _content;
}


void Document::addContent(string w) {
	_content.push_back(w);
}




Document *Document::getDocFromFile(string filename) {
	return new Document(filename);
}


list<Document *> Document::getDocsFromDir(string dirpath) {
	list<Document *> docs;
	
	tinydir_dir dir;
	int i;
	tinydir_open_sorted(&dir, dirpath.c_str());

	for (i = 0; i < dir.n_files; i++)
	{
		tinydir_file file;
		tinydir_readfile_n(&dir, &file, i);

		if (!file.is_dir)
		{
			docs.push_back(new Document(dirpath+"/"+file.name));
		}
	}

	tinydir_close(&dir);
	
	return docs;
}




bool Document::operator==(const Document& doc) const {
	return _id == doc.getId();
}


bool Document::operator<(const Document& doc) const {
	return _id < doc.getId();
}


bool Document::operator>(const Document& doc) const {
	return _id > doc.getId();
}




string Document::toString() const {
	ostringstream os;
	
	os << _filename << " (" << _id << ")";;
	return os.str();
}















