#include "tgaimage.h"

void line1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
void line4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);
void line5(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main()
{
	TGAImage image(100, 100, TGAImage::RGB);


	line5(50, 60, 20, 80, image, white);

	line5(20, 30, 50, 80, image, red);

	
	//image.flip_vertically();
	image.write_tga_file("output_1.tga");
}

//Bresenham's line algorithm
void line1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	const float start = 0.0;
	const float end = 1.0;
	const float step = 0.01;

	for (float t = start; t < end; t += step)
	{
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;

		image.set(x, y, color);
	}

}

void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{

	for (int x = x0; x <= x1; x++)
	{
		float t = (x - x0) / static_cast<float>(x1 - x0);
		//linearly interpolate y
		int y = y0 * (1.0f - t) + y1 * t;

		image.set(x, y, color);
	}
}


void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
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
		float t = (x - x0) / static_cast<float>(x1 - x0);
		//linearly interpolate y
		int y = y0 * (1.0f - t) + y1 * t;

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

void line4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
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
	float derror = std::abs(dy / (float)dx);
	float error = 0;
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

		error += derror;
		if (error > 0.5)
		{
			y += (y1 > y0 ? 1 : -1);
			error -= 1.0;
		}
	}
}


void line5(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
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

	float derror2 = std::abs(dy)*2;
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


