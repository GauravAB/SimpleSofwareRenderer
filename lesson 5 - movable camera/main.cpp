#include "geometry.h"
#include "model.h"
#include <iostream>

Model* model = NULL;
int* zbuffer = NULL;

const int height = 800;
const int width = 800;
const int depth = 255;

 vec3f light_dir = vec3f(1, -1, 1).normalize();
 vec3f eye(1, 1, 3);
 vec3f center(0, 0, 0);



//line sweeper
void triangle(vec3i t0, vec3i t1, vec3i t2, vec2i uv0, vec2i uv1, vec2i uv2,float* intensity, TGAImage& image, int* zbuffer)
{
	float ity0 = intensity[0];
	float ity1 = intensity[1];
	float ity2 = intensity[2];


	if (t0.y == t1.y && t0.y == t2.y) return;
	if (t0.y > t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); std::swap(ity0, ity1); }
	if (t0.y > t2.y) { std::swap(t0, t2); std::swap(uv0, uv2);std::swap(ity0, ity2); }
	if (t1.y > t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); std::swap(ity1, ity2);}

	int total_height = t2.y - t0.y;

	for (int i = 0; i < total_height; i++)
	{
		bool second_half = i > (t1.y - t0.y) || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

		float alpha = (float)i /total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

		vec3i A = t0 + vec3f(t2 - t0) * alpha;
		vec3i B = second_half ? t1 + vec3f(t2 - t1) * beta : t0 + vec3f(t1 - t0) * beta;
		vec2i uvA = uv0 + (uv2 - uv0) * alpha;
		vec2i uvB = second_half ? uv1 + (uv2 - uv1) * beta : uv0 + (uv1 - uv0) * beta;
		float ityA = ity0 + (ity2-ity0) * alpha;
		float ityB = second_half ? ity1 + (ity2 - ity1) * beta : ity0 + (ity1 - ity0) * beta;


		if (A.x > B.x)
		{
			std::swap(A, B);
			std::swap(uvA, uvB);
			std::swap(ityA, ityB);
		}

		for (int j = A.x; j <= B.x; j++)
		{
			float phi = (B.x == A.x) ? 1.f : (float)(j - A.x) / (float)(B.x - A.x);
			
			vec3i P = vec3f(A) + vec3f(B - A) * phi;
			vec2i UV = uvA + (uvB - uvA) * phi;
			float ityP = ityA + (ityB - ityA) * phi;

			int idx = P.x + P.y * width;
			if (P.x >= width || P.y >= height || P.x < 0 || P.y < 0) continue;

			if (zbuffer[idx] < P.z)
			{
				TGAColor color = model->diffuse(UV);
				zbuffer[idx] = P.z;
				std::cout << "writing: " << P.x << " " << P.y << std::endl;
				image.set(P.x, P.y, 
					TGAColor(
						color.bgra[2],
						color.bgra[1],
						color.bgra[0])* ityP);
			}
		}
	}
}

Matrix viewport(int x0, int y0, int width, int height)
{
	Matrix M = Matrix::identity(4);
	M[0][3] = x0+width/2.f;
	M[1][3] = y0+height/2.f;
	M[2][3] = depth / 2.f;

	M[0][0] = width / 2.f;
	M[1][1] = height / 2.f;
	M[2][2] = depth / 2.f;

	return M;
}

Matrix lookat(vec3f eye, vec3f center, vec3f up)
{
	vec3f z = (eye - center).normalize();
	vec3f x = (up ^ z).normalize();
	vec3f y = (z ^ x).normalize();
	Matrix res = Matrix::identity(4);

	for (int i = 0; i < 3; i++)
	{
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
		res[i][3] = -center[i];

	}

	return res;
}


int main()
{
	model = new Model("../resources/african_head/african_head.obj");

	zbuffer = new int[width * height];

	for (int i = 0; i < width * height; i++)
	{
		zbuffer[i] = std::numeric_limits<int>::min();
	}

	{//rasterize triangle

		Matrix ModelView = lookat(eye, center, vec3f(0, 1, 0));
		Matrix Projection = Matrix::identity(4);
		Matrix ViewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		Projection[3][2] = -1.f / (eye - center).norm();

		Matrix z = (ViewPort * Projection * ModelView);


		TGAImage image(width, height, TGAImage::RGB);

		for (int i = 0; i < model->nfaces(); i++)
		{
			vec3f world_coords[3];
			vec3i screen_coords[3];
			std::vector<int> face = model->face(i);
			float intensity[3];

			for (int j = 0; j < 3; j++)
			{
				vec3f v = model->vert(face[j]);
				world_coords[j] = v;
				screen_coords[j] = vec3f(z * Matrix(v));
				intensity[j] = model->norm(i, j) * light_dir;

				std::cout << world_coords[j].x << " " << world_coords[j].y << " " << world_coords[j].z << "\n";
				std::cout << screen_coords[j].x << " " << screen_coords[j].y << " " << screen_coords[j].z << "\n";
			}

				vec2i uv[3];
				for (int k = 0; k < 3; k++)
				{
					uv[k] = model->uv(i, k);
				}

				triangle(screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], intensity, image, zbuffer);

		}

		image.write_tga_file("output.tga");
	}



	delete model;
	delete[] zbuffer;

	return 0;
}