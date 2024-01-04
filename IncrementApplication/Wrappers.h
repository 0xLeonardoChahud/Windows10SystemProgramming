#include <Windows.h>

class AutoCriticalSection {
public:

	// The most important functions in this class
	// since it enables us to automatically do some cleanup and initialization
	// with the class constructor and destructor respectively.
	AutoCriticalSection();
	~AutoCriticalSection();

	 // We dont want copy constructors acting since it would copy
	 // the CRITICAL_SECTION object by default.
	AutoCriticalSection(const AutoCriticalSection&) = delete;

	// We dont want assignment operators for the same reason explained above.
	AutoCriticalSection& operator=(const AutoCriticalSection&) = delete;

	// We also dont want move semantics acting on our class.
	AutoCriticalSection(const AutoCriticalSection&&) = delete;
	AutoCriticalSection& operator=(const AutoCriticalSection&&) = delete;

	// Functions that will enter and leave the critical section.
	void Enter(void);
	void Leave(void);

private:
	CRITICAL_SECTION _cs;
};
