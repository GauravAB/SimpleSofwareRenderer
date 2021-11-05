#include <cmath>
#include <limits>
#include <cstdlib>
#include "our_gl.h"

Matrix ModelView;
Matrix Projection;
Matrix Viewport;


IShader ::~IShader() {}

void viewport(int x, int y, int w, int h)
{
	Viewport = Matrix::identity();

	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = 1.f;
	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = 0;
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
	vec3f S[2];

	for (int i = 2; i--;)
	{
		S[i][2] = A[i] - P[i];
		S[i][0] = C[i] - A[i];
		S[i][1] = B[i] - A[i];
	}

	vec3f c = cross(S[0], S[1]);
	if (std::abs(c[2]) > 1e-2)
	{
		return vec3f(1.f - (c.x + c.y) / c.z, c.y / c.z, c.x / c.z);
	}

	return vec3f(-1, 1, 1);
}


void triangle(mat<4, 3, float>& clipc, IShader& shader, TGAImage& image, float* zbuffer)
{
	//screenspace
	
	mat<3, 4, float> pts = (Viewport * clipc).transpose();
	//std::cout << pts[0] << std::endl;
	//std::cout << pts[1] << std::endl;
//	std::cout << pts[2] << std::endl;

	//projected onto 3D plane (projx,projy)
	mat<3, 2, float> pts2;
	for (int i = 0; i < 3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);
	//std::cout << pts2[0] << std::endl;
	//std::cout << pts2[1] << std::endl;
	//std::cout << pts2[2] << std::endl;

	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));

		}
	}

	
	vec2i P;
	TGAColor color;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
			//std::cout << " screen: " << bc_screen.x << " " << bc_screen.y << " "<< bc_screen.z  << " = " << bc_screen.x + bc_screen.y + bc_screen.z	 << std::endl;
			vec3f bc_clip = vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
		//	std::cout << " clip: " << bc_clip.x << " " << bc_clip.y << " " << bc_clip.z << std::endl;
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
			
			float frag_depth = clipc[2] * bc_clip;
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer[P.x + P.y * image.get_width()] > frag_depth) continue;
			
			bool discard = shader.fragment(vec3f(P.x, P.y, frag_depth), bc_clip, color);
			if (!discard)
			{
				zbuffer[P.x + P.y * image.get_width()] = frag_depth;
				image.set(P.x, P.y, color);
			}
		}
	}
}





