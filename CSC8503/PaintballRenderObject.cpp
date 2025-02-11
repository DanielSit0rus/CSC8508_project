#include "PaintballRenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

PaintballRenderObject::PaintballRenderObject(PaintballTransform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader) {
	if (!tex) {
		bool a = true;
	}
	this->transform = parentTransform;
	this->mesh = mesh;
	this->texture = tex;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

PaintballRenderObject::~PaintballRenderObject() {

}