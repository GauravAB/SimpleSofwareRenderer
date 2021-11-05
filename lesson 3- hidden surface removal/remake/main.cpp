#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

using namespace gbGeometry;


const TGAColor white = TGAColor(255,255,255);
const TGAColor red = TGAColor(255,0,0);

const int width = 800;
const int height = 800;

Vec3f barycentric(Vec3f A , Vec3f B , Vec3f C, Vec3f P)
{
    Vec3f s[2];
    //ACx ABx PAx
    //ACy ABy PAy
    for(int i =2; i--;)
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }

//cross product to find (u , v , 1 ) orthogonal to AC AB PA 's x and y component vectors
    Vec3f u = Cross(s[0],s[1]);
    //non-zero non-negative u[2] means no degenerate triangle 
    if(std::abs(u[2]) > 1e-2)
        //return barycentric coordinates
        return Vec3f(1.f- (u.x + u.y) / u.z, u.y / u.z , u.x / u.z);
    
    //if degenerate triangle
    return Vec3f(-1,1,1);
}

//whole algorithm is carries out in screen-space coordinates
//integer values are promoted to float for calculations
void triangle(Vec3f *pts , float* zbuffer, TGAImage& image, float intensity, Vec2f* uv , std::shared_ptr<Model> model)
{
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() -1);


 //min max bounding box
   for(int i = 0; i < 3; i++)
   {
        for(int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j],pts[i][j]));
            bboxmax[j] = std::min(clamp[j],std::max(bboxmax[j],pts[i][j]));
        }
    }

    Vec3f P;

    //iterate all pixels in the bounding box
    for(P.x = bboxmin.x ; P.x <= bboxmax.x ; P.x++)
    {
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f bc_screen = barycentric(pts[0],pts[1],pts[2],P);
            //if degenerate triangle than skip this pixel
            if(bc_screen.x <0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            P.z = 0;
            //evaluate interpolated z value for the point in polygon
            for(int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
            //if z value less than  available value at that position 
            if(zbuffer[int(P.x + P.y * width)] < P.z)
            {
                // update z-buffer with values closer to camera
                zbuffer[int(P.x + P.y * width)] = P.z;

                //ADD TEXTURE         
                /*
                Vec2f puv;
                for(int i = 0; i < 3; i++)
                {
                   //interpolate uv coordinates like for z
                   puv.x = puv.x + uv[i].x * bc_screen[i];
                   puv.y = puv.y + uv[i].y * bc_screen[i];                  
                }

                //z-buffer verified point in polygon is rendered
                image.set(P.x,P.y,
                    model->diffuse_map.get(puv.x * model->diffuse_map.get_width(),
                            puv.y *  model->diffuse_map.get_height())*intensity);
                */

               //FLAT SHADING
                image.set(P.x,P.y,TGAColor(255)*intensity);
            }
        }
    }
}

//from world(-1,1) + 1.0 => world(0,2) / 2.0 => world(0,1) * width or height = > screen(0 to width)
Vec3f world2screen(Vec3f v)
{
    return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.)*height/2. + 0.5),v.z);
}


int main(int argc , char** argv)
{
    std::shared_ptr<Model> model;

    if(2 == argc)
    {
        model = std::make_shared<Model>(argv[1]);
    }
    else{
        model = std::make_shared<Model>("suz.obj");
    }


    //2D depth buffer to store depth values of all visible pixels 
    float *zbuffer = new float[width * height];
    for(int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f lightDir(0,0,-1);

    for(int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f pts[3];
        Vec3f wc[3];
        Vec2f uv[3];
        for(int j =0 ; j < 3; j++)
        {
            wc[j] = model->vert(face[j*2]);
            pts[j] = world2screen(wc[j]);
            uv[j] = model->st(face[(j*2) + 1]);
        }
           
        Vec3f facenormal = Cross(Vec3f(wc[2]-wc[0]),Vec3f(wc[1]-wc[0]));
        facenormal = Normalize(facenormal);
        std::cout << facenormal << std::endl;


        float intensity = Dot(facenormal,lightDir);
        //std::cerr << intensity << std::endl;

        if(intensity > 0)
        {
            triangle(pts,zbuffer,image,intensity,uv,model);
        }
    }

    image.write_tga_file("suz.tga");

    TGAImage zimage(width,height,TGAImage::GRAYSCALE);
    
    for(int i = 0 ; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            zimage.set(j,i,TGAColor(255*((zbuffer[i * width + j] + 1.0) / 2.0)));
        }
    }

    zimage.write_tga_file("zbuffer.tga");

    delete []zbuffer;


    return 0;
}













