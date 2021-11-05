
#include <iostream>
#include <vector>
#include <limits>
#include "model.h"
#include "geometry.h"
#include "../common/tgaimage.h"
#include "ourGL.h"


Model* model = NULL;
const int width  = 800;
const int height = 800;

vec3f light_dir(1.f, 1.f, 1.f);
vec3f eye(0.5, 0.5, 3);
vec3f center(0, 0, 0);
vec3f up(0, 1, 0);

//flat shading
//intensity calculated per normal ( per vertex ) and interpolated
struct GouraudShader : public IShader
{
	Model model_;

	GouraudShader(Model& m) :model_(m) {}

	//written by vertex shader and fragment shader
	vec3f varying_intensity;
	
	virtual vec4f vertex(int iface, int nthvert)
	{
		vec4f gl_vertex = embed<4>(model_.vert(iface, nthvert)); //read the vertex from .obj file

		gl_vertex = ViewPort * Projection * ModelView * gl_vertex; //transform it to screen coordinates
		varying_intensity[nthvert] = std::max(0.f, model_.normal(iface, nthvert) * light_dir); //get diffuse lighting intensity
		return gl_vertex;
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		float intensity = varying_intensity * bar;
		color = TGAColor(255, 255, 255, 255) * intensity;
		return false;
	}
};

struct GouraudShader_mod1 : public IShader
{
	Model model_;

	GouraudShader_mod1(Model& m) :model_(m) {}

	//written by vertex shader and fragment shader
	vec3f varying_intensity;

	virtual vec4f vertex(int iface, int nthvert)
	{
		vec4f gl_vertex = embed<4>(model_.vert(iface, nthvert)); //read the vertex from .obj file

		gl_vertex = ViewPort * Projection * ModelView * gl_vertex; //transform it to screen coordinates
		varying_intensity[nthvert] = std::max(0.f, model_.normal(iface, nthvert) * light_dir); //get diffuse lighting intensity
		return gl_vertex;
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		float intensity = varying_intensity * bar;
		
		if (intensity > 0.85)
		{
			intensity = 1.0;
		}
		else if (intensity > 0.60)
		{
			intensity = 0.8;
		}
		else if (intensity > 0.45)
		{
			intensity = 0.6;
		}
		else if (intensity > 0.30)
		{
			intensity = 0.45;
		}
		else if (intensity > 0.15)
		{
			intensity = 0.30;
		}
		else
		{
			intensity = 0.0;
		}

		color = TGAColor(255, 155,0)* intensity;
		return false;
	}
};

struct PerFragmentLight : public IShader
{
	//written by vertex shader and fragment shader

	Model* model_;

	mat<2, 3, float> varying_uv;
	mat<4, 4, float> uniform_M; //projection*modelview
	mat<4, 4, float> uniform_MIT; //(projection*modelview).invert_transpose()


	///////////////////////////////////////////////////
	PerFragmentLight(Model* m) :model_(m) {}

	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, model_->uv(iface, nthvert));
		vec4f gl_vertex = embed<4>(model_->vert(iface, nthvert)); //read the vertex from .obj file
		
		//vertex processing
		return  ViewPort * Projection * ModelView * gl_vertex; //transform it to screen coordinates
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		vec3f n = proj<3>(uniform_M * embed<4>(model_->normal(uv))).normalize();
		vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		float intensity = std::max(0.f, n * l);
		color = model_->diffuse(uv)*intensity;

		return false;
	}
};

//considering global coordinate normal map
struct PhongShader : public IShader
{
	//written by vertex shader and fragment shader

	Model* model_;

	mat<2, 3, float> varying_uv;
	mat<4, 4, float> uniform_M; //projection*modelview
	mat<4, 4, float> uniform_MIT; //(projection*modelview).invert_transpose()


	///////////////////////////////////////////////////
	PhongShader(Model* m) :model_(m) {}

	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, model_->uv(iface, nthvert));
		vec4f gl_vertex = embed<4>(model_->vert(iface, nthvert)); //read the vertex from .obj file

		//vertex processing
		return  ViewPort * Projection* ModelView* gl_vertex; //transform it to screen coordinates
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		vec3f n = proj<3>(uniform_M * embed<4>(model_->normal(uv))).normalize();
		vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		float intensity = std::max(0.f, n * l);
		vec3f r = (n * (intensity * 2.0f) - l).normalize();
		
		//
		float spec = pow(std::max(r.z, 0.0f), model_->specular(uv));

		float diff = intensity;
		TGAColor c = model_->diffuse(uv);

		color = c;

		for (int i = 0; i < 3; i++)
		{
			color[i] = std::min<float>(5 + c[i] * (diff + 0.6 * spec), 255);
		}

		return false;
	}
};

//interpolating normals
struct PhongShader2 : public IShader
{
	//written by vertex shader and fragment shader

