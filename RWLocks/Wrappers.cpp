#include "Wrappers.h"

AutoReadWriteLock::AutoReadWriteLock() {
	::ZeroMemory(&_srwlock, sizeof(SRWLOCK));
	::InitializeSRWLock(&_srwlock);
}

AutoReadWriteLock::~AutoReadWriteLock() {

}

_Acquires_shared_lock_(_srwlock)
void AutoReadWriteLock::LockShared(void) {
	::AcquireSRWLockShared(&_srwlock);
}

_Releases_shared_lock_(_srwlock)
void AutoReadWriteLock::UnlockShared(void) {
	::ReleaseSRWLockShared(&_srwlock);
}

_Acquires_exclusive_lock_(_srwlock)
void AutoReadWriteLock::LockExclusive(void) {
	::AcquireSRWLockExclusive(&_srwlock);
}

_Releases_exclusive_lock_(_srwlock)
void AutoReadWriteLock::UnlockExclusive(void) {
	::ReleaseSRWLockExclusive(&_srwlock);
}
