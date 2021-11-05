#ifndef _GEOMETRY_5_H__
#define _GEOMETRY_5_H__

#include <cmath>
#include <fstream>
#include <vector>

template<class T>
struct vec2
{
	T x, y;
	vec2<T>() : x(T()), y(T()) {}
	vec2<T>(T _x, T _y) : x(_x), y(_y) {}
	vec2<T>(const vec2<T>& v) : x(T()), y(T()) { *this = v; }
	vec2<T>& operator =(const vec2<T>& v)
	{
		if (this != &v)
		{
			x = v.x;
			y = v.y;
		}

		return *this;
	}
	vec2<T> operator +(const vec2<T>& V) const { return vec2<T>(x + V.x, y + V.y); }
	vec2<T> operator -(const vec2<T>& V) const { return vec2<T>(x - V.x, y - V.y); }
	vec2<T> operator *(float t) const { return vec2<T>(x * t, y * t); }
	T& operator[](const int i) { if (i == 0) return x; else return y; }
	template<class > friend std::ostream& operator<<(std::ostream& s, vec2<T>& v);
};

template <class T>
struct vec3
{
	T x, y ,z;

	vec3<T>() : x(T()), y(T()), z(T()) {}
	vec3<T>(T _x, T _y , T _z) : x(_x), y(_y),z(_z) { }
	template<class u> vec3<T>(const vec3<u>& v);

	vec3<T>(const vec3<T>& v) : x(T()), y(T()),z(T()) { *this = v; }
	vec3<T>& operator =(const vec3<T>& v)
	{
		if (this != &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

		return *this;
	}

	vec3<T> operator^ (const vec3<T>& v) const { return vec3<T>(y* v.z - z * v.y, z* v.x - x * v.z, x* v.y - y * v.x); }
	vec3<T> operator +(const vec3<T>& V) const { return  vec3<T> (x + V.x,  y + V.y , z + V.z); }
	vec3<T> operator -(const vec3<T> & V) const { return vec3<T>(x - V.x,  y - V.y,  z - V.z); }
	vec3<T> operator *(float t) const { return vec3<T>(x * t, y * t , z * t); }
	T operator*(const vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return std::sqrt(x * x + y * y+z * z); }
	vec3<T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
	T& operator[](const int i) { if (i <= 0) return x; else if (i == 1) return y; else return z; }
	template<class > friend std::ostream& operator<<(std::ostream & s, vec3<T> & v);

};

typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

typedef vec3<float> vec3f;
typedef vec3<int>   vec3i;


template<> template<> vec3<int>::vec3(const vec3<float>& v);
template<> template<> vec3<float>::vec3(const vec3<int>& v);

template<class T> std::ostream& operator<<(std::ostream& s, vec2<T>& v)
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class T> std::ostream& operator<<(std::ostream& s, vec3<T>& v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << " )\n";
	return s;
}


const int DEFAULT_ALLOC = 4;

class Matrix
{
	std::vector<std::vector<float> > m;
	int rows, cols;

public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);
	std::vector<float>& operator[](const int i);
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};





#endif //_GEOMETRY_5_H__