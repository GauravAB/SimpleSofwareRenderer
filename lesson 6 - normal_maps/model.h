#ifndef __MODEL_H
#define __MODEL_H

#include <vector>
#include <string>
#include "geometry.h"
#include "../common/tgaimage.h"

class Model
{
private:
	std::vector<vec3f> verts_;
	std::vector<vec2f> uv_;
	std::vector<vec3f> norms_;
	std::vector<std::vector<vec3i>> faces_;
	TGAImage diffusemap_;
	TGAImage specularmap_;
	TGAImage normalmap_;
	TGAImage grid_;
	void load_texture(std::string filename, const char* suffx, TGAImage& img);
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
	TGAColor grid(vec2f uv);
	float specular(vec2f uv);

	std::vector<int> face(int idx);
};









#endif