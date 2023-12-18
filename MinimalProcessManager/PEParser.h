#pragma once
#include <string>
#include <memory>
#include <Windows.h>

class PEParser {
private:
	PIMAGE_DOS_HEADER ptrDosHeader;
	PIMAGE_NT_HEADERS ptrNtHeader;
	PIMAGE_OPTIONAL_HEADER ptrOptHeader;
	std::wstring exePath;
	bool is64bitProcess;
	std::unique_ptr<byte[]> fileBuffer;

	void init(void);

public:
	PEParser(const std::wstring& exePath);
	~PEParser();

	void Reload(const std::wstring& exePath);

	bool is64bit(void) const { return this->is64bitProcess; }
	void clean(void);
};