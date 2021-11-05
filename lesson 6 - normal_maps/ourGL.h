#ifndef _OUR_GL_H_
#define _OUR_GL_H_

#include "../common/tgaimage.h"
#include "geometry.h"


extern Matrix ModelView;
extern Matrix Projection;
extern Matrix ViewPort;

void viewport(int x, int y, int width, int height);
void projection(float coeff = 0.f);
void lookat(vec3f eye, vec3f center, vec3f up);

struct IShader
{
	virtual ~IShader();
	virtual vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3f bar, TGAColor& color) = 0;

};

void triangle(mat<4,3,float> &clipc, IShader& shader, TGAImage& image, float* zbuffer);


#endif //_OUR_GL_H_