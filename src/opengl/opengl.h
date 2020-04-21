#ifndef OPENGL_H
#define OPENGL_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include <thread>

#include <epoxy/gl.h>
#include <epoxy/glx.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "volumes.h"
#include "sparse_vector.h"

namespace gl {

class Camera {

public:
        enum Mode {
                PERSPECTIVE,
                ORTHO
        };

        Camera();

        glm::mat4 view();
        glm::mat4 projection();

        // TODO change position/direction functions
        Mode mode;

        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 up;

        float fov;
        float zNear, zFar, aspect;
        float width, height;
        float left, right, top, bottom;
};

class Shader {
public:
        Shader();
        Shader(const char *vertexPath, const char *fragmentPath);

        unsigned int program();

        void setMat4(const std::string &name, const glm::mat4 &value);
        void setMat3(const std::string &name, const glm::mat3 &value);
        void setInt(const std::string &name, const int &value);
        void setFloat(const std::string &name, const float &value);

private:
        unsigned int m_vertex;
        unsigned int m_fragment;
        unsigned int m_program;

        std::map<std::string, int> m_uniforms;

        bool addFromFile(int type, const char *path);
        bool addFromSource(int type, const char *src);
        bool link();

        int uniformLocation(const std::string &name);
};

struct Texture {
        unsigned int id;
        std::string type;
        std::string path;

        Texture(unsigned int id, const char* type, const char* path);

        static Texture loadFromPath(const char* path, const char* type);
        static Texture loadFromImage(unsigned char* image, int w, int h, int ch, const char* type);
};

struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texCoords;

        bool operator==(const Vertex& other) const;

        friend std::ostream &operator<<(std::ostream& strm, const Vertex& v)
        {
                strm << "position : (" << v.pos[0] << "," << v.pos[1] << "," << v.pos[2] << "), ";
                strm << "normal : (" << v.normal[0] << "," << v.normal[1] << "," << v.normal[2] << "), ";
                strm << "color : (" << v.color[0] << "," << v.color[1] << "," << v.color[2] << "), ";
                strm << "texCoords : (" << v.texCoords[0] << "," << v.texCoords[1] << ")";
                strm << std::endl;

                return strm;
        }
};

class Mesh {
public:
        Mesh();

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
             std::vector<Texture> textures);

        ~Mesh();

        Volume volume();

        void updateVBO();
        void updateInstancesVBO();

        unsigned int VAO, VBO, EBO, intancesVBO;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        sparse_vector<glm::mat4> instances;
        glm::mat4 transform;

private:
        Volume m_volume;
        size_t m_instancesDrawn;

        void setup();
};

class Model {
public:
        Model();
        Model(const std::vector<Mesh> &meshes);
        Model(const std::string &path);

        ~Model();

        size_t addInstance(const glm::mat4 &transform);
        void removeInstance(size_t index);

        std::vector<Mesh> meshes;

private:
        std::string m_path;

        std::string pathFromFileName(const std::string &fileName);

        void load(const std::string &path);
};

class OpenGLScene {
public:
        Camera camera;
        std::map<std::string, Model> models;
        std::map<std::string, Mesh*> meshes;
        std::map<std::string, Shader> shaders;
        // TODO lights
};

class OpenGLWindow {
public:
	OpenGLWindow();
        ~OpenGLWindow();

	void setTitle(const char *title);
        void show();
        void resize(int width, int height);
        void close();
        float fps();
        uint32_t lag();
        bool isRunning();
        bool aboutToBeClosed();

        OpenGLScene *scene();
        SDL_Window *sdlWindow();
        SDL_GLContext context();

protected:
        virtual void update(float dt);
        virtual void processEvent(const SDL_Event &event);

private:
        void processEvents();

        OpenGLScene *m_scene;
        SDL_Window *m_window;
        SDL_GLContext m_context;

        uint32_t m_elapsed;
        uint32_t m_lag;

        bool m_running;
        bool m_open;
};

} // namespace gl

#endif
