#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry5.h"

template<> template<> vec3<int>::vec3(const vec3<float>& v) : x(int(v.x + 0.5)), y(int(v.y + 0.5)), z(int(v.z + 0.5)) {

}

template<> template<> vec3<float>::vec3(const vec3<int>& v) : x(v.x), y(v.y), z(v.z) {

}

Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))), rows(r), cols(c) {}

int Matrix::nrows()
{
	return rows;
}

int Matrix::ncols()
{
	return cols;
}

Matrix Matrix::identity(int dimensions)
{
	Matrix E(dimensions, dimensions);

	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			E[i][j] = (i == j ? 1.f : 0.f);
		}
	}

	return E;
}

std::vector<float>& Matrix::operator[](const int i)
{
	assert(i >= 0 && i < rows);
	return m[i];
}

Matrix Matrix::operator*(const Matrix& a)
{
	assert(cols == a.rows);
	Matrix result(rows, a.cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < a.cols; j++)
		{
			result.m[i][j] = 0.f;
			for (int k = 0; k < cols; k++)
			{
				result.m[i][j] += m[i][k] * a.m[k][j];
			}
		}
	}

	return result;
}

Matrix Matrix::transpose()
{
	Matrix result(cols, rows);
	
	for (int i = 0; i < rows; i++)
	{

		for (int j = 0; j < cols; j++)
		{
			result[j][i] = m[i][j];
		}
	}
	return result;
}

Matrix Matrix::inverse()
{
	assert(rows == cols);

	return 0;
}

std::ostream& operator<<(std::ostream& s, Matrix& m)
{
	for (int i = 0; i < m.nrows(); i++)
	{
		for (int j = 0; j < m.ncols(); j++)
		{
			s << m[i][j];

			if (j < m.ncols() - 1)
				s << "\t";
		}
		s << "\n";
	}
	return s;
}
