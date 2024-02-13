#pragma once
#include <iostream>

class Matrix
{
public:
	Matrix(const uint64_t rows, const uint64_t columns);

	// Setters
	void SetAllValues(const int val);

	// Getters
	uint64_t Rows() const { return _rows; }
	uint64_t Columns() const { return _columns; }

	int at(const int ri, const int ci) const;

private:

	const uint64_t _rows;
	const uint64_t _columns;
	std::unique_ptr<std::unique_ptr<int[]>[]> _matrix;
};

