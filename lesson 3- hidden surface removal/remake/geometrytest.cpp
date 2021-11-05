#include "geometry.h"
#include <cassert>
#include <iostream>
#include <string>
#include <cstdio>


using namespace gbGeometry;


void testvec3()
{
    Vec3f v1 = Vec3f(1,1,1);
    Vec3f v2 = Vec3f(2,2,2);

    assert(Dot(v1,v2) == (float)6);

    assert((v2 - v1) == Vec3f(1,1,1));
    assert((v2 + v1) == Vec3f(3,3,3));
    assert((v1*=10) == Vec3f(10,10,10));
    assert((v2/=2.) == (v1/10));
    assert( (v1/10).LengthSquared() == (3));
    assert(v2.Length() == (float)std::sqrt(3.));

    Vec3f x(1,0,0);
    Vec3f y(0,1,0);
    Vec3f z(0,0,1);

    assert(Dot(x,y) == 0);
    assert(Cross(x,y) == z);
    printf("%.20f \n" , Normalize(v2).Length());
    printf("%.20f \n" , Normalize(v1).Length());

    assert(Cross((z-Vec3f(0,0,0)),(y-Vec3f(0,0,0))) == (-x));
}


int main()
{
    testvec3();

    return 0;
}