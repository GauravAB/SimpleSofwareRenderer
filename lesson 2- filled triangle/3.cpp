#include "geometry.h"
#include "model.h"
#include "../impl/tgaimage.h"
#include <memory>
#include <algorithm>


const int width =  800;
const int height = 800;

TGAColor white(255, 255, 255, 255);
TGAColor red(255, 0, 0, 255);
TGAColor green(0, 255, 0, 255);

//screen space line interpolater
void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color)
{
	bool steep = false;

	//find steep
	if(std::abs(p0.x - p1.x) < std::abs(p0.y-p1.y))
	{
		//run pixels along y and interpolate x values
		//if this is not done then the line will have holes since dx < dy
		//run loops for longest distance (here dy(y2-y1)) and interpolate between for x
		std::swap(p0.x,p0.y);
		std::swap(p1.x,p1.y);
		steep = true;
	}

	//we always move left to right
	if(p0.x > p1.x)
	{
		std::swap(p0,p1);
	}

	//run loop for most pixels according to steepness 
	for(int x = p0.x ; x <= p1.x ; x++)
	{
		float t = x -p0.x /(float)( p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;

		if(steep)
		{
			image.set(y,x,color);
		}
		else
		{
			image.set(x,y,color);
		}
	}
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	Vec2i p0 = {x0,y0};
	Vec2i p1 = {x1,y1};
	bool steep = false;

	//find steep
	if(std::abs(p0.x - p1.x) < std::abs(p0.y-p1.y))
	{
		//run pixels along y and interpolate x values
		//if this is not done then the line will have holes since dx < dy
		//run loops for longest distance (here dy(y2-y1)) and interpolate between for x
		std::swap(p0.x,p0.y);
		std::swap(p1.x,p1.y);
		steep = true;
	}

	//we always move left to right
	if(p0.x > p1.x)
	{
		std::swap(p0,p1);
	}

	//run loop for most pixels according to steepness 
	for(int x = p0.x ; x <= p1.x ; x++)
	{
		float t = x -p0.x /(float)( p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;

		if(steep)
		{
			image.set(y,x,color);
		}
		else
		{
			image.set(x,y,color);
		}
	}
}


//---------------------------LINE SWEEPER--------------------------------------
//line sweeper my try
/*void triangle1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	int x0 = t0.x;
	int y0 = t0.y;
	int x1 = t1.x;
	int y1 = t1.y;
	int x2 = t2.x;
	int y2 = t2.y;
	
	Vec2i lower;
	Vec2i middle;
	Vec2i top;

	if ((y0 < y1) && (y1 < y2 ))
	{
		lower.x = x0;
		lower.y = y0;

		middle.x = x1;
		middle.y = y1;

		top.x = x2;
		top.y = y2;
	}
	else if ((y0 < y2) && (y2 < y1))
	{
		lower.x = x0;
		lower.y = y0;

		middle.x = x2;
		middle.y = y2;

		top.x = x1;
		top.y = y1;
	}
	else if ((y1 < y0) && (y0 < y2))
	{
		lower.x = x1;
		lower.y = y1;

		middle.x = x0;
		middle.y = y0;

		top.x = x2;
		top.y = y2;
	}
	else if ((y1 < y2) && (y2 < y0))
	{
		lower.x = x1;
		lower.y = y1;

		middle.x = x2;
		middle.y = y2;

		top.x = x0;
		top.y = y0;
	}
	else if ((y2 < y0) && (y0 < y1))
	{
		lower.x = x2;
		lower.y = y2;

		middle.x = x0;
		middle.y = y0;

		top.x = x1;
		top.y = y1;
	}
	else if ((y2 < y1) && (y1 < y0))
	{
		lower.x = x2;
		lower.y = y2;

		middle.x = x1;
		middle.y = y1;

		top.x = x0;
		top.y = y0;
	}

	for (float i = 0; i <= 1.0; i += 0.001)
	{
		int xl = lower.x + (middle.x-lower.x) * i;
		int yl = lower.y + (middle.y-lower.y) * i;

		int xr = lower.x + (top.x-lower.x) * i;
		int yr = lower.y + (top.y-lower.y) * i;

		line(xl, yl, xr, yr, image, red);
	}


	line(t0, t1, image, color);
	line(t0, t2, image, color);
	line(t1, t2, image, color);
}
//recommended un-optimized readable code
void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	//bubble sort
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	
	int total_height = t2.y - t0.y; //longest segment length

	//traveling each pixel on the line of other segment from t0 to t1
	for (int y = t0.y; y <= t1.y; y++)
	{
		int segment_height = t1.y - t0.y + 1;
		
		//As we cover pixels from t0 to t1 , we find explicit parameters <-[0,1] on line segs t0t1 t0t2
		//explicit parameter of interpolation along to lines
		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t0.y) / segment_height;

	//find 2D point on t0 - t2 line
		Vec2i A = t0 + (t2 - t0) * alpha;
	//find 2D point on t1 - t0 line 
		Vec2i B = t0 + (t1 - t0) * beta;

	//we interpolate from left to right
		if (A.x > B.x) std::swap(A, B);

		for (int j = A.x; j <= B.x; j++)
		{
			image.set(j, y, color);
		}
	}

//when done filling triangle from t0t1 (half ) , we move from t1 - t2
	for (int y = t1.y; y <= t2.y; y++)
	{
		//integer rounded segment height
		int segment_height = t2.y - t1.y + 1;
		//paraments on line t1t2 on line t1t2 and t0t2
		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t1.y) / segment_height;
		//parameter based points
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t1 + (t2 - t1) * beta;

	//left to right
		if (A.x > B.x) std::swap(A, B);

	//fill the pixels between two points	
		for (int j = A.x; j < B.x; j++)
		{
			image.set(j, y, color);
		}
	}
}
*/

//more handy but less readable
void triangle3(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	if (t0.y == t1.y && t0.y == t2.y) return; //Colinear
	
	//sort vertices based to their heights
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; i++)
	{
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y: 0)) / segment_height;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++)
		{
			image.set(j, t0.y + i, color);
		}
	}
}


