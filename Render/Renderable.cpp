#include <Render/Renderable.h>
#include <Render/GLHelper.h>
#include <Base/Log.h>

Renderable::Renderable():
	_viewMatrix(Matrix4::Identity), _shader(0),
	_indexPointer(0), _numIndices(0),
	_drawMode(GL_QUADS)
{
}

Renderable::~Renderable() {
	if(_indexPointer) { free(_indexPointer); }
    clearRenderStates();
}

void Renderable::setViewMatrix(const Matrix4 &matrix) {
	_viewMatrix = matrix;
}

const Matrix4& Renderable::getViewMatrix() const {
	return _viewMatrix;
}

void Renderable::addRenderState(GenericRenderState *renderState) {
    _renderStates.push_back(renderState);
}

void Renderable::clearRenderStates() {
    RenderStateList::iterator itr;
    for(itr = _renderStates.begin(); itr != _renderStates.end(); itr++) {
        delete (*itr);
    }
    _renderStates.clear();
}

void Renderable::setIndexPointer(unsigned int *indexPointer, const unsigned int numIndices) {
	size_t byteSize = numIndices * sizeof(unsigned int);
	_indexPointer = (unsigned int *)malloc(byteSize);
	memcpy(_indexPointer, indexPointer, byteSize);
	_numIndices = numIndices;
}

void Renderable::setShader(Shader *shader) {
	_shader = shader;
}

void Renderable::setDrawMode(GLenum mode) {
    _drawMode = mode;
}

void Renderable::render() {
    RenderStateList::iterator itr;

    glPushMatrix();
    glMultMatrixf(_viewMatrix.ptr());

	if(_shader) {
		_shader->enable();
	}
    for(itr = _renderStates.begin(); itr != _renderStates.end(); itr++) {
        (*itr)->preRender();
    }

    ASSERT(_indexPointer);
    glDrawElements(_drawMode, _numIndices, GL_UNSIGNED_INT, _indexPointer);

    for(itr = _renderStates.begin(); itr != _renderStates.end(); itr++) {
        (*itr)->postRender();
    }
	if(_shader) {
		_shader->disable();
	}

    glPopMatrix();
}

Renderable* Renderable::OrthoBox(const Vector2 &pos, const Vector2 &dims, bool texCoords, bool normals, float z) {
    return OrthoBox(Vector3(pos.x, pos.y, z), Vector2(dims.x, dims.y), texCoords, normals);
}

Renderable* Renderable::OrthoBox(const Vector3 &pos, const Vector2 &dims, bool texCoords, bool normals) {
	Renderable *renderable = new Renderable();
	renderable->setViewMatrix(Matrix4::MakeTranslation(pos));

	Vector3 disp = pos + Vector3(dims.x, dims.y, 0.0f);

	// Determine if the vertex order needs to be flipped to preserve proper winding order
	bool flipped = ((dims.x < 0) != (dims.y < 0));

	// Set the verts
	if(flipped) {
		float verts[4 * 3] = {
			pos.x,  disp.y, pos.z,
			disp.x, disp.y, pos.z,
			disp.x, pos.y,  pos.z,
			pos.x,  pos.y,  pos.z
		};
        renderable->addRenderState(new VertexBufferState(4, GL_FLOAT, 3, &verts[0]));
	} else {
		float verts[4 * 3] = {
			pos.x,  pos.y,  pos.z,
			disp.x, pos.y,  pos.z,
			disp.x, disp.y, pos.z,
			pos.x,  disp.y, pos.z
		};
        renderable->addRenderState(new VertexBufferState(4, GL_FLOAT, 3, &verts[0]));
	}

	// Set the texture coordinates
	if(texCoords && flipped) {
		float texCoords[4 * 2] = {
			0, 1,
			1, 1,
			1, 0,
			0, 0
		};
        renderable->addRenderState(new TexCoordBufferState(4, GL_FLOAT, 2, &texCoords[0]));
	} else if(texCoords) {
		float texCoords[4 * 2] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};
        renderable->addRenderState(new TexCoordBufferState(4, GL_FLOAT, 2, &texCoords[0]));
	}

	// Set the normals
	if(normals) {
		float normals[4 * 3] = {
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1
		};
        renderable->addRenderState(new NormalBufferState(4, GL_FLOAT, &normals[0]));
	}

	// Set the indices
	unsigned int indices[4] = { 0, 1, 2, 3 };
	renderable->setIndexPointer(&indices[0], 4);

	return renderable;
}

Renderable* Renderable::Sprite(const Vector2 &pos, const Vector2 &dims, const float z, Shader *shader) {
    Renderable *renderable = Renderable::OrthoBox(pos, dims, true, true, z);
    renderable->setShader(shader);
    return renderable;
}

Renderable* Renderable::Lines(const std::vector<Vector2> &verts) {
    Renderable *renderable;
    float *vertexBuffer;
    unsigned int *indexBuffer;
    unsigned int size;
    unsigned int i;

    size = (unsigned int)verts.size();

    renderable = new Renderable();

    vertexBuffer = new float[size * 3];
    indexBuffer = new unsigned int[size];

    for(i=0; i < size; i++) {
        vertexBuffer[3*i  ] = verts[i].x;
        vertexBuffer[3*i+1] = verts[i].y;
        vertexBuffer[3*i+2] = 0.0f;

        indexBuffer[i] = i;
    }

    renderable->addRenderState(new VertexBufferState(size, GL_FLOAT, 3, &vertexBuffer[0]));
    renderable->setIndexPointer(&indexBuffer[0], size);
    renderable->setDrawMode(GL_LINE_STRIP);

    delete vertexBuffer;
    delete indexBuffer;

    return renderable;
}
