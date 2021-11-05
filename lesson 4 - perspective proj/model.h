#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include "geometry5.h"
#include "../common/tgaimage.h"


class Model
{
private:
	std::vector<vec3f> verts_;
	std::vector<std::vector<vec3i>> faces_;
	std::vector<vec3f> norms_;
	std::vector<vec2f> uv_;
	TGAImage diffusemap_;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	vec3f vert(int i);
	vec2i uv(int iface, int nvert);
	TGAColor diffuse(vec2i uv);
	std::vector<int> face(int idx);
};




#endif //_MODEL_H