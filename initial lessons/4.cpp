#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 800;
const int height = 800;


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
	
	for (int x = x0; x <= x1; x++)
	{
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1.0 - t) + y1 * (t);
		if (steep)
		{
			image.set(y, x, color);

		}
		else
		{
			image.set(x, y, color);
		}
	}
}

vec3 barycentric(vec3 A, vec3 B, vec3 C, vec3 P)
{
	vec3 s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}

	vec3 u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2)
		return vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

	return vec3(-1, 1, 1);

}


void triangle(vec3* pts, float* zbuffer, TGAImage& image, vec2* uv, std::shared_ptr<Model> model ,TGAColor color)
{
	vec2 bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	vec2 clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j],std::max(bboxmax[j], pts[i][j]));

		}
	}

	vec3 P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			vec3 bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

			P.z = 0;
			vec2 st;
			
			
			//Interpolate the current pixel uv and z values
			for (int i = 0; i < 3; i++)
			{
				st.x += uv[i][0] * bc_screen[i];
				st.y += uv[i][1] * bc_screen[i];
				P.z += pts[i][2] * bc_screen[i];
			}
			
			

			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				
				TGAColor texture_color = model->diffuse(st);
				zbuffer[int(P.x + P.y * width)] = P.z;
				image.set(P.x, P.y,
					TGAColor(
						 texture_color.bgra[2]*(color.bgra[2])/255,
						 texture_color.bgra[1]*(color.bgra[1])/255,
						 texture_color.bgra[0]*(color.bgra[0])/255,
						255));
			}
		}
	}
}

void triangle_ls(vec3 t0, vec3 t1, vec3 t2, vec2 uv0, vec2 uv1, vec2 uv2, TGAImage& image, float intensity, int* zbuffer,std::shared_ptr<Model> model)
{
	if (t0.y == t1.y && t0.y == t2.y) return; //collinear  degenerate triangle

	//bubble sort triangle coordinates according to y for interpolation
	if (t0.y > t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
	if (t0.y > t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
	if (t1.y > t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }

	int total_height = (int)t2.y - (int)t0.y;
	for (int i = 0; i <= total_height; i++)
	{
		bool second_half = (i > (t1.y - t0.y)) ||  (t1.y == t0.y);
		int segment_height = second_half ? (t2.y - t1.y) : (t1.y - t0.y);

		//interpolater variable on the segment t2 - t0
		float alpha = (float)i / total_height;
		//interpolater for t0 to t1 first then after for t1 t0 t2
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

		vec3 A = t0 + (t2 - t0) * alpha;
		vec3 B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		vec2 uvA = uv0 + (uv2 - uv0) * alpha;
		vec2 uvB = second_half ? (uv1 + (uv2 - uv1) * beta) : (uv0 + (uv1 - uv0) * beta);

		if (A.x > B.x)
		{
			std::swap(A, B);
			std::swap(uvA, uvB);
		}

		for (int j = A.x; j <= B.x; j++)
		{
			float phi = (B.x == A.x) ? 1.0f : (float)(j - A.x) / (float)(B.x - A.x);

			vec3 P = A + ((B - A) * phi);
			vec2 uvP = uvA + ((uvB - uvA) * phi);

			int idx = P.x + P.y * width;
			
			if (zbuffer[idx] < P.z)
			{
				zbuffer[idx] = P.z;
				TGAColor color = model->diffuse(uvP);
				image.set(P.x, P.y,
					TGAColor(
						color.bgra[2]*intensity,
						color.bgra[1]*intensity,
						color.bgra[0]*intensity));
			}
		}
	}
}



vec3 world2screen(vec3 v)
{	
	return vec3(
		(int)((v.x + 1.0) * (width / 2.0f) + 0.5f), 
		(int)((v.y + 1.0f) * (height / 2.0f) + 0.5f),
		(int)((v.z + 1.0) * (255 /2.0f)+0.5) );
}


/*void single_triangle_test(TGAImage& image)
{

	float* zbuffer = new float[width * height];
	for (int i = width * height; i--;)
		zbuffer[i] = -std::numeric_limits<float>::max();

	vec3 pts[3] = { vec3(10,10,0),vec3(100,30,0),vec3(190,160,0)};
	triangle(pts,zbuffer, image, red);

}
*/


int main()
{
	std::shared_ptr<Model> model = std::make_shared<Model>("obj/african_head/african_head.obj");

//	float* zbuffer = new float[width * height];
	int* zbuffer = new int[width * height];

	//for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = std::numeric_limits<int>::min();
	}

	TGAImage image(width, height, TGAImage::RGB);

	vec3 light_dir(0, 0, -1);

	
	for (int i = 0; i < model->nfaces(); i++)
	{
		vec3 pts[3];
		vec3 worldcoords[3];
	

		for (int j = 0; j < 3; j++)
		{
			worldcoords[j] = model->vert(i, j);
			pts[j] = world2screen(worldcoords[j]);
		}


		vec3 n = cross((worldcoords[2] - worldcoords[0]), (worldcoords[1] - worldcoords[0]));
		n.normalize();
		float intensity = n * light_dir;

		if (intensity > 0.0f)
		{
			vec2 uv[3];
			for (int k = 0; k < 3; k++)
			{
				uv[k] = model->uv(i, k);
			}

			triangle_ls(pts[0], pts[1], pts[2], uv[0], uv[1], uv[2],image,intensity,zbuffer,model);
	
		}
	}
	
	//single_triangle_test(image);
	
	delete [] zbuffer;
	
	image.write_tga_file("output_4_textured_linesweeper_zf.tga");

	return 0;
}















