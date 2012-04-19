#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <Base/Base.h>
#include <Base/Vector2.h>
#include <Base/Matrix4.h>
#include <Render/Material.h>
#include <Render/UniformBuffer.h>

class Renderable {
public:
    Renderable();
    virtual ~Renderable();

    void setViewMatrix(const Matrix4 &matrix);
    const Matrix4& getViewMatrix() const;

    void setMaterial(Material *material);
    void setIndexBuffer(GLuint numIndices, GLuint *indices);
    void setAttribBuffer(const std::string &name, GLuint numElements, GLenum elementType, GLuint elementSize, void *buffer);
    void setDrawMode(GLenum mode);
    
    void enterRenderScope();
    void exitRenderScope();

    void render(const Matrix4 &projection, const Matrix4 &modelView);

public:
    static Renderable* OrthoBox(const Vector2 &pos, const Vector2 &dims, bool texCoords, bool normals, float z, Material *material);
    static Renderable* OrthoBox(const Vector3 &pos, const Vector2 &dims, bool texCoords, bool normals, Material *material);
    static Renderable* Sprite(const Vector2 &pos, const Vector2 &dims, const float z, Material *material);
    static Renderable* Lines(const std::vector<Vector2> &verts);

private:
    typedef std::map<std::string,GLuint> VertexAttribMap;

private:
    void recreateTransformBuffer(Shader *shader);
    void updateTransformBuffer(const Matrix4 &projection, const Matrix4 &modelView);

private:
    GLuint _vao;
    GLuint _indexBufferID;
    GLuint _numIndices;
    
    VertexAttribMap _vertexAttribBuffers;
    
    Matrix4 _viewMatrix;
    // TODO - This seems like a really cludgey way of doing this
    UniformBuffer *_transformBuffer;

    Material *_material;
    GLenum _drawMode;
};

typedef std::list<Renderable*> RenderableList;

#endif
