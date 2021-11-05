#include "model.h"
#include "algo.h"


const int width = 800;
const int height = 800;
const TGAColor white(255, 255, 255, 255);

int main()
{
	std::shared_ptr<Model> model = std::make_shared<Model>("resources/african_head/african_head.obj");
	TGAImage image(width, height, TGAImage::RGB);

	for (int i = 0; i < model->nfaces(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vec3 v0 = model->vert(i,j);
			vec3 v1 = model->vert(i,(j + 1) % 3);

			int x0 = (v0.x + 1.0) * width  / 2;
			int y0 = (v0.y + 1.0) * height / 2;
			int x1 = (v1.x + 1.0) * width  / 2;
			int y1 = (v1.y + 1.0) * height / 2;

			line(x0, y0, x1, y1, image, white);
		}
	}

	image.write_tga_file("output_2.tga");
}



