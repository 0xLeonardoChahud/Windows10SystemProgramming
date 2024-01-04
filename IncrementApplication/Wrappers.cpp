#include "Wrappers.h"

AutoCriticalSection::AutoCriticalSection() {
	::InitializeCriticalSection(&_cs);
}

AutoCriticalSection::~AutoCriticalSection() {
	::DeleteCriticalSection(&_cs);
}


void AutoCriticalSection::Enter(void) {
	::EnterCriticalSection(&_cs);
}


void AutoCriticalSection::Leave(void) {
	::LeaveCriticalSection(&_cs);
}