	Model* model_;

	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_nrm;
	mat<4, 4, float> uniform_M; //projection*modelview
	mat<4, 4, float> uniform_MIT; //(projection*modelview).invert_transpose()


	///////////////////////////////////////////////////
	PhongShader2(Model* m) :model_(m) {}

	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, model_->uv(iface, nthvert));
		varying_nrm.set_col(nthvert, proj<3>(uniform_MIT * embed<4>(model_->normal(iface, nthvert), 0.f)));

		vec4f gl_vertex = embed<4>(model_->vert(iface, nthvert)); //read the vertex from .obj file

		//vertex processing
		return  ViewPort * Projection* ModelView* gl_vertex; //transform it to screen coordinates
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		vec3f bn = (varying_nrm * bar).normalize();
	
		vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		float intensity = std::max(0.f, bn * l);
		vec3f r = (bn * (intensity * 2.0f) - l).normalize();

		//
		float spec = pow(std::max(r.z, 0.0f), model_->specular(uv));

		float diff = intensity;
		TGAColor c = model_->diffuse(uv);

		color = c;

		for (int i = 0; i < 3; i++)
		{
			color[i] = std::min<float>(5 + c[i] * (diff + 0.6 * spec), 255);
		}

		return false;
	}
};

//using normal map
//by calculating the tangent space 



void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	bool steep = false;

	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	float derror2 = std::abs(dy) * 2;
	float error2 = 0;
	int y = y0;


	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}

		error2 += derror2;
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

struct PhongShader3 : public IShader
{
	//written by vertex shader and fragment shader

	Model* model_;
	TGAImage* image_;

	mat<2, 3, float> varying_uv; //triangle uv coordinates
	mat<3, 3, float> varying_nrm; //triangle normal coordinates
	mat<4, 3, float> varying_tri; //triangle coordinates (clip coordinates)

	mat<3, 3, float> ndc_tri; //triangle in normalized deviced coordinates

	///////////////////////////////////////////////////
	PhongShader3(Model* m ,TGAImage* im) :model_(m), image_(im) {}

	virtual vec4f vertex(int iface, int nthvert)
	{
		varying_uv.set_col(nthvert, model_->uv(iface, nthvert));
		varying_nrm.set_col(nthvert, proj<3>((Projection*ModelView).invert_transpose() * embed<4>(model_->normal(iface, nthvert), 0.f)));

		vec4f gl_vertex = Projection*ModelView * embed<4>(model_->vert(iface, nthvert)); //read the vertex from .obj file
		
		varying_tri.set_col(nthvert, gl_vertex);
		ndc_tri.set_col(nthvert, proj<3>(gl_vertex / gl_vertex[3]));
		
		return gl_vertex;
	}

	virtual bool fragment(vec3f bar, TGAColor& color)
	{
		vec2f uv = varying_uv * bar;
		vec3f bn = (varying_nrm * bar).normalize();

		mat<3, 3, float> A;
		A[0] = ndc_tri.col(1) - ndc_tri.col(0);
		A[1] = ndc_tri.col(2) - ndc_tri.col(0);
		A[2] = bn;

		mat<3, 3, float> AI = A.invert();

		vec3f i = AI * vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
		vec3f j = AI * vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
		
		

		//darboux basis matrix
		mat<3, 3, float> B;
		B.set_col(0, i.normalize());
		B.set_col(1, j.normalize());
		B.set_col(2, bn);


		vec3f pn = model_->normal(uv);
		vec3f n = ( B * pn).normalize();
		float diff = std::max(0.f, n * light_dir);
		vec3f r = (n * (diff * 2.0f) - light_dir).normalize();

		//
		float spec = pow(std::max(r.z, 0.0f), model_->specular(uv));

		TGAColor c = model_->diffuse(uv);
		for (int i = 0; i < 3; i++)
		{
			c[i] = std::min<float>(5 + c[i] * (diff + 0.6 * spec),255);
		}
		color = c;

		return false;
	}
};


int main()
{
	Model* model = new Model("../resources/african_head/african_head.obj");
	assert(model != 0);

	

	TGAImage frame(width, height, TGAImage::RGB);
	float* zbuffer = new float[width * height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();

	
	//GouraudShader shader(*model);
	//GouraudShader_mod1 shader(*model);
	PhongShader3 shader(model,&frame);

	//for every triangle
	for (int i = 0; i < model->nfaces(); i++)
	{
		//for every vertex in triangle
		for (int j = 0; j < 3; j++)
		{
			shader.vertex(i, j);
		}
		
		//rasterize and call fragment shader for each pixel
		triangle(shader.varying_tri, shader, frame, zbuffer);
	}
	
	std::cout << "Writing image to file\n";
	frame.write_tga_file("framebuffer.tga");
	
	delete model;
	delete[]zbuffer;
	return 0;
}










