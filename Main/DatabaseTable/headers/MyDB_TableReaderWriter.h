
#ifndef TABLE_RW_H
#define TABLE_RW_H

#include <memory>
#include "MyDB_BufferManager.h"
#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_Table.h"
#include "stdlib.h"

// create a smart pointer for the catalog
using namespace std;
class MyDB_PageReaderWriter;
class MyDB_TableReaderWriter;
class TableRecordIterator;

typedef shared_ptr <MyDB_TableReaderWriter> MyDB_TableReaderWriterPtr;
typedef shared_ptr <TableRecordIterator> TableRecordItPtr;

class MyDB_TableReaderWriter {

public:

	// ANYTHING ELSE YOU NEED HERE

	// create a table reader/writer for the specified table, using the specified
	// buffer manager
	MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer);

	// gets an empty record from this table
	MyDB_RecordPtr getEmptyRecord ();

	// append a record to the table
	void append (MyDB_RecordPtr appendMe);

	// return an itrator over this table... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// load a text file into this table... overwrites the current contents
	void loadFromTextFile (string fromMe);

	// dump the contents of this table into a text file
	void writeIntoTextFile (string toMe);

	// access the i^th page in this file
	MyDB_PageReaderWriter operator[] (size_t pageIdx);

	// access the last page in the file
    MyDB_PageReaderWriter last ();

    // return the number of pages in the file
    size_t pageNum();

private:
	MyDB_TablePtr table;
	MyDB_BufferManagerPtr bufferMgr;
	// ANYTHING YOU NEED HERE
};



class TableRecordIterator: public MyDB_RecordIterator{

public:
	TableRecordIterator(MyDB_TableReaderWriterPtr tableReaderWriterPtr,  MyDB_RecordPtr iterateIntoMe);
	~TableRecordIterator();
	void getNext();
	bool hasNext();

private:
	MyDB_TableReaderWriterPtr tableRWPtr;
	MyDB_RecordPtr curRecord;
	size_t curPageIdx;
	MyDB_RecordIteratorPtr pageIt;
};



#endif
