#include "Matrix.h"


Matrix::Matrix(const uint64_t rows, const uint64_t columns) : _rows(rows), _columns(columns) {
	_matrix = std::make_unique<std::unique_ptr<int[]>[]>(_rows);
	for (std::size_t i = 0; i < _rows; i++) {
		_matrix[i] = std::make_unique<int[]>(_columns);
	}
}


int Matrix::at(const int ri, const int ci) const {
	return _matrix[ri][ci];
}

void Matrix::SetAllValues(const int value) {
	for (std::size_t i = 0;  i < _rows; i++) {
		for (std::size_t j = 0; j < _columns; j++) {
			_matrix[i][j] = value;
		}
	}
}