//---------------------------BARYCENTRIC EDGE FUNCTION-------------------------
Vec3f barycentric(Vec2i* pts, Vec2i P)
{
	//cross of vector (AxBx,AxCx,PxAx) ^ (AyBy,AyCy,PyAy)
	
	Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^
		           Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);

// (u,v,1) if abs(u[2]) < 1 then the triangle is degenerate 
	if(std::abs(u[2] ) < 1) return Vec3f(-1,1,1);
	
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle_edgeFunction(Vec2i* pts, TGAImage& image, TGAColor color)
{
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);

	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	Vec2i P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			image.set(P.x, P.y, color);
		}
	}
}

void triangle_edgeFunction(Vec2i p1 , Vec2i p2, Vec2i p3, TGAImage& image, TGAColor color)
{
	Vec2i pts[3] = { p1, p2 , p3 };

	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);

	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	Vec2i P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			image.set(P.x, P.y, color);
		}
	}
}


//Applications
void single_triangle_test(TGAImage& image)
{
	Vec2i pts[3] = { Vec2i(10,10),Vec2i(100,30),Vec2i(190,160) };
	triangle_edgeFunction(pts, image, red);

}


void african_face_triangulated(TGAImage& image, std::shared_ptr<Model> model)
{
	Vec3f light_dir(0, 0, -1);

//for all the triangles of the model
	for (int i = 0; i < model->nfaces(); i++)
	{
		Vec2i screen_coords[3];
		Vec3f world_coords[3];

		std::vector<int> face = model->face(i);

		for (int j = 0; j < 3; j++)
		{
			world_coords[j] = model->vert(face[j]);
			//screencoords here are window coordinates i.e correspoding to size of image
			screen_coords[j] = Vec2i((world_coords[j].x + 1.0) * width / 2.0, (world_coords[j].y + 1.0) * height / 2.0);
		}

		//face normal
		Vec3f n = ( world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//exposure to light ( costheta) ( dotproduct) (N.L)
		float intensity = n*light_dir;

		if (intensity > 0)
		{
			triangle_edgeFunction(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}
}
/*
void draw3Triangles(TGAImage& image)
{
	triangle2(Vec2i(400,0),Vec2i(0,400),Vec2i(400,400),image,TGAColor(255,0,0));		
	triangle2(Vec2i(400,800),Vec2i(0,400),Vec2i(400,400),image,TGAColor(255,255,0));
		
}
void draw3TrianglesUsingEdgeFunction(TGAImage& image)
{
	triangle_edgeFunction(Vec2i(400,0),Vec2i(0,400),Vec2i(400,400),image,TGAColor(255,255,0));		
	triangle_edgeFunction(Vec2i(400,800),Vec2i(0,400),Vec2i(400,400),image,TGAColor(255,0,255));
}
*/

int main()
{
	TGAImage image(width, height, TGAImage::RGB);
	
	std::shared_ptr<Model> model = std::make_shared<Model>("../resources/african_head/african_head.obj");

	//single_triangle_test(image);
	african_face_triangulated(image,model);
	//draw3Triangles(image);	
	//draw3TrianglesUsingEdgeFunction(image);
	image.write_tga_file("africanhead_using_edge_function.tga");

	return 0;
}



