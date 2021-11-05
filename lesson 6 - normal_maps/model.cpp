#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>
#include <sstream>
#include "model.h"


Model::Model(const char* filename) : verts_(), norms_(), faces_(), uv_(), diffusemap_(), specularmap_(),normalmap_()
{
	assert(filename != 0);
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	
	if (in.fail()) return;

	char trash;
	std::string line;

	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());

		if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			vec3f v;
			for (int i = 0; i < 3; i++)
			{
				iss >> v[i];
			}
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash >> trash;
			vec3f n;

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
//			std::cout << "uvm:{" << uv.x << " " << uv.y << "}" << std::endl;
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
	load_texture(filename, "_nm_tangent.tga", normalmap_);
	load_texture(filename, "_spec.tga", specularmap_);
	load_texture(filename, "_grid.tga", grid_);


}

Model::~Model() {}

int Model::nverts()
{
	return (int)verts_.size();
}

int Model::nfaces()
{
	return (int)faces_.size();
}

vec3f Model::vert(int iface, int nthvert)
{
	return verts_[faces_[iface][nthvert][0]];
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


TGAColor Model::diffuse(vec2f uvf)
{
	//map to texture map
	
	vec2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());	

	return diffusemap_.get(uv[0], uv[1]);
}

TGAColor Model::grid(vec2f uvf)
{
	//map to texture map

	vec2i uv(uvf[0] * grid_.get_width(), uvf[1] * grid_.get_height());
	return grid_.get(uv[0], uv[1]);
}

vec3f Model::normal(vec2f uvf)
{
	vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
	TGAColor c = normalmap_.get(uv[0], uv[1]);
	vec3f res;

	for (int i = 0; i < 3; i++)
	{
		res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
	}

	return res;
}

vec2f Model::uv(int iface, int nthvert)
{
	int idx = faces_[iface][nthvert][1];

	vec2f tmp =  uv_[idx];
	//std::cout << "uv_[" << idx << "]" << ": " << tmp.x << " " << tmp.y << std::endl;

	return tmp;
}


float Model::specular(vec2f uvf)
{
	vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1]*specularmap_.get_height());
	return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

vec3f Model::normal(int iface, int nthvert)
{
	int idx = faces_[iface][nthvert][2];
	return norms_[idx].normalize();
}














