
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"



MyDB_PageReaderWriter::MyDB_PageReaderWriter(MyDB_PageHandle pageHandlePtr, bool isNew):
	pageHandle(move(pageHandlePtr)),
	type(RegularPage)
{
	auto page = (PageInfo*)(pageHandle->getBytes());
    if (isNew){
		page->offset = 0 ;
		pageHandle->wroteBytes();
	}
}

void MyDB_PageReaderWriter :: clear () {
	auto page = (PageInfo*)(pageHandle->getBytes());
	page->offset = 0;
	pageHandle->wroteBytes();
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	auto page = (PageInfo*)(pageHandle->getBytes());
	return page->type;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr recordPtr) {
	PageRecordItPtr pageRecordItPtr =  make_shared<PageRecordIterator>(pageHandle,recordPtr);
	return dynamic_pointer_cast<MyDB_RecordIterator>(pageRecordItPtr);
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType pageType) {
	auto page = (PageInfo*)(pageHandle->getBytes());
	page->type = pageType;
	pageHandle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr recordPtr) {

	auto page = (PageInfo*)(pageHandle->getBytes());
	// if the remaining space is too limited to append new record, the append operation fails
	size_t pageDataSize = pageHandle->getParent().getPageSize() - sizeof(page->offset) - sizeof(page->type);
	if(recordPtr->getBinarySize() + page->offset > pageDataSize){
		return false;
	}
	char* addr = (char*)recordPtr->toBinary(page->data+page->offset);
	page->offset = addr - page->data;
	pageHandle->wroteBytes();
	return true;
}

MyDB_PageHandle MyDB_PageReaderWriter :: getPageHandle(){
	return pageHandle;
}


/***  PageRecordIterator  ***/

PageRecordIterator::PageRecordIterator(MyDB_PageHandle pageHandle, MyDB_RecordPtr recordPtr):
	pageHandle(move(pageHandle)),
	recordPtr(move(recordPtr)),
	nextRecOffset(0)
{
}

PageRecordIterator::~PageRecordIterator() {

}

void PageRecordIterator::getNext() {
	auto page = (PageInfo*)(pageHandle->getBytes());
	char *nextLoc = (char*) recordPtr->fromBinary(page->data+nextRecOffset);
	nextRecOffset = nextLoc - page->data;
}

bool PageRecordIterator::hasNext() {
	auto page = (PageInfo*)(pageHandle->getBytes());
	return nextRecOffset < page->offset;
}

#endif
