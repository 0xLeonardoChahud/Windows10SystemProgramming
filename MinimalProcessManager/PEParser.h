#pragma once
#include <string>
#include <memory>
#include <Windows.h>
#include <wil/resource.h>

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
	PEParser( _In_ const std::wstring& exePath);
	~PEParser();

	void Reload(_In_ const std::wstring& exePath);

	bool is64bit(void) const noexcept { return this->is64bitProcess; }
	void clean(void) noexcept;
};