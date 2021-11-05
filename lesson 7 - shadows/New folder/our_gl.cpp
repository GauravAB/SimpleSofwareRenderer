#include <cmath>
#include <limits>
#include <cstdlib>
#include "our_gl.h"

Matrix ModelView;
Matrix Projection;
Matrix Viewport;


IShader::~IShader() {}

void viewport(int x, int y, int w, int h)
{
	Viewport = Matrix::identity();

	Viewport[0][3] = x + w/2.f;
	Viewport[1][3] = y + h/2.f;
	Viewport[2][3] = depth/2.f;

	Viewport[0][0] = w/2.f;
	Viewport[1][1] = h/2.f;
	Viewport[2][2] = depth/2.f;
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
	ModelView = Matrix::identity();

	for (int i = 0; i < 3; i++)
	{
		ModelView[0][i] = x[i];
		ModelView[1][i] = y[i];
		ModelView[2][i] = z[i];
		ModelView[i][3] = -center[i];
	}
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
	if (std::abs(u[2] > 1e-2))
	{
		return vec3f( 1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}
	return vec3f(-1, 1, 1);
}


void triangle(vec4f* pts, IShader& shader, TGAImage& image, float* zbuffer)
{
	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(image.get_width()-1, image.get_height()-1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
			bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
			
		}
	}

	vec2i P;

	TGAColor color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			vec3f c = barycentric(
				proj<2>(pts[0] / pts[0][3]),
				proj<2>(pts[1] / pts[1][3]),
				proj<2>(pts[2] / pts[2][3]),
				proj<2>(P));

			float z = pts[0][2] * c.x + pts[1][2] * c.y + pts[2][2] * c.z;
			float w = pts[0][3] * c.x + pts[1][3] * c.y + pts[2][3] * c.z;
			int frag_depth = z / w;

			if (c.x < 0 || c.y < 0 || c.z < 0 || zbuffer[P.x + P.y * image.get_width()] > frag_depth) continue;
			bool discard = shader.fragment(c, color);
			if (!discard)
			{
				zbuffer[P.x + P.y * image.get_width()] = frag_depth;
				image.set(P.x, P.y, color);
			}
		}
	}
}







