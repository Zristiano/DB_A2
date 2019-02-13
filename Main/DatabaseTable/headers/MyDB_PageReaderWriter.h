
#ifndef PAGE_RW_H
#define PAGE_RW_H

#include "MyDB_PageType.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_PageHandle.h"

class PageRecordIterator;
typedef shared_ptr<PageRecordIterator> PageRecordItPtr;

typedef struct{
    size_t offset;
    MyDB_PageType type;
    char data[0];
}PageInfo;

class MyDB_PageReaderWriter {

public:

	MyDB_PageReaderWriter(MyDB_PageHandle pageHandlePtr, bool isNew);
	// ANY OTHER METHODS YOU WANT HERE

	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an itrator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// appends a record to this page... return false is the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an ennumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);


	MyDB_PageHandle getPageHandle();
private:
	MyDB_PageHandle pageHandle;
	MyDB_PageType  type;
	// ANYTHING ELSE YOU WANT HERE
};


class PageRecordIterator: public MyDB_RecordIterator{
public:
    PageRecordIterator(MyDB_PageHandle pageHandle, MyDB_RecordPtr recordPtr);
    ~PageRecordIterator();
    void getNext ();
    bool hasNext ();

private:
    MyDB_PageHandle pageHandle;
    MyDB_RecordPtr recordPtr;
    size_t nextRecOffset;
};


#endif
