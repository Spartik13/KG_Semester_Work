#include "Mesh.h"
#include "MathUtils.h"

MeshData MakeCubeData() {
    MeshData d;
    // 6 граней: нормаль + два касательных вектора
    const float normals[6][3] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
    const float tangents[6][3] = {{0, 0, -1}, {0, 0, 1}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {-1, 0, 0}};
    for (int f = 0; f < 6; ++f) {
        Vec3 n(normals[f][0], normals[f][1], normals[f][2]);
        Vec3 t(tangents[f][0], tangents[f][1], tangents[f][2]);
        Vec3 b = Cross(n, t);
        const float su[4] = {-1, 1, 1, -1};
        const float sv[4] = {-1, -1, 1, 1};
        unsigned int base = static_cast<unsigned int>(d.vertices.size() / 6);
        for (int i = 0; i < 4; ++i) {
            Vec3 p = n * 0.5f + t * (su[i] * 0.5f) + b * (sv[i] * 0.5f);
            d.vertices.insert(d.vertices.end(), {p.x, p.y, p.z, n.x, n.y, n.z});
        }
        d.indices.insert(d.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    }
    return d;
}

// Цилиндр: боковая поверхность и две крышки.
MeshData MakeCylinderData(int slices) {
    MeshData d;
    const float r = 0.35f, h = 0.5f;

    // Боковая поверхность
    for (int j = 0; j <= slices; ++j) {
        float theta = 2.0f * kPi * static_cast<float>(j) / slices;
        float c = std::cos(theta), s = std::sin(theta);
        d.vertices.insert(d.vertices.end(), {r * c, h, r * s, c, 0.0f, s});
        d.vertices.insert(d.vertices.end(), {r * c, -h, r * s, c, 0.0f, s});
    }
    for (int j = 0; j < slices; ++j) {
        unsigned int a = j * 2;
        d.indices.insert(d.indices.end(), {a, a + 2, a + 1, a + 2, a + 3, a + 1});
    }

    // Крышки: верхняя и нижняя
    for (int cap = 0; cap < 2; ++cap) {
        float y = cap == 0 ? h : -h;
        float ny = cap == 0 ? 1.0f : -1.0f;
        unsigned int center = static_cast<unsigned int>(d.vertices.size() / 6);
        d.vertices.insert(d.vertices.end(), {0.0f, y, 0.0f, 0.0f, ny, 0.0f});
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * kPi * static_cast<float>(j) / slices;
            d.vertices.insert(d.vertices.end(), {r * std::cos(theta), y, r * std::sin(theta), 0.0f, ny, 0.0f});
        }
        for (int j = 0; j < slices; ++j) {
            if (cap == 0)
                d.indices.insert(d.indices.end(), {center, center + 2 + j, center + 1 + j});
            else
                d.indices.insert(d.indices.end(), {center, center + 1 + j, center + 2 + j});
        }
    }
    return d;
}

MeshData MakePlaneData(float h) {
    MeshData d;
    d.vertices = {-h, 0, -h, 0, 1, 0,
                   h, 0, -h, 0, 1, 0,
                   h, 0,  h, 0, 1, 0,
                  -h, 0,  h, 0, 1, 0};
    d.indices = {0, 2, 1, 0, 3, 2};
    return d;
}

Mesh::~Mesh() { Release(); }

void Mesh::Release() {
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    vao_ = vbo_ = ebo_ = 0;
    indexCount_ = 0;
}

void Mesh::Create(const MeshData& data) {
    Release();
    indexCount_ = static_cast<GLsizei>(data.indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(float), data.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

    // атрибут 0 — позиция, атрибут 1 — нормаль
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Mesh::Draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
