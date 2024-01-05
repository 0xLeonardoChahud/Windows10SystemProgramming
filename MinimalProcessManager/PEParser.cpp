#include "PEParser.h"


PEParser::PEParser(
	_In_ const std::wstring& exePath
) : ptrDosHeader(nullptr), ptrNtHeader(nullptr), ptrOptHeader(nullptr), exePath(L""), is64bitProcess(false), fileBuffer(nullptr) 
{
	this->init();
}

void PEParser::init()  {
	if (this->exePath.empty())
		return;

	if (!this->exePath.compare(L"(none)"))
		return;
	this->clean();
	DWORD headerSize{ sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS64) + sizeof(IMAGE_OPTIONAL_HEADER64) };
	HANDLE hFile{ nullptr };
	bool error{ false };

	this->fileBuffer.reset(new byte[headerSize + 1]);
	if (fileBuffer == nullptr) {
		::wprintf(L"[-] Failed reading exe PE header\n");
		error = true;
		goto Exit;
	}

	hFile = ::CreateFile(this->exePath.data(), FILE_READ_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		::wprintf(L"[-] Failed opening file for read : %u\n", ::GetLastError());
		error = true;
		goto Exit;
	}

	if (!::ReadFile(hFile, this->fileBuffer.get(), headerSize, nullptr, nullptr)) {
		::wprintf(L"[-] Failed reading file : %u\n", ::GetLastError());
		error = true;
		goto Exit;
	}

	this->ptrDosHeader = (PIMAGE_DOS_HEADER)this->fileBuffer.get();
	this->ptrNtHeader = (PIMAGE_NT_HEADERS)(this->ptrDosHeader->e_lfanew + static_cast<PBYTE>(this->fileBuffer.get()));
	this->ptrOptHeader = static_cast<PIMAGE_OPTIONAL_HEADER>(&this->ptrNtHeader->OptionalHeader);
	
	this->is64bitProcess = (this->ptrOptHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) ? true : false;
	::wprintf(L"MAGIC: 0x%x", this->ptrOptHeader->Magic);

	Exit:
		if (fileBuffer && error) {
			this->fileBuffer.reset(nullptr);
		}

		if (hFile) {
			::CloseHandle(hFile);
		}

	

}

void PEParser::Reload(
	_In_ const std::wstring& newExePath
) 
{
	this->exePath = newExePath;
	this->init();
}

void PEParser::clean(void) {
	if (this->fileBuffer) {
		this->fileBuffer.reset(nullptr);
	}
	this->ptrOptHeader = nullptr;
	this->ptrNtHeader = nullptr;
	this->ptrOptHeader = nullptr;
	this->fileBuffer = nullptr;
	this->is64bitProcess = false;
}

PEParser::~PEParser() {
	this->clean();
}