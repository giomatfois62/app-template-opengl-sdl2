#include "opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace gl;
using namespace std;
using namespace glm;

Camera::Camera() :
        mode(PERSPECTIVE),
        fov(90.0f),
        zNear(0.001f),
        zFar(1000.0f)
{
    position = vec3(0.0f, 0.0f, 4.2f);
    direction = vec3(0.0f, 0.0f, -1.0f);
    up = vec3(0.0f, 1.0f, 0.0f);
}

mat4 Camera::view()
{
    return lookAt(position, position + direction, up);
}

mat4 Camera::projection()
{
    if(mode == PERSPECTIVE)
        return perspective(radians(fov), aspect, zNear, zFar);
    else
        return ortho(left, right, top, bottom, zNear, zFar);
}

Shader::Shader() {}

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    addFromFile(GL_VERTEX_SHADER, vertexPath);
    addFromFile(GL_FRAGMENT_SHADER, fragmentPath);
    link();
}

unsigned int Shader::program() { return m_program; }

void Shader::setMat4(const string &name, const mat4 &value)
{
    glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::setMat3(const string &name, const mat3 &value)
{
    glUniformMatrix3fv(uniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::setInt(const string &name, const int &value)
{
    glUniform1i(uniformLocation(name), value);
}

void gl::Shader::setFloat(const std::string &name, const float &value)
{
    glUniform1f(uniformLocation(name), value);
}

bool Shader::addFromFile(int type, const char *path)
{
    ifstream t(path);
    stringstream buffer;

    buffer << t.rdbuf();

    string src = buffer.str();

    return addFromSource(type, src.c_str());
}

bool Shader::addFromSource(int type, const char *src)
{
    int res;
    unsigned int *shader;

    switch (type) {
        case GL_VERTEX_SHADER:
            shader = &m_vertex;
            break;
        case GL_FRAGMENT_SHADER:
            shader = &m_fragment;
            break;
    }

    *shader = glCreateShader(type);

    glShaderSource(*shader, 1, &src, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &res);

    if(!res) {
        char infoLog[512];
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << endl;
    }

    return res;
}

bool Shader::link()
{
    m_program = glCreateProgram();

    glAttachShader(m_program, m_vertex);
    glAttachShader(m_program, m_fragment);
    glLinkProgram(m_program);

    int res;
    glGetProgramiv(m_program, GL_LINK_STATUS, &res);

    if(!res) {
        char infoLog[512];
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(m_vertex);
    glDeleteShader(m_fragment);

    return res;
}

int Shader::uniformLocation(const string &name)
{
    auto it = m_uniforms.find(name);

    if(it == m_uniforms.end()) {
        int location = glGetUniformLocation(program(), name.c_str());
        it = m_uniforms.insert(make_pair(name, location)).first;
    }

    return it->second;
}

Texture::Texture(unsigned int id, const char *type, const char *path) :
    id(id), type(type), path(path)
{

}

Texture Texture::loadFromPath(const char *path, const char *type)
{
    stbi_set_flip_vertically_on_load(true);

    int w, h, ch;
    unsigned char* image = stbi_load(path, &w, &h, &ch, 0);

    if(!image) {
        cerr << "Unable to load texture: " << path << endl;
        exit(1);
    }

    Texture text = loadFromImage(image, w, h, ch, type);
    text.path = path;

    stbi_image_free(image);

    return text;
}

Texture Texture::loadFromImage(unsigned char *image, int w, int h, int ch, const char *type)
{
    unsigned int id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (ch == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (ch == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture(id, type, "");
}

bool Vertex::operator==(const Vertex &other) const
{
    return pos == other.pos && normal == other.normal &&
            color == other.color && texCoords == other.texCoords;
}

Mesh::Mesh() {}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) :
    vertices(vertices),
    indices(indices),
    textures(textures),
    transform(mat4(1.0f)),
    m_instancesDrawn(0)
{
    setup();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &intancesVBO);
    glDeleteVertexArrays(1, &VAO);
}

Volume Mesh::volume() { return m_volume; }

void Mesh::updateVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(),
                     &vertices[0] );
}

void Mesh::updateInstancesVBO()
{
    // remember number of bytes allocated last call, use glBufferSubdata if possible
    // https://www.roxlu.com/2014/028/opengl-instanced-rendering
    glBindBuffer(GL_ARRAY_BUFFER, intancesVBO);

    size_t mat4size = sizeof(mat4);

    if(instances.realSize() != m_instancesDrawn) {
        glBufferData( GL_ARRAY_BUFFER, mat4size * m_instancesDrawn,
                      nullptr, GL_STREAM_DRAW );
        glBufferData( GL_ARRAY_BUFFER, mat4size * instances.realSize(),
                      &instances[0], GL_STREAM_DRAW );
        m_instancesDrawn = instances.realSize();
    } else {
        glBufferSubData( GL_ARRAY_BUFFER, 0, mat4size * instances.realSize(),
                         &instances[0] );
    }
}

void Mesh::setup()
{
    // compute volume
    vec3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
    vec3 max = {FLT_MIN, FLT_MIN, FLT_MIN};

    for(Vertex v : vertices) {
        min[0] = fminf(min[0], v.pos[0]);
        min[1] = fminf(min[1], v.pos[1]);
        min[2] = fminf(min[2], v.pos[2]);

        max[0] = fmaxf(max[0], v.pos[0]);
        max[1] = fmaxf(max[1], v.pos[1]);
        max[2] = fmaxf(max[2], v.pos[2]);
    }

    m_volume = Volume(min, max);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &intancesVBO);

    // bind VAO
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    // vertex colors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    // vertex texture coords
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    // set vertex attribute for instance matrices
    glBindBuffer(GL_ARRAY_BUFFER, intancesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * instances.size(),
                 &instances[0], GL_STREAM_DRAW);

    size_t location = 4;
    size_t vec4size = sizeof(vec4);
    size_t stride = 4 * vec4size;
    size_t offset = 0;

    for(size_t i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
                              stride, (void*)offset);

        glVertexAttribDivisor(location, 1);

        location++;
        offset += vec4size;
    }

    // unbind VAO
    glBindVertexArray(0);
}

