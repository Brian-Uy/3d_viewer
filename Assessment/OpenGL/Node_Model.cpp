#include "Node_Model.h"

NodeModel::NodeModel(std::string modelFilePath, std::string normalFilePath) : model(Model(modelFilePath)), normalTexture(TextureLoader::loadTexture(normalFilePath)) {
	model.attachTexture(normalTexture, "texture_normal");
}

NodeModel::~NodeModel(void) {}