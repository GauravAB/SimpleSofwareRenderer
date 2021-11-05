#ifndef _ALGO_H__
#define _ALGO_H__
#include <algorithm>

#include "../impl/tgaimage.h"
#include "../impl/geometry.h"

//BRESENHAM LINE DRAWER
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

void line(vec2 t1 , vec2 t2, TGAImage& image, TGAColor color)
{
	
	bool steep = false;

	if (std::abs(t1.x - t2.x) < std::abs(t1.y - t2.y))
	{
		std::swap(t1.x, t1.y);
		std::swap(t2.x, t2.y);
		steep = true;
	}

	if (t1.x > t2.x)
	{
		std::swap(t1,t2);
	}

	for(int x = t1.x; x <= t2.x; x++)
	{
		float t = (x - t1.x)/(float)(t2.x - t1.x);
		int y = t1.y*(1.-t) + t2.y *t;
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



#endif