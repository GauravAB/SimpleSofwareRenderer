#ifndef _MODEL_H
#define _MODEL_H

#include "geometry.h"
#include "tgaimage.h"

class Model
{
    public:

    Model(const char* filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int index);
    Vec2f st(int index);
    std::vector<int> face(int index);
    TGAImage diffuse_map;
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec2f> sts_;
    std::vector<std::vector<int> > faces_;
};


#endif //_MODEL_H