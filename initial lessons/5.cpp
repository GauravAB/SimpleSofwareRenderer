#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"


const int width = 800;
const int height = 800;
const int depth = 255;

std::shared_ptr<Model> model;
int* zbuffer = NULL;

vec3 light_dir(0, 0, -1);
vec3 camera(0, 0, 3);



vec3 m2v(mat<4, 1> m)
{
	return vec3(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

mat<4,1> v2m(vec3 v)
{
	mat<4,1> m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;

	return m;
}


mat<4,4> viewport(int x, int y, int w, int h)
{
	mat<4,4> m = mat<4,4>::identity();
	
	//translate
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	//scale
	m[0][0] = w * 0.5f;
	m[1][1] = h * 0.5f;
	m[2][2] = depth / 2.f;

	return m;
}

void triangle(vec3 t0, vec3 t1, vec3 t2, vec2 uv0, vec2 uv1, vec2 uv2, TGAImage& image, float intensity, int* zbuffer)
{
	if (t0.y == t1.y && t0.y == t2.y) return; //collinear  degenerate triangle
	
	//bubble sort triangle coordinates according to y for interpolation
	if (t0.y > t1.y) { std::swap(t0, t1); std::swap(uv0, uv1);}
	if (t0.y > t2.y) { std::swap(t0, t2); std::swap(uv0, uv2);}
	if (t1.y > t2.y) { std::swap(t1, t2); std::swap(uv1, uv2);}

	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; i++)
	{
		bool second_half = (i > (t1.y - t0.y)) || (t1.y == t0.y);
		int segment_height = second_half ? (t2.y - t1.y) : (t1.y - t0.y);

		//interpolater variable on the segment t2 - t0
		float alpha = (float)i / total_height;
		//interpolater for t0 to t1 first then after for t1 t0 t2
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

		vec3 A = t0 + vec3(t2 - t0) * alpha;
		vec3 B = second_half ? t1 + vec3(t2 - t1) * beta : t0 + vec3(t1 - t0) * beta;
		vec2 uvA = uv0 + (uv2 - uv0)*alpha;
		vec2 uvB = second_half ? (uv1 + (uv2 - uv1) * beta) : (uv0 + (uv1 - uv0) * beta);

		if (A.x > B.x)
		{
			std::swap(A, B);
			std::swap(uvA, uvB);
		}

		for (int j = A.x; j <= B.x; j++)
		{
			float phi = B.x == A.x ? 1.0 : (float)(j - A.x) / (float)(B.x - A.x);

			vec3 P = vec3(A) + vec3(B - A) * phi;
			vec2 uvP = uvA + (uvB - uvA) * phi;

			int idx = P.x + P.y * width;
			if (zbuffer[idx] < P.z)
			{
				zbuffer[idx] = P.z;
				TGAColor color = model->diffuse(uvP);
				image.set(P.x, P.y, 
					TGAColor(
						color.bgra[2]*intensity ,
						color.bgra[1]*intensity ,
						color.bgra[0]*intensity ));
			}
		}
	}
}


int main()
{
	model = std::make_shared<Model>("obj/african_head/african_head.obj");
	zbuffer = new int[width * height];

	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = std::numeric_limits<int>::min();
	}

	{ //Draw Model
		mat<4, 4> projection = mat<4,4>::identity();
		mat<4, 4> vp = viewport(width / 8, height / 8, (width * 3) / 4, (height * 3) / 4);
		//projection[3][2] = -1.f / camera.z;

		TGAImage image(width, height, TGAImage::RGB);

		for (int i = 0; i < model->nfaces(); i++)
		{
			vec3 screen_coords[3];
			vec3 world_coords[3];

			for (int j = 0; j < 3; j++)
			{
				world_coords[j] = model->vert(i, j);
				vec4 temp(world_coords[j].x, world_coords[j].y, world_coords[j].z, 1.);

				vec4 sc_unprojected = ( temp);
				//screen_coords[j] = vec3(sc_unprojected.x / sc_unprojected.w, sc_unprojected.y / sc_unprojected.w, sc_unprojected.z / sc_unprojected.w);
				screen_coords[j] = vec3(
					((sc_unprojected.x + 1.0)*(width/2.0f +0.5)) ,
					((sc_unprojected.y+1.0) * (height / 2.0f + 0.5)),
					((sc_unprojected.z+1.0) * (depth / 2.0f + 0.5)));

				//	screen_coords[j] = m2v(vp * temp);
			}

			vec3 n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
			n.normalize();

			float intensity = n * light_dir;
			if (intensity > 0.0f)
			{
				vec2 uv[3];
				for (int k = 0; k < 3; k++)
				{
					uv[k] = model->uv(i, k);
				}

				triangle(screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], image, intensity, zbuffer);
			}
		}

		image.write_tga_file("output_5.tga");
	}

	{//dump z-buffer 

	}

	delete[] zbuffer;
	
	return 0;
}










