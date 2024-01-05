#pragma once
#include <Windows.h>


class AutoReadWriteLock {
public:
	AutoReadWriteLock();
	~AutoReadWriteLock();

	_Acquires_shared_lock_(_srwlock)
	void LockShared(void);

	_Releases_shared_lock_(_srwlock)
	void UnlockShared(void);

	_Acquires_exclusive_lock_(_srwlock)
	void LockExclusive(void);

	_Releases_exclusive_lock_(_srwlock)
	void UnlockExclusive(void);

	// Unused
	AutoReadWriteLock(const AutoReadWriteLock&) = delete;
	AutoReadWriteLock& operator=(const AutoReadWriteLock&) = delete;

	AutoReadWriteLock(AutoReadWriteLock&&) = delete;
	AutoReadWriteLock& operator=(const AutoReadWriteLock&&) = delete;


private:
	SRWLOCK _srwlock;

};