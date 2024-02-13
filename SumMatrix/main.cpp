#include <iostream>
#include <Windows.h>
#include "Matrix.h"

using ptrSumMatrix = uint64_t (*)(const Matrix&);

uint64_t SumMatrixCacheFriendly(const Matrix& matrix) {
	uint64_t sum{ 0 };
	for (std::size_t i = 0; i < matrix.Rows(); i++) {
		for (std::size_t j = 0; j < matrix.Columns(); j++) {
			sum += matrix.at(i, j);
		}
	}

	std::wcout << L"[Cache Friendly]";
	return sum;
}


uint64_t SumMatrix(const Matrix& matrix) {
	uint64_t sum{ 0 };
	for (std::size_t i = 0; i < matrix.Columns(); i++) {
		for (std::size_t j = 0; j < matrix.Rows(); j++) {
			sum += matrix.at(j, i);
		}
	}

	std::wcout << L"[Column Sum]";
	return sum;
}

void RunAndDisplayInfo(Matrix& matrix, const int value, ptrSumMatrix func) {
	matrix.SetAllValues(value);

	uint64_t Sum{ 0 };
	ULONGLONG elapsed{ 0 };
	const uint64_t Expected{ matrix.Rows() * matrix.Columns() * value };

	elapsed = ::GetTickCount64();
	Sum = func(matrix);
	elapsed = ::GetTickCount64() - elapsed;
	std::wcout << L"[ info ]\tElapsed: " << elapsed << " ms, Sum: " << Sum << L", Expected: " << Expected << L"\n" << std::endl;
}


int wmain(const int argc, const wchar_t* const argv[]) {


	for (std::size_t i = 10; i < 15; i++) {
		Matrix wideMatrix((std::size_t)1 << (i + 1), (std::size_t)1 << (i + 1));
		RunAndDisplayInfo(wideMatrix, 1, SumMatrixCacheFriendly);
		RunAndDisplayInfo(wideMatrix, 1, SumMatrix);

	}
	
	



	return 0;
}