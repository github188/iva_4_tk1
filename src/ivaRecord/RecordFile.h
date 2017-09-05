#ifndef __RECORD_FILE_H__
#define __RECORD_FILE_H__

void * RecordInitThread(void *arg);
void * FileCreateThread(void *arg);
void * FileWriteThread(void *arg);
void * FileCloseThread(void *arg);

#endif // __RECORD_FILE_H__
