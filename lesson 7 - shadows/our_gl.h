#ifndef _OUR_GL_H_
#define _OUR_GL_H_

#include "../common/tgaimage.h"
#include "geometry.h"
extern Matrix ModelView;
extern Matrix Projection;
extern Matrix Viewport;
const float depth = 2000.f;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f);
void lookat(vec3f eye, vec3f center, vec3f up);

struct IShader
{
	virtual ~IShader();
	virtual vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3f bar, TGAColor& color) = 0;
};

void triangle(vec4f* pts, IShader& shader, TGAImage& image, float* zbuffer);

#endif //_OUR_GL_H_