Model::Model() {}

Model::Model(const vector<Mesh> &meshes) : meshes(meshes) {}

Model::Model(const string &path) : m_path(path)
{
    load(path);
}

Model::~Model() {}

size_t Model::addInstance(const mat4 &transform)
{
    if(meshes.size() == 0)
        return 0;

    size_t index;

    for(Mesh &mesh : meshes)
        index = mesh.instances.insert(transform);

    return index;
}

void Model::removeInstance(size_t index)
{
    for(Mesh &mesh : meshes)
        mesh.instances.remove(index);
}

string Model::pathFromFileName(const string &fileName)
{
    string path;

    if (fileName.find_last_of("/\\") != string::npos) {
#ifndef _WIN32
        const char dirsep = '/';
#else
        const char dirsep = '\\';
#endif

        path = fileName.substr(0, fileName.find_last_of("/\\")) + dirsep;
    }

    return path;
}

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
template<> struct hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return ((hash<vec3>()(vertex.pos) ^
                 (hash<vec3>()(vertex.normal) << 1)) >> 1) ^
                (hash<vec3>()(vertex.color) << 1);
    }
};
}

void Model::load(const string &path)
{
    tinyobj::ObjReader reader;

    if(!reader.ParseFromFile(path)) {
        cerr << "Unable to load model from " << path << " : " << reader.Error() << endl;
        exit(1);
    }

    string texturePath = pathFromFileName(path);
    vector<Texture> allTextures;

    // Loop over shapes
    for(size_t s = 0; s < reader.GetShapes().size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;

        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        unordered_map<Vertex, size_t> uniqueVertices;

        for(size_t f = 0; f < reader.GetShapes()[s].mesh.num_face_vertices.size(); f++) {

            size_t fv = reader.GetShapes()[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for(size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = reader.GetShapes()[s].mesh.indices[index_offset + v];

                tinyobj::real_t vx = reader.GetAttrib().vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = reader.GetAttrib().vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = reader.GetAttrib().vertices[3*idx.vertex_index+2];

                tinyobj::real_t nx = reader.GetAttrib().normals[3*idx.normal_index+0];
                tinyobj::real_t ny = reader.GetAttrib().normals[3*idx.normal_index+1];
                tinyobj::real_t nz = reader.GetAttrib().normals[3*idx.normal_index+2];

                tinyobj::real_t tx = reader.GetAttrib().texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = reader.GetAttrib().texcoords[2*idx.texcoord_index+1];

                // Optional: vertex colors
                tinyobj::real_t red = reader.GetAttrib().colors[3*idx.vertex_index+0];
                tinyobj::real_t green = reader.GetAttrib().colors[3*idx.vertex_index+1];
                tinyobj::real_t blue = reader.GetAttrib().colors[3*idx.vertex_index+2];

                Vertex vertex;

                vertex.pos = {vx, vy, vz}; // pos
                vertex.normal = {nx, ny, nz}; // normal
                vertex.color = {red, green, blue}; // color
                vertex.texCoords = {tx, ty}; // texCoords

                if(uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = vertices.size();
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }

            index_offset += fv;

            // TODO: read per-face material
            size_t materialId = reader.GetShapes()[s].mesh.material_ids[f];
            string diffuseTexture = texturePath + reader.GetMaterials()[materialId].diffuse_texname;

            bool textureLoaded = false;

            for(size_t i = 0; i < allTextures.size(); ++i) {
                if(strcmp(allTextures[i].path.data(), diffuseTexture.c_str()) == 0) {
                    textureLoaded = true;
                    textures.push_back(allTextures[i]);
                    break;
                }
            }

            if(!textureLoaded) {
                Texture texture = Texture::loadFromPath(diffuseTexture.c_str(), "texture_diffuse");
                allTextures.push_back(texture);
                textures.push_back(texture);
            }
        }

        Mesh mesh(vertices, indices, textures);
        meshes.push_back(mesh);

        cout << "Loaded Mesh with " << vertices.size() << " verts and " << mesh.volume() << " Volume" << endl;
    }
}

OpenGLWindow::OpenGLWindow() : m_running(false), m_open(true)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    m_window = SDL_CreateWindow("window", 100, 100, 800, 600,
	SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_context = SDL_GL_CreateContext(m_window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    m_scene = new OpenGLScene;

    resize(800, 600);
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_scene;

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void OpenGLWindow::setTitle(const char *title)
{
	SDL_SetWindowTitle(m_window, title);
}

void OpenGLWindow::show()
{
    m_running = true;

    m_elapsed = SDL_GetTicks();

    while(m_running) {
        processEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uint32_t time = SDL_GetTicks();
        m_lag = time - m_elapsed;

        m_elapsed = time;

        update(m_lag * 0.001f);

        SDL_GL_SwapWindow(m_window);
    }
}

void OpenGLWindow::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    m_scene->camera.width = width;
    m_scene->camera.height = height;
    m_scene->camera.aspect = (float) width / (float) height;
}

void OpenGLWindow::close()
{
    m_running = false;
    m_open = false;
}

float OpenGLWindow::fps()
{
    return 1.0f/(m_lag * 0.001f);
}

uint32_t OpenGLWindow::lag()
{
    return m_lag;
}

bool OpenGLWindow::isRunning()
{
    return m_running;
}

bool OpenGLWindow::aboutToBeClosed()
{
    return !m_open;
}

OpenGLScene *OpenGLWindow::scene()
{
    return m_scene;
}

SDL_Window *OpenGLWindow::sdlWindow()
{
    return m_window;
}

SDL_GLContext OpenGLWindow::context()
{
    return m_context;
}

void OpenGLWindow::update(float dt)
{

}

void OpenGLWindow::processEvent(const SDL_Event &event)
{

}

void OpenGLWindow::processEvents()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT)
            close();

        if(event.type == SDL_WINDOWEVENT)
            if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                resize(event.window.data1, event.window.data2);

        processEvent(event);
    }
}


