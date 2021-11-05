#include <vector>
#include <cstdlib>
#include <limits>
#include <iostream>
#include "../common/tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

#define M_PI       3.14159265358979323846   // pi
Model* model = NULL;
float* shadowbuffer = NULL;
const int width = 800;
const int height = 800;


vec3f eye(1.2, -0.8, 3);
vec3f center(0, 0, 0);
vec3f up(0, 1, 0);




struct ZShader : public IShader
{
	mat<4, 3, float> varying_tri;
	
	virtual vec4f vertex(int iface, int nthvert)
	{
		vec4f gl_vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
		varying_tri.set_col(nthvert, gl_vertex);
	//	std::cout << varying_tri.col(nthvert) << std::endl;
		return gl_vertex;
	}

	virtual bool fragment(vec3f gl_FragCoord, vec3f bar, TGAColor& color)
	{
		color = TGAColor(0,0,0);
		return false;
	}
};

float max_elevation_angle(float* zbuffer, vec2f p, vec2f dir)
{
	float maxangle = 0;
	for (float t = 0.; t < 1000.; t += 1.)
	{
		vec2f cur = p + dir * t;
		if (cur.x >= width || cur.y >= height || cur.x < 0 || cur.y < 0) return maxangle;

		float distance = (p - cur).norm();
		if (distance < 1.f) continue;
		float elevation = zbuffer[int(cur.x) + int(cur.y) * width] - zbuffer[int(p.x) + int(p.y) * width];
		maxangle = std::max(maxangle, atanf(elevation / distance));
	}

	return maxangle;
}


int main()
{
	float* zbuffer = new float[width * height];
	for (int i = width * height; i--;) { zbuffer[i] = -std::numeric_limits<float>::max(); }
	model = new Model("../resources/diablo3_pose/diablo3_pose.obj");

	TGAImage frame(width, height, TGAImage::RGB);
	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());

	ZShader zshader;
	for (int i = 0; i < model->nfaces(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			zshader.vertex(i, j);
		}

		triangle(zshader.varying_tri, zshader, frame, zbuffer);
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (zbuffer[x + y * width] < -1e5) continue;
			float total = 0;
			for (float a = 0; a < M_PI * 2 - 1e-4; a += M_PI / 4)
			{
				total += M_PI / 2 - max_elevation_angle(zbuffer, vec2f(x, y), vec2f(cos(a), sin(a)));
			}

			total /= (M_PI / 2) * 8;
			total = pow(total, 100.f);
			frame.set(x, y, TGAColor(total * 255, total * 255, total * 255));
		}
	}

	frame.write_tga_file("framebuffer2.tga");

	delete []zbuffer;
	delete[]shadowbuffer;
	delete model;

	return 0;
}