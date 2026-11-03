#pragma once
#include <GL/glew.h>
#include <vector>

// Данные вершин: позиция + нормаль на вершину.
struct MeshData {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

MeshData MakeCubeData();
MeshData MakeCylinderData(int slices);
MeshData MakePlaneData(float halfSize);

class Mesh {
public:
    Mesh() = default;
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Create(const MeshData& data);
    void Release();
    void Draw() const;

private:
    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLsizei indexCount_ = 0;
};