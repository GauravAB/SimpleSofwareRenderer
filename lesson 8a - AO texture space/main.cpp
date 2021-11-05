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

TGAImage total(1024, 1024, TGAImage::GRAYSCALE);
TGAImage occl(1024, 1024, TGAImage::GRAYSCALE);


struct AOShader : public IShader
{
	mat<2, 3, float> varying_uv;
	mat<4, 3, float> varying_tri;
	TGAImage aoimage;

	virtual vec4f vertex(int iface, int nthvert)
	{
		vec4f gl_vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
		varying_tri.set_col(nthvert, gl_vertex);
		varying_uv.set_col(nthvert,model->uv(iface, nthvert));
		return gl_vertex;
	}

	virtual bool fragment(vec3f gl_FragCoord, vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		int t = aoimage.get(uv.x * 1024, uv.y * 1024)[0];
		
		color = TGAColor(t, t, t);
		return false;
	}
};

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
		color = TGAColor(255, 255, 255) *((gl_FragCoord.z + 1.f) / 2.f);
		return false;
	}
};

struct Shader : public IShader
{
	mat<2, 3, float> varying_uv;
	mat<4, 3, float> varying_tri;

	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		vec4f gl_vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
		varying_tri.set_col(nthvert, gl_vertex);
		return gl_vertex;
	}

	virtual bool fragment(vec3f gl_FragCoord, vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		if (std::abs(shadowbuffer[int(gl_FragCoord.x + gl_FragCoord.y * width)] - gl_FragCoord.z < 1e-2))
		{
			occl.set(uv.x * 1024, uv.y * 1024, TGAColor(255));
		}
		color = TGAColor(255, 0, 0);
		return false;
	}
};

vec3f rand_point_on_unit_sphere()
{
	//random variables on (0,1)
	float u = (float)rand() / (float)RAND_MAX;
	float v = (float)rand() / (float)RAND_MAX;
	float theta = 2.f * M_PI * u;
	float phi = acosf(2.f * v - 1.f);
	
	return vec3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

int main()
{
	float* zbuffer = new float[width * height];
	shadowbuffer = new float[width * height];
	model = new Model("../resources/diablo3_pose/diablo3_pose.obj");

	//TGAImage frame(width, height, TGAImage::RGB);

	
	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

		const int nrenders = 100;
		for (int iter = 1; iter <= nrenders; iter++)
		{
	 	std::cout << iter << " from " << nrenders << std::endl;
		for (int i = 0; i < 3; i++) up[i] = (float)rand() / (float)RAND_MAX;
		eye = rand_point_on_unit_sphere();
		eye.y = std::abs(eye.y);
		std::cout << "v " << eye << std::endl;

		for (int i = width * height; i--; shadowbuffer[i] = zbuffer[i] = -std::numeric_limits<float>::max());

		TGAImage frame(width, height, TGAImage::RGB);
		lookat(eye, center, up);
		viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		projection(0);

		ZShader zshader;
		for (int i = 0; i < model->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				zshader.vertex(i, j);
			}

			triangle(zshader.varying_tri, zshader, frame, shadowbuffer);
		}

		
		//frame.flip_vertically();
		frame.write_tga_file("zbuffer.tga");

		
		Shader shader;
		occl.clear();
		for (int i = 0; i < model->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				shader.vertex(i, j);
			}

			triangle(shader.varying_tri, shader, frame, zbuffer);
		}

		//occlusion gaussian blur
		for (int i = 0; i < 1024; i++)
		{
			for (int j = 0; j < 1024; j++)
			{
				float tmp = total.get(i, j)[0];
				total.set(i, j, TGAColor((tmp * (iter - 1) + occl.get(i, j)[0]) / (float)iter + 0.5f));
			}
		}
	}
	
	total.write_tga_file("occlusion.tga");
	occl.write_tga_file("occl.tga");
	

	//render model with AO image intensity
	TGAImage newframe(width, height, TGAImage::RGB);
	for (int i = width * height; i--;) { zbuffer[i] = -std::numeric_limits<float>::max(); }
	eye = vec3f(1, 1, 4);
	center = vec3f(0, 0, 0);
	up = vec3f(0, 1, 0);
	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f/(eye - center).norm());
	AOShader aoshader;
	if (aoshader.aoimage.read_tga_file("occlusion.tga") == false)
	{
		std::cout << "error " << std::endl;
	}
	aoshader.aoimage.flip_vertically();

	for (int i = 0; i < model->nfaces(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			aoshader.vertex(i, j);
		}

		triangle(aoshader.varying_tri, aoshader, newframe, zbuffer);
	}

	newframe.write_tga_file("Framebuffer2.tga");


	delete []zbuffer;
	delete[]shadowbuffer;
	delete model;

	return 0;
}