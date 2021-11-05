//THIS FILE USES ALL ASSERTS AND IS DEFAULT DEBUG use -NDEBUG switch to disable asserts

//compiler switches:
// -NDEBUG
// -DOUBLE_PRECISION


#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

#include <cassert>

#if defined(_WIN32) || defined(_WIN64)
    #define P_WINDOWS
#endif

#ifndef NDEBUG
    #define DCHECK(x) { assert(x);}
#endif

//USING DOUBLE PRECISION OR DOUBLE PRECISION
#ifdef DOUBLE_PRECISION
    typedef double Float;
#else  
    typedef float Float;
#endif


 //Contains Vector class and related utility operation functions

 namespace gbGeometry
 {
     template <typename T>
     inline bool isNaN(const T x)
     {
         return std::isnan(x);
     }

     template<>
     inline bool isNaN(const int x)
     {
         return false;
     }

// VECTOR CLASS AND OPERATIONS
    //forward declarations
    template<typename T>
    class Point2;
    template<typename T>
    class Point3;
    template<typename T>
    class Normal3;


    template<typename T>
    class Vector2
    {
        public:
        Vector2() { x = y = 0;}
        Vector2(T xx, T yy) : x(xx) , y(yy) { DCHECK(!HasNaNs()); }
        bool HasNaNs() const { return isNaN(x) || isNaN(y); }
        explicit Vector2(const Point2<T>& p);
        explicit Vector2(const Point3<T>& p);
    #ifndef NDEBUG
        //Default versions ok for release ver but for debug we use asserts
        Vector2(const Vector2<T> &v)
        {
            DCHECK(!v.HasNaNs());
            x = v.x;
            y = v.y;
        }
        Vector2<T>& operator=(const Vector2<T>& v)
        {
            DCHECK(!v.HasNaNs());
            x = v.x;
            y = v.y;
            return (*this);
        }
    #endif //NDEBUG

        Vector2<T> operator+(const Vector2<T>& v) const{
            DCHECK(!v.HasNaNs());
            return Vector2(v.x + x, v.y + y);
        }
        Vector2<T>& operator+=(const Vector2<T>& v)
        {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            return *this;
        }

        Vector2<T> operator-(const Vector2<T>& v) const{
            DCHECK(!v.HasNaNs());
            return Vector2(x - v.x, y - v.y);
        }
        Vector2<T>& operator-=(const Vector2<T>& v)
        {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            return *this;
        }

        bool operator==(const Vector2<T>& v) const { return x == v.x && y == v.y; }
        bool operator!=(const Vector2<T>& v) const { return x != v.x || y != v.y; }

        template<typename U>
        Vector2<T> operator*(U f) const{
            return Vector2<T>(f*x, f*y);
        }
        template<typename U>
        Vector2<T>& operator*=(U f){
            DCHECK(!isNaN(f));
            x *= f;
            y *= f;
            return *this;
        }

        template<typename U>
        Vector2<T> operator/(U f) const{
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1 / f;
            return Vector2<T>(x * inv, y *inv);
        }

        template<typename U>
        Vector2<T>& operator/=(U f){
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1 / f;     
            x *= inv;
            y *= inv;

            return *this;
        }

        Vector2<T> operator-() const { return Vector2<T>(-x,-y);}
        T operator[](int i) const{
            DCHECK(i >= 0 && i <= 1);
            if(i == 0) return x;
            return y;
        }

        T& operator[](int i){
            DCHECK(i >= 0 && i <= 1);
            if(i == 0) return x;
            return y;
        } 

        Float LengthSquared() const { return x * x + y * y;}
        Float Length() const { return std::sqrt(LengthSquared());}

        //Vector2 public data
        T x , y;
    };

    template<typename T>
    inline std::ostream& operator<<(std::ostream& os , const Vector2<T> &v)
    {
        os << "[ " << v.x << ", " << v.y << " ]";
        return os;
    }

    template<>
    inline std::ostream& operator<<(std::ostream& os , const Vector2<float> &v)
    {
        char x[256];
        char y[256];
        
        sprintf(x,"%f " , v.x);
        sprintf(y,"%f " , v.y);
        
        os << "[ " << x << ", " << y << " ]";
        return os;
    }

    template<typename T>
    class Vector3
    {
        public:

        Vector3() { x= y = z = 0; }
        Vector3(T xx, T yy ,T zz) : x(xx) , y(yy) ,z(zz) { DCHECK(!HasNaNs());}
        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        explicit Vector3(const Point3<T>& p);
    #ifndef NDEBUG
        Vector3(const Vector3<T>& v)
        {
            DCHECK(!v.HasNaNs());
            x = v.x;
            y = v.y;
            z = v.z;
        }

        Vector3<T>& operator=(const Vector3<T> &v)
        {
            DCHECK(!v.HasNaNs());
            x = v.x;
            y = v.y;
            z = v.z;    
            return *this;
        }
    #endif

    T operator[](int i) const{
        DCHECK(i >= 0 && i <= 2);
        if(i == 0) return x;
        if(i == 1) return y; 
        return z;
    }

    T& operator[](int i){
        DCHECK(i >= 0 && i <= 2);
        if(i == 0) return x;
        if(i == 1) return y; 
        return z;
    }

    Vector3<T> operator+(const Vector3<T> &v)
    {
        DCHECK(!v.HasNaNs());
        return Vector3(x + v.x , y + v.y , z + v.z);
    }
    Vector3<T>& operator+=(const Vector3<T>& v)
    {
        DCHECK(!v.HasNaNs());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3<T> operator-(const Vector3<T> &v)
    {
        DCHECK(!v.HasNaNs());
        return Vector3<T>(x - v.x , y - v.y , z - v.z);
    }

    Vector3<T>& operator-=(const Vector3<T>& v)
    {
        DCHECK(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template<typename U>
    Vector3<T> operator*(U f) const{
        return Vector3<T>(f * x, f * y , f * z);
    }
    template<typename U>
    Vector3<T>& operator*=(U f) {
        DCHECK(!isNaN(f));
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    bool operator==(const Vector3<T>& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vector3<T>& v) const{
        return x != v.x || y != v.y || z != v.z;
    }
    
    template<typename U>
    Vector3<T> operator/(U f) const
    {
        DCHECK(f != static_cast<U>(0));
        Float inv = (Float)1 / f;
        return Vector3<T>(x * inv, y * inv, z * inv);
    }
    template<typename U>
    Vector3<T>& operator/=(U f)
    {
        DCHECK(f != static_cast<U>(0));
        Float inv = (Float) 1/ f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }

    Vector3<T> operator-() const { return Vector3<T>(-x,-y,-z);}
    Float LengthSquared() const { return x*x + y*y + z*z;}
    Float Length() const { return std::sqrt(LengthSquared());}
    explicit Vector3(const Normal3<T>&n);
    
       //public data members
        T x , y, z;

    };


     template<typename T>
    inline std::ostream& operator<<(std::ostream& os , const Vector3<T> &v)
    {
        os << "[ " << v.x << ", " << v.y << ", " << v.z <<  " ]";
        return os;
    }

    template<>
    inline std::ostream& operator<<(std::ostream& os , const Vector3<float> &v)
    {
        char x[256];
        char y[256];
        char z[256];
        
        sprintf(x,"%f " , v.x);
        sprintf(y,"%f " , v.y);
        sprintf(z,"%f " , v.z);
        
        os << "[ " << x << ", " << y << ", " << z <<  " ]";
        return os;
    }

typedef Vector2<Float> Vec2f;
typedef Vector2<int> Vec2i;
typedef Vector3<Float> Vec3f;
typedef Vector3<int> Vec3i;

    //operations on point are limited
    template<typename T>
    class Point2
    {
        public:
        explicit Point2(const Point3<T>& p) : x(p.x) , y(p.y) { DCHECK(!HasNaNs()); }
        Point2() { x = y = 0;}
        Point2( T xx , T yy) : x(xx) , y(yy) { DCHECK(!HasNaNs()); }

        template<typename U>
        explicit Point2(const Point2<U> &p)
        {
            x = (T)p.x;
            y = (T)p.y;
            DCHECK(!HasNaNs());
        }
        template<typename U>
        explicit Point2(const Vector2<U> &p)
        {
            x = (T)p.x;
            y = (T)p.y;
            DCHECK(!HasNaNs());
        }

        template<typename U>
        explicit operator Vector2<U>() const{
            return Vector2<U>(x,y);
        }

    #ifndef NDEBUG
        Point2(const Point2<T> &p)
        {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
        }
        Point2<T>& operator=(const Point2<T>& p)
        {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
            return *this;
        }
    #endif

        Point2<T> operator+(const Vector2<T>& v) const{
            DCHECK(!v.HasNaNs());
            return Point2<T>(x + v.x , y + v.y);
        }
        Point2<T>& operator+=(const Vector2<T>& v) 
        {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2<T> operator-(const Point2<T> &p) const{
            DCHECK(!p.HasNaNs());
            return Vector2<T>(x - p.x , y  - p.y);
        }
        Point2<T> operator-(const Vector2<T>& v) const{
            DCHECK(!v.HasNaNs());
            return Point2<T>(x - v.x, y - v.y);
        }
        Point2<T> operator-() const { return Point2<T>(-x,-y);}
        Point2<T>& operator-=(const Vector2<T>& v){
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            return *this;
        }
        Point2<T>& operator+=(const Point2<T>& p)
        {
            DCHECK(!p.HasNaNs());
            x += p.x;
            y += p.y;
            return *this;
        }
        Point2<T> operator+(const Point2<T>& p)
        {
            DCHECK(!p.HasNaNs());
            return Point2<T>(x +p.x , y + p.y);
        }
        
        template<typename U>
        Point2<T> operator*( U f) const{
            return Point2<T>(f * x, f * y);
        }
        template<typename U>
        Point2<T>& operator*=(U f){
            x *= f;
            y *= f;
            return *this;
        }
        template<typename U>
        Point2<T> operator/(U f) const{
            DCHECK(f != static_cast<U>(0));
            Float inv =  (Float)1 / f;
            return Point2<T>(inv * x , inv * y);
        }
        template<typename U>
        Point2<T>& operator/=(U f) {
            DCHECK(f != static_cast<U>(0));
            Float inv =  (Float)1 /f;
            x *= inv;
            y *= inv;
            return *this;
        }
        T operator[](int i) const
        {
            DCHECK(i >= 0 && i <= 1);
            if( i == 0) return x;
            return y;
        }
        T& operator[](int i){
        
            DCHECK(i >= 0 && i <= 1);
            if( i == 0) return x;
            return y;           
        }

        bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
        bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
        bool HasNaNs() const { return isNaN(x) || isNaN(y) ; }
        
        //POint2 public Data
        T x , y;
    };

    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const Point2<T> &v)
    {
        os << "[ " << v.x << ", " << v.y << " ]";
        return os;
    }

    template<>
    inline std::ostream& operator<<(std::ostream &os, const Point2<Float> &v){
        char x[256];
        char y[256];
        sprintf(x,"%f ",v.x);
        sprintf(y,"%f ",v.y);
        os << "[ " << x << ", " << y << " ]";
        return os;
    }

    template<typename T>
    class Point3
    {
        public:
        Point3() { x = y = z = 0; }
        Point3(T x , T y , T z) : x(x) , y(y) , z(z) { DCHECK(!HasNans()); }
        template<typename U>
        explicit Point3(const Point3<U>& p) : x((T)p.x),y((T)p.y),z((T)p.z){
            DCHECK(!HasNans());
        }
        template<typename U>
        explicit operator Vector3<U>() const{
            return Vector3<U>(x , y , z);
        }

    #ifndef NDEBUG

        Point3(const Point3<T> &p)
        {
            DCHECK(!p.HasNaNs());
            x = p.x;
            y = p.y;
            z = p.z;
        }
        Point3<T>& operator=(const Point3<T>& p)
        {
            DCHECK(!p.HasNans());
            x = p.x ;
            y = p.y;
            z = p.z;
        }

    #endif

        Point3<T> operator+(const Vector3<T> &v) const{
            DCHECK(!v.HasNaNs());
            return Point3<T>(x + v.x, y + v.y , z + v.z);
        }
        Point3<T>& operator+=(const Vector3<T>& v)
        {
            DCHECK(!v.HasNaNs());
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> operator-(const Point3<T>& p) const{
            DCHECK(!p.HasNans());
            return Vector3<T>(x - p.x, y - p.y, z - p.z);
        }
        Point3<T> operator-(const Vector3<T> &v) const{
            DCHECK(!v.HasNaNs());
            return Point3<T>(x - v.x, y - v.y, z - v.z);
        }
        Point3<T>& operator-=(const Vector3<T> &v)
        {
            DCHECK(!v.HasNaNs());
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        Point3<T>& operator+=(const Point3<T> &p)
        {
            DCHECK(!p.HasNaNs());
            x += p.x;
            y += p.y;
            z += p.z;
            return *this;
        }
        Point3<T> operator+(const Point3<T>& p) const{
            DCHECK(!p.HasNans());
            return Point3<T>(x + p.x, y + p.y, z + p.z);
        }

        template<typename U>
        Point3<T> operator*(U f) const{
            return Point3<T>(f * x, f * y, f * z);
        }
        template<typename U>
        Point3<T>& operator*=(U f)
        {
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }
        
        template<typename U>
        Point3<T> operator/(U f) const{
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1 / f;
            return Point3(inv * x, inv * y, inv * z);
        }
        template<typename U>
        Point3<T>& operator/=(U f){
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1/ f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        T operator[](int i) const{
            DCHECK(i >= 0 && i <= 2);
            if(i == 0) return x;
            if(i == 1) return y;

            return z;
        }
        
        T& operator[](int i) {
            DCHECK(i >= 0 && i <= 2);
            if(i == 0) return x;
            if(i == 1) return y;
            return z;
        }

        bool operator==(const Point3<T>& p) const{
            return x==p.x && y == p.y && z == p.z;
        }

        bool operator!=(const Point3<T>& p) const{
            return x!=p.x || y != p.y || z != p.z;
        }

        bool HasNans() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        Point3<T> operator-() const { return Point3<T>(-x,-y,-z); }

        //public data members
        T x , y, z;
    };

     template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const Point3<T> &v)
    {
        os << "[ " << v.x << ", " << v.y <<  ", " << v.z <<  " ]";
        return os;
    }

    template<>
    inline std::ostream& operator<<(std::ostream &os, const Point3<Float> &v){
        char x[256];
        char y[256];
        char z[256];

        sprintf(x,"%f ",v.x);
        sprintf(y,"%f ",v.y);
        sprintf(z,"%f ",v.z);

        os << "[ " << x << ", " << y << ", " << z  << " ]";
        return os;
    }

typedef Point2<Float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

    //Normal is kind of like a Vector but is a separate entity in geometry

    template<typename T>
    class Normal3
    {
        public:
        Normal3() { x = y = z = 0; }
        Normal3( T xx , T yy , T zz) : x(xx) , y(yy) , z(zz) { DCHECK(!HasNaNs()); }
       
        Normal3<T> operator-() const { return Normal3<T>(-x,-y,-z); }
        Normal3<T> operator+(const Normal3<T>& n) const{
            DCHECK(!n.HasNaNs());
            return Normal3<T>(x + n.x, y + n.y, z + n.z);
        }
        Normal3<T>& operator+=(const Normal3<T>&n )
        {
            DCHECK(!n.HasNaNs());
            x += n.x;
            y += n.y;
            z += n.z;
            return *this;
        }

        Normal3<T> operator-(const Normal3<T>& n) const{
            DCHECK(!n.HasNaNs());
            return Normal3<T>(x - n.x, y - n.y, z - n.z);
        }
        Normal3<T>& operator-=(const Normal3<T>& n) {
            DCHECK(!n.HasNaNs());
            x -= n.x;
            y -= n.y;
            z -= n.z;
            return *this;
        }

        bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
        template<typename U>
        Normal3<T> operator*(U f) const{
            return Normal3<T>(f * x , f * y , f* z);
        }
        template<typename U>
        Normal3<T>& operator*=(U f) const{
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }

        template<typename U>
        Normal3<T> operator/(U f) const{
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1 / f;
            return Normal3<T>(x * inv, y * inv, z * inv);
        }

        template<typename U>
        Normal3<T>& operator/=(U f){
            DCHECK(f != static_cast<U>(0));
            Float inv = (Float)1 / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        Float LengthSquared() const { return x*x + y * y + z*z; }
        Float Length() const { return std::sqrt(LengthSquared()); }
    #ifndef NDEBUG
        Normal3<T>(const Normal3<T>& n)
        {
            DCHECK(!n.HasNaNs());
            x = n.x;
            y = n.y;
            z = n.z;
        }

        Normal3<T>& operator=(const Normal3<T>& n)
        {
            DCHECK(!n.HasNaNs());
            x = n.x;
            y = n.y;
            z = n.z;
            return *this;
        }
    #endif 

        explicit Normal3<T>(const Vector3<T> &v) : x(v.x) , y(v.y) , z(v.z){
            DCHECK(!v.HasNaNs());
        }

        bool operator==(const Normal3<T>& n) const {
            return x== n.x && y ==n.y & z== n.z;
        }
        bool operator!=(const Normal3<T>& n) const {
            return x != n.x || y != n.y || z != n.z;
        }

        T operator[](int i) const{
            DCHECK(i >= 0 && i <= 2);
            if(i == 0) return x;
            if(i == 1) return y;

            return z;
        }

        T& operator[](int i) {
            DCHECK(i >= 0 && i <= 2);
            if(i == 0) return x;
            if(i == 1) return y;
            return z;
        }

        //Normal3<T> public Data
        T x , y , z;
    };

    template<typename T> 
    inline std::ostream &operator<<(std::ostream& os, const Normal3<T>& n)
    {
        os << "[ " << n.x << ", " << n.y << ", " << n.z << " ]";
        return os;
    }

    template<> 
    inline std::ostream &operator<<(std::ostream& os, const Normal3<Float>& n)
    {
        char x[256];
        char y[256];
        char z[256];
        
        sprintf(x,"%f",n.x);
        sprintf(y,"%f",n.y);
        sprintf(z,"%f",n.z);
        os << "[ " << x << ", " << y << ", " << z << " ]";
        return os;
    }

typedef Normal3<Float> Normal3f;

//Geometry inline functions

    template<typename T>
    inline Vector3<T>::Vector3(const Point3<T>& p) : x(p.x), y(p.y), z(p.z){
        DCHECK(!HasNaNs());
    }

    template<typename T, typename U>
    inline Vector3<T> operator*(U s, const Vector3<T>& v){
        return v * s;
    }
    
    template<typename T>
    Vector3<T> Abs(const Vector3<T>& v)
    {
        return Vector3<T>(std::abs(v.x), std::abs(v.y),std::abs(v.z));
    }

    template<typename T>
    inline T Dot(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        return (v1.x * v2.x + v1.y * v2.y + v1.z*v2.z);
    }

    template<typename T>
    inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        return std::abs(v1.x * v2.x + v1.y * v2.y + v1.z*v2.z);
        
    }

    template<typename T>
    inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;

        return Vector3<T>(
            (v1y*v2z)-(v1z*v2y),
            (v2x*v1z)-(v1x*v2z),
            (v1x*v2y)-(v2x*v1y)
            );
    }

    template<typename T>
    inline Vector3<T> Cross(const Vector3<T>& v1, const Normal3<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;

        return Vector3<T>((v1y*v2z)-(v1z*v2y),(v2x*v1z)-(v1x*v2z),(v1x*v2y)-(v2x*v1y));
    }

    template<typename T>
    inline Vector3<T> Cross(const Normal3<T>& v1, const Vector3<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;

        return Vector3<T>((v1y*v2z)-(v1z*v2y),(v2x*v1z)-(v1x*v2z),(v1x*v2y)-(v2x*v1y));
    }

    template<typename T>
    inline Vector3<T> Normalize(const Vector3<T>& v)
    {
        return v / v.Length();
    }

    template<typename T>
    T MinComponent(const Vector3<T>& v)
    {
        return std::min(v.x, std::min(v.y,v.z));
    }

    template<typename T>
    T MaxComponent(const Vector3<T>& v)
    {
        return std::max(v.x,std::max(v.y,v.z));
    }

    template<typename T>
    int MaxDimension(const Vector3<T>& v)
    {
        return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
    }

    template<typename T>
    Vector3<T> Min(const Vector3<T> &p1, const Vector3<T> &p2)
    {
        return Vector3<T>(std::min(p1.x,p2.x),std::min(p1.y,p2.y), std::min(p1.z,p2.z));
    }

    template<typename T>
    Vector3<T> Max(const Vector3<T> &p1, const Vector3<T> &p2)
    {
        return Vector3<T>(std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z));
    }

    //like swizzle
    template<typename T>
    Vector3<T> Permute(const Vector3<T> &v, int x, int y , int z)
    {
        return Vector3<T>(v[x],v[y],v[z]);
    }

    template<typename T>
    //assumed that v1 is normalized
    inline void CoordinateSystem(const Vector3<T>& v1, const Vector3<T> *v2, Vector3<T> *v3)
    {
        if(std::abs(v1.x) > std::abs(v1.y))
            *v2 = Vector3<T>(-v1.z,0,v1.x) / std::sqrt(v1.x*v1.x+v1.z*v1.z);
        else
            *v2 = Vector3<T>(0,v1.z,-v1.y) / std::sqrt(v1.y*v1.y + v1.z*v1.z);
        
        *v3 = Cross(v1,*v2);
    }

    template<typename T>
    Vector2<T>::Vector2(const Point2<T> &p) : x(p.x),y(p.y)
    {
        DCHECK(!HasNaNs());
    }

    template<typename T>
    Vector2<T>::Vector2(const Point3<T>& p) : x(p.x), y(p.y)
    {
        DCHECK(!HasNaNs());
    }

    template<typename T, typename U>
    inline Vector2<T> operator*(U f, const Vector2<T> &v)
    {
        return v * f;
    }

    template<typename T>
    inline Float Dot(const Vector2<T> &v1, const Vector2<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        return v1.x*v2.x + v1.y*v2.y;
    }

    template<typename T>
    inline Float AbsDot(const Vector2<T> &v1, const Vector2<T>& v2)
    {
        DCHECK(!v1.HasNaNs() && !v2.HasNaNs());
        return std::abs(v1.x*v2.x + v1.y*v2.y);
    }

    template<typename T>
    inline Vector2<T> Normalize(const Vector2<T> &v)
    {
        return v / v.Length();
    }

    template<typename T>
    Vector2<T> Abs(const Vector2<T>& v)
    {
        return Vector2<T>(std::abs(v.x),std::abs(v.y));
    }

    template<typename T>
    inline Float Distance(const Point3<T> &p1, const Point3<T>& p2)
    {
        return (p1 - p2).Length();
    }

    template<typename T>
    inline Float DistanceSquared(const Point3<T> &p1, const Point3<T> &p2)
    {
        return (p1 - p2).LengthSquared();
    }

    template<typename T, typename U>
    inline Point3<T> operator*(U f, const Point3<T> &p)
    {
        DCHECK(!p.HasNaNs());
        return p * f;
    }

    template<typename T>
    Point3<T> Lerp(Float t, const Point3<T> &p0, const Point3<T> &p1)
    {
        return (1 - t) * p0 + t * p1;
    }

    template<typename T>
    Point3<T> Max(const Point3<T> &p1, const Point3<T> &p2){
        return Point3<T>(std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z));
    }

    template<typename T>
    Point3<T> Floor(const Point3<T> &p)
    {
        return Point3<T>(std::floor(p.x),std::floor(p.y),std::floor(p.z));
    }

    template<typename T>
    Point3<T> Ceil(const Point3<T> &p)
    {
        return Point3<T>(std::ceil(p.x),std::ceil(p.y),std::ceil(p.z));    
    }

    template<typename T>
    Point3<T> Abs(const Point3<T> &p)
    {
        return Point3<T>(std::abs(p.x),std::abs(p.y),std::abs(p.z));
    }

    template<typename T>
    inline Float Distance(const Point2<T> &p1, const Point2<T> &p2)
    {
        return (p1 - p2).Length();
    }

    template<typename T>
    inline Float DistanceSquared(const Point2<T> &p1, const Point2<T> &p2)
    {
        return (p1 - p2).LengthSquared();
    }

    template<typename T, typename U>
    inline Point2<T> operator*(U f, const Point2<T> &p)
    {
        DCHECK(!p.HasNaNs());
        return p * f;
    }

    template <typename T>
    Point2<T> Floor(const Point2<T> &p)
    {
        return Point2<T>(std::floor(p.x),std::floor(p.y));
    }

    template <typename T>
    Point2<T> Ceil(const Point2<T> &p)
    {
        return Point2<T>(std::ceil(p.x),std::ceil(p.y));
    }

    template <typename T>
    Point2<T> Lerp(Float t, const Point2<T>& p1, const Point2<T>& p2)
    {
        return (1 - t) * p1 + t * p2;
    }

    template<typename T>
    Point2<T> Min(const Point2<T> &p1 , const Point2<T> &p2)
    {
        return Point2<T>(std::min(p1.x,p2.x),std::min(p1.y,p2.y));
    }

    template<typename T>
    Point2<T> Max(const Point2<T> &p1 , const Point2<T> &p2)
    {
        return Point2<T>(std::max(p1.x,p2.x),std::max(p1.y,p2.y));
    }

    template<typename T>
    Point3<T> Permute(const Point3<T> &p, int x, int y, int z)
    {
        return Point3<T>(p[x],p[y],p[z]);
    }

    template<typename T, typename U>
    inline Normal3<T> operator*(U f, const Normal3<T>& n)
    {
        return Normal3<T>(f * n.x , f * n.y, f * n.z);
    }

    template<typename T>
    inline Normal3<T> Normalize(const Normal3<T> &n)
    {
        return n / n.Length();
    }

    template<typename T>
    inline Vector3<T>::Vector3(const Normal3<T> &n) : x(n.x),y(n.y),z(n.z)
    {
        DCHECK(!n.HasNaNs());
    }


    template<typename T>
    inline T Dot(const Normal3<T> &n1, const Vector3<T> &v2)
    {
        DCHECK(!n1.HasNaNs() && !v2.HasNaNs());

        return n1.x*v2.x + n1.y*v2.y + n1.z*v2.z;
    }

    template<typename T>
    inline T Dot(const Normal3<T> &n1, const Normal3<T> &n2)
    {
        DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
        return n1.x*n2.x + n1.y*n2.y + n1.z*n2.z;
    }

    template<typename T>
    inline T AbsDot(const Normal3<T> &n1, const Vector3<T> &v2)
    {
        DCHECK(!n1.HasNaNs() && !v2.HasNaNs());
        return std::abs(n1.x * v2.x + n1.y*v2.y + n1.z*v2.z);
    }

    template<typename T>
    inline T AbsDot(const Normal3<T> &n1, const Normal3<T> &n2)
    {
        DCHECK(!n1.HasNaNs() && !n2.HasNaNs());
        return std::abs(n1.x * n2.x + n1.y*n2.y + n1.z*n2.z);
    }

    template<typename T>
    inline Normal3<T> Faceforward(const Normal3<T> &n, const Vector3<T> &v)
    {
        return (Dot(n,v) < 0.f) ? -n : n;
    }


    template<typename T>
    inline Normal3<T> Faceforward(const Normal3<T> &n, const Normal3<T> &v)
    {
        return (Dot(n,v) < 0.f) ? -n : n;
    }
    
    template<typename T>
    inline Vector3<T> Faceforward(const Vector3<T> &v1, const Vector3<T> &v2)
    {
        return (Dot(v1,v2) < 0.f) ? -v1 : v1;
    }

    template<typename T>
    inline Vector3<T> Faceforward(const Vector3<T> &v, const Normal3<T> &n)
    {
        return (Dot(v,n) < 0.f) ? -v : v;
    }

    template<typename T>
    Normal3<T> Abs(const Normal3<T> &v)
    {
        return Normal3<T>(std::abs(v.x),std::abs(v.y),std::abs(v.z));
    }

 }


#endif
