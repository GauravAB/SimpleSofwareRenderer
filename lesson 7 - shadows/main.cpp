#include <vector>
#include <limits>
#include <iostream>

#include "../common/tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"



const int width = 800;
const int height = 800;
float* shadowbuffer = NULL;

vec3f light_dir(1, 1, 0);
vec3f eye(1, 1, 3);
vec3f center(0, 0, 0);
vec3f up(0, 1, 0);

Model* pmodel = NULL;

struct Depthshader : public IShader
{
	mat<3, 3, float> varying_tri;

	Depthshader() : varying_tri() {}

	virtual vec4f vertex(int iface, int nthvert)
	{
		vec4f gl_vertex = embed<4>(pmodel->vert(iface, nthvert));
		gl_vertex = Viewport * Projection * ModelView * gl_vertex;	
		varying_tri.set_col(nthvert, proj<3>(gl_vertex / gl_vertex[3]));

		return gl_vertex;
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		vec3f p = varying_tri * bar;
		
		color = TGAColor(255, 255, 255) * (p.z / depth);
		return false;
	}
};

struct Shader : public IShader
{
	mat<4, 4, float> uniform_M;
	mat<4, 4, float> uniform_MIT;
	mat<4, 4, float> uniform_Mshadow;
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_tri;

	Shader(Matrix& M, Matrix MIT, Matrix MS) : uniform_M(M), uniform_MIT(MIT), uniform_Mshadow(MS), varying_uv(), varying_tri() {}
	
	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, pmodel->uv(iface, nthvert));
		vec4f gl_vertex = (Viewport*Projection*ModelView)*embed<4>(pmodel->vert(iface, nthvert));
		varying_tri.set_col(nthvert, proj<3>(gl_vertex / gl_vertex[3]));
		return gl_vertex;
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		
		vec4f sb_p = uniform_Mshadow * embed<4>(varying_tri * bar); //corresponding point in shadow buffer
		sb_p = sb_p / sb_p[3];

		int idx = int(sb_p[0]) + int(sb_p[1]) * width;
		float shadow = .3 + .7 * (shadowbuffer[idx] < sb_p[2] + 43.34);//magic coeff to avoid z - fighting
	
		vec2f uv = varying_uv * bar;
		vec3f n = proj<3>(uniform_MIT * embed<4>(pmodel->normal(uv))).normalize();
		vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		vec3f r = (n * (n * l * 2.f) - l).normalize();
		float spec = pow(std::max(r.z, 0.0f), pmodel->specular(uv));
		float diff = std::max(0.0f, n * l);
		
		TGAColor c = pmodel->diffuse(uv);
		
		for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * shadow * (1.2 * diff + 0.6 * spec), 255);
	
		return false;
	}
};


int main()
{
	//geometry_test();
	
	
	float* zbuffer = new float[width * height];
	shadowbuffer = new float[width * height];

	for (int i = width * height; i--;)
	{
		zbuffer[i] =  -std::numeric_limits<float>::max();
		shadowbuffer[i] = -std::numeric_limits<float>::max();
	}

	pmodel = new Model("../resources/diablo3_pose/diablo3_pose.obj");
	assert(pmodel);

	light_dir.normalize();
	
	
	//rendering shadow buffer
	{
		TGAImage depth(width, height, TGAImage::RGB);
		lookat(light_dir, center, up);
		viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		projection(0);
		Depthshader depthshader;
		vec4f screen_coords[3];

		for (int i = 0; i < pmodel->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				screen_coords[j] = depthshader.vertex(i, j);
			}

			triangle(screen_coords, depthshader, depth, shadowbuffer);
		}

	//	depth.flip_vertically();
		depth.write_tga_file("depth.tga");
	}

	Matrix M = Viewport * Projection * ModelView;
	

	{//rendering the frame buffer
		TGAImage frame(width, height, TGAImage::RGB);
		lookat(eye, center, up);
		viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		projection(-1.f / (eye - center).norm());
		
		Shader shader(ModelView, (Projection * ModelView).invert_transpose(), M * (Viewport * Projection * ModelView).invert());
		vec4f screen_coords[3];

		for (int i = 0; i < pmodel->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				screen_coords[j] = shader.vertex(i, j);
			}

			triangle(screen_coords, shader, frame, zbuffer);
		}

		frame.write_tga_file("framebuffer2.tga");
	}

	delete []zbuffer;
	delete []shadowbuffer;
	delete pmodel;
	return 0;
}