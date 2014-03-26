



#include "Document.h"
#include "tinydir.h"
#include <sstream>


using namespace std;


Document::Document(string filename) {
	static int id = 1;
	_id = id++;
	_filename = filename;
}
		
string Document::getFilename() {
	return _filename;
}

int Document::getId() {
	return _id;
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









string Document::toString() const {
	ostringstream os;
	
	os << _id << ": " << _filename;
	return os.str();
}














