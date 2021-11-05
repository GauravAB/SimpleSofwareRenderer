#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include "geometry.h"

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;
    std::vector<Vec2f> sts_;
    
public:

    Model(const char* filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec2f st(int i);
    std::vector<int> face(int idx);
};

#endif //_MODEL_H