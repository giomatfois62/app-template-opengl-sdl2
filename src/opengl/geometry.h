#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "opengl.h"

#define DEFAULT_VERTEX_COLOR glm::vec3(0.8f, 0.8f, 0.8f)
#define RED_VERTEX_COLOR glm::vec3(1.0f, 0.0f, 0.0f)
#define GREEN_VERTEX_COLOR glm::vec3(0.0f, 1.0f, 0.0f)
#define BLUE_VERTEX_COLOR glm::vec3(0.0f, 0.0f, 1.0f)

namespace gl {

Mesh triangle()
{
    std::vector<Vertex> vertices = {
        {{-1,-1,0}, {0,0,1}, {1,0,0}, {0,0}},
        {{1,-1,0}, {0,0,1}, {0,1,0}, {0,0}},
        {{0,1,0}, {0,0,1}, {0,0,1}, {0,0}},
    };

    std::vector<unsigned int> indices = {0, 1, 2};

   std::vector<Texture> textures = {
   Texture::loadFromPath("res/smile.png", "texture_diffuse")
   };

    return Mesh(vertices, indices, textures);
}

Mesh quad()
{
    std::vector<Vertex> vertices = {
        {{-1,-1,0}, {0,0,1}, {0,1,0}, {0,0}},
        {{1,-1,0}, {0,0,1}, {0,1,0}, {1,0}},
        {{1,1,0}, {0,0,1}, {0,1,0}, {1,1}},
        {{-1,1,0}, {0,0,1}, {0,1,0}, {0,1}},
    };

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

	std::vector<Texture> textures = {
	Texture::loadFromPath("res/smile.png", "texture_diffuse")
	};

    return Mesh(vertices, indices, textures);
}

Mesh* grid(double minX, double maxX, double minY, double maxY, size_t Nx, size_t Ny)
{
    std::vector<Vertex> vertices;

    double dx = (maxX - minX) / (Nx - 1);
    double dy = (maxY - minY) / (Ny - 1);

    for(size_t i = 0; i < Nx; ++i) {
        for(size_t j = 0; j < Ny; ++j) {
            Vertex v;

            v.pos = {minX + i * dx, minY + j * dy, 0};
            v.normal = {0, 0, 1};
            v.color = DEFAULT_VERTEX_COLOR;
            v.texCoords = {i / (float)(Nx-1), j / (float)(Ny-1)};

            vertices.push_back(v);
        }
    }

    std::vector<unsigned int> indices;

    for(size_t i = 0; i < Nx - 1; ++i) {
        for(size_t j = 0; j < Ny - 1; ++j) {
            indices.push_back(i * Ny + j);
            indices.push_back(i * Ny + j + 1);
            indices.push_back((i+1) * Ny + j+1);

            indices.push_back(i * Ny + j);
            indices.push_back((i+1) * Ny + j);
            indices.push_back((i+1) * Ny + j+1);
        }
    }

    std::vector<Texture> textures; /* = {
        Texture::loadFromPath("res/smile.png", "texture_diffuse")
    };*/

    return new Mesh(vertices, indices, textures);
}

Mesh cube()
{
    std::vector<Vertex> vertices = {
        {{-1,-1,1}, {0,0,1}, RED_VERTEX_COLOR, {0,0}},
        {{1,-1,1}, {0,0,1}, RED_VERTEX_COLOR, {1,0}},
        {{1,1,1}, {0,0,1}, RED_VERTEX_COLOR, {1,1}},
        {{-1,1,1}, {0,0,1}, RED_VERTEX_COLOR, {0,1}},

        {{-1,-1,-1}, {0,0,-1}, RED_VERTEX_COLOR, {0,0}},
        {{1,-1,-1}, {0,0,-1}, RED_VERTEX_COLOR, {1,0}},
        {{1,1,-1}, {0,0,-1}, RED_VERTEX_COLOR, {1,1}},
        {{-1,1,-1}, {0,0,-1}, RED_VERTEX_COLOR, {0,1}},

        {{1,-1,1}, {1,0,0}, GREEN_VERTEX_COLOR, {0,0}},
        {{1,-1,-1}, {1,0,0}, GREEN_VERTEX_COLOR, {1,0}},
        {{1,1,-1}, {1,0,0}, GREEN_VERTEX_COLOR, {1,1}},
        {{1,1,1}, {1,0,0}, GREEN_VERTEX_COLOR, {0,1}},

        {{-1,-1,1}, {-1,0,0}, GREEN_VERTEX_COLOR, {0,0}},
        {{-1,-1,-1}, {-1,0,0}, GREEN_VERTEX_COLOR, {1,0}},
        {{-1,1,-1}, {-1,0,0}, GREEN_VERTEX_COLOR, {1,1}},
        {{-1,1,1}, {-1,0,0}, GREEN_VERTEX_COLOR, {0,1}},

        {{-1,1,1}, {0,1,0}, BLUE_VERTEX_COLOR, {0,0}},
        {{1,1,1}, {0,1,0}, BLUE_VERTEX_COLOR, {1,0}},
        {{1,1,-1}, {0,1,0}, BLUE_VERTEX_COLOR, {1,1}},
        {{-1,1,-1}, {0,1,0}, BLUE_VERTEX_COLOR, {0,1}},

        {{-1,-1,1}, {0,-1,0}, BLUE_VERTEX_COLOR, {0,0}},
        {{1,-1,1}, {0,-1,0}, BLUE_VERTEX_COLOR, {1,0}},
        {{1,-1,-1}, {0,-1,0}, BLUE_VERTEX_COLOR, {1,1}},
        {{-1,-1,-1}, {0,-1,0}, BLUE_VERTEX_COLOR, {0,1}},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    std::vector<Texture> textures = {
        Texture::loadFromPath("res/smile.png", "texture_diffuse")
    };

    return Mesh(vertices, indices, textures);
}

} // namespace gl

#endif
