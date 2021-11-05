#include <cmath>
#include <limits>
#include <cstdlib>
#include "ourGL.h"

Matrix ModelView;
Matrix ViewPort;
Matrix Projection;


IShader::~IShader() {}

void viewport(int x, int y, int w, int h)
{
	ViewPort = Matrix::identity();

	ViewPort[0][3] = x + w / 2.f;
	ViewPort[1][3] = y + h / 2.f;
	ViewPort[2][3] = 0;

	ViewPort[0][0] = w / 2.f;
	ViewPort[1][1] = h / 2.f;
	ViewPort[2][2] = 1.f;
}


void projection(float coeff)
{
	Projection = Matrix::identity();
	Projection[3][2] = coeff;

}

void lookat(vec3f eye, vec3f center, vec3f up)
{
	vec3f z = (eye - center).normalize();
	vec3f x = cross(up, z).normalize();
	vec3f y = cross(z, x).normalize();
	Matrix  Minv = Matrix::identity();
	Matrix Tr = Matrix::identity();

	for (int i = 0; i < 3; i++)
	{
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		Tr[i][3] = -center[i];

	}

	ModelView = Minv * Tr;
}

vec3f barycentric(vec2f A, vec2f B, vec2f C, vec2f P)
{
	vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		
		s[i][2] = A[i] - P[i];
	}

	vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2)
		return vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

	return vec3f(-1, 1, 1);
}

void triangle(mat<4,3,float>& clipc, IShader& shader, TGAImage& image, float* zbuffer)
{
	//map to screenspace clip coordinates
	mat<3, 4, float> pts = (ViewPort * clipc).transpose(); //transpose for ease of access to each of points
	
	//perspective division 
	//normalized device coordinates just x,y in transposed form for easy access
	mat<3, 2, float> pts2;
	for (int i = 0; i < 3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);

	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f,std::min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j],pts2[i][j]));
		}
	}

	vec2i P;
	TGAColor color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			//perspective div
			vec3f bc_screen = barycentric(pts2[0],pts2[1],pts2[2],P);
			vec3f bc_clip = vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

			float frag_depth = clipc[2] * bc_clip;

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer[P.x+ P.y*image.get_width()] > frag_depth) continue;
			bool discard = shader.fragment(bc_clip, color);
			if (!discard)
			{
				zbuffer[P.x+P.y * image.get_width()] = frag_depth;
				image.set(P.x, P.y, color);
			}
		}
	}

}








