#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include "model.h"
#include <string>


Model::Model(const char* filename) : verts_(), norms_(), uv_(), faces_(), diffusemap_()
{
	assert(filename != 0);
	std::ifstream in;
	in.open(filename, std::ifstream::in);

	if (in.fail())
	{
		std::cerr << "failed to open file " << filename << std::endl;
		return;
	}

	std::string line;
	char trash;

	while (!in.eof())
	{
		std::string line;
		std::getline(in, line);
		std::istringstream iss(line.c_str());

		if (!line.compare(0, 2, "v "))
		{
			vec3f v;
			iss >> trash;
			for (int i = 0; i < 3; i++)
			{
				iss >> v[i];
			}
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			vec3f n;
			iss >> trash >> trash;
			for (int i = 0; i < 3; i++)
			{
				iss >> n[i];
			}
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash >> trash;
			vec2f uv;
			for (int i = 0; i < 2; i++)
			{
				iss >> uv[i];
			}

			uv_.push_back(uv);
		}
		else if (!line.compare(0, 2, "f "))
		{
			iss >> trash;
			std::vector<vec3i> f;
			vec3i tmp;

			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
			{
				for (int i = 0; i < 3; i++) tmp[i]--;
				f.push_back(tmp);
			}

			faces_.push_back(f);
		}
	}
	
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;

	load_texture(filename, "_diffuse.tga", diffusemap_);
}

Model::~Model()
{

}


int Model::nverts()
{
	return (int)verts_.size();
}


int Model::nfaces()
{
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
	std::vector<int> face;
	for (int i = 0; i < (int)faces_[idx].size(); i++)
	{
		face.push_back(faces_[idx][i][0]);
	}
	return face;
}

vec3f Model::vert(int i)
{
	return verts_[i];
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");

	if (dot != std::string::npos)
	{
		texfile = texfile.substr(0, dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

TGAColor Model::diffuse(vec2i uv)
{
	return diffusemap_.get(uv.x, uv.y);
}

vec2i Model::uv(int iface, int nvert)
{
	int idx = faces_[iface][nvert][1];
	return vec2i(uv_[idx].x * diffusemap_.get_width(), uv_[idx].y * diffusemap_.get_height());
}

vec3f Model::norm(int iface, int nvert)
{
	int idx = faces_[iface][nvert][2];
	return norms_[idx].normalize();
}


















