
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <stdio.h>
#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer):
	table(forMe),
	bufferMgr(myBuffer){}


MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator[] (size_t pageIdx) {
	// if the requested page exceeds current last page in the table, we replenish all the pages from
	// current last page to pageIdx page first, then return the requested PageReaderWriter
	while (table->lastPage()<(int)pageIdx){
		int lastPage = table->lastPage()+1;
		table->setLastPage((size_t)lastPage);
        MyDB_PageHandle pageHandle = bufferMgr->getPage(table,table->lastPage());
		MyDB_PageReaderWriter newPage(pageHandle,true);
	}
	MyDB_PageHandle pageHandle = bufferMgr->getPage(table,pageIdx);
	return MyDB_PageReaderWriter(pageHandle, false);
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared<MyDB_Record>(table->getSchema());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	int lastPage = table->lastPage();
	lastPage = lastPage==-1? 0 : lastPage;
	return (*this)[lastPage];
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr record) {
	MyDB_RecordPtr recordPtr = move(record);
	if(!last().append(recordPtr)){
		(*this)[table->lastPage()+1].append(recordPtr);
	}
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	ifstream is(fromMe);
	string line;
	MyDB_RecordPtr rec = getEmptyRecord();
	table->setLastPage(-1);
	while (getline(is,line)){
		rec->fromString(line);
		append(rec);
	}
	is.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	MyDB_TableReaderWriterPtr tableRWPtr = make_shared<MyDB_TableReaderWriter>(table,bufferMgr);
	TableRecordItPtr tableRecordItPtr = make_shared<TableRecordIterator>(tableRWPtr,iterateIntoMe);
	return dynamic_pointer_cast<MyDB_RecordIterator>(tableRecordItPtr);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	ofstream os(toMe);
    MyDB_RecordPtr rec = getEmptyRecord();
	MyDB_RecordIteratorPtr recordIteratorPtr = getIterator(rec);
	while(recordIteratorPtr->hasNext()){
	    recordIteratorPtr->getNext();
	}
	os.close();

}


size_t MyDB_TableReaderWriter :: pageNum(){
	return (size_t) table->lastPage();
}



/*****  TableRecordIterator  *******/
TableRecordIterator::TableRecordIterator(MyDB_TableReaderWriterPtr tableReaderWriterPtr,  MyDB_RecordPtr iterateIntoMe):
	tableRWPtr(move(tableReaderWriterPtr)),
	curRecord(move(iterateIntoMe)),
	curPageIdx(0),
	pageIt((*tableRWPtr)[curPageIdx].getIterator(curRecord))
{}

TableRecordIterator::~TableRecordIterator(){

}

void TableRecordIterator::getNext(){
	return pageIt->getNext();
}

bool TableRecordIterator::hasNext(){
	// we iterate through all the pages to find next record,
	// if we don't find it even though we have already reached the last page, we return false;
	while (! pageIt->hasNext()){
		if (curPageIdx>=tableRWPtr->pageNum()) return false;
		pageIt = (*tableRWPtr)[++curPageIdx].getIterator(curRecord);
	}
	return true;
}

#endif

