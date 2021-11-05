#ifndef _GEOMETRY_H__
#define _GEOMETRY_H__

#include <cmath>
#include <vector>
#include <fstream>

class Matrix;

template<class T>
struct vec2
{
	T x, y;
	vec2<T>() : x(T()), y(T()) {}
	vec2<T>(T x_, T y_) : x(x_), y(y_) {}
	vec2<T> operator+(const vec2<T>& V) const { return vec2<T>(x + V.x, y + V.y); }
	vec2<T> operator-(const vec2<T>& V) const { return vec2<T>(x - V.x, y - V.y); }
	vec2<T> operator*(const float t) const { return vec2<T>(x * t, y * t); }
	T& operator[](const int i) { return i <= 0 ? x : y; }
	template<class > friend std::ostream& operator<<(std::ostream& s, vec2<T>& v);
};

template<class T>
struct vec3
{
	T x, y, z;
	vec3<T>() : x(T()), y(T()), z(T()) {}
	vec3<T>(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
	vec3<T>(Matrix m);
	template<class u> vec3<T>(const vec3<u>& v);

	vec3<T>operator+(const vec3<T>& V) const { return vec3<T>(x + V.x, y + V.y, z + V.z); }
	vec3<T>operator-(const vec3<T>& V) const { return vec3<T>(x - V.x, y - V.y, z - V.z); }
	vec3<T>operator^(const vec3<T>& v) const {
		return vec3<T>(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}
	vec3<T>operator*(const float t) const { return vec3<T>(x * t, y * t, z * t); }
	T operator*(const vec3<T>& V) const { return ((x * V.x) + (y * V.y) + (z * V.z)); }
	float norm() { return std::sqrt(x * x + y * y + z * z); }
	vec3<T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
	T& operator[](const int i) { return((i == 0) ? x : ((i == 1) ? y : z)); }
	template<class> friend std::ostream& operator<<(std::ostream& s, vec3<T>& v);
};

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

template<> template<> vec3<int>::vec3(const vec3<float>& v);
template<> template<> vec3<float>::vec3(const vec3<int>& v);

template< class T> std::ostream& operator<<(std::ostream& s, vec2<T>& v)
{
	s << " ( " << v.x << ", " << v.y << ") \n";
	return s;
}

template< class T> std::ostream& operator<<(std::ostream& s, vec3<T>& v)
{
	s << " ( " << v.x << ", " << v.y << ", " << v.z <<  " ) \n";
	return s;
}

////////////////////////////////////////////////////////////////////////////////////////

class Matrix
{
	std::vector<std::vector<float> > m;
	int rows, cols;
public:
	Matrix(int r = 4, int c = 4);
	Matrix(vec3f v);
	int nrows();
	int ncols();
	static Matrix identity(int dimensions);
	std::vector<float>& operator[](const int i);
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();
	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};


#endif //_GEOMETRY_H__