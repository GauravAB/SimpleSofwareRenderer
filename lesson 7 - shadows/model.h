#ifndef __MODEL_H_
#define __MODEL_H_

#include <vector>
#include <string>
#include "geometry.h"
#include "../common/tgaimage.h"

class Model
{
private:
	std::vector<vec3f> verts_;
	std::vector<vec3f> norms_;
	std::vector<vec2f> uv_;
	std::vector<std::vector<vec3i>> faces_; //vec3i means v/vt/vn
	TGAImage diffusemap_;
	TGAImage specularmap_;
	TGAImage normalmap_;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	vec3f normal(int iface, int nthvert);
	vec3f normal(vec2f uv);
	vec3f vert(int i);
	vec3f vert(int iface, int nthvert);
	vec2f uv(int iface, int nthvert);
	TGAColor diffuse(vec2f uv);
	float specular(vec2f uv);
	std::vector<int> face(int idx);
};


#endif //__MODEL_H_