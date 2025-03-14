#pragma once
#include "Includes.h"

// transformations
struct Transformation {
	float position[3] = { 0.0f, 0.0f, 0.0f };
	float rotation[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 0.1f, 0.1f, 0.1f };
};

class NodeModel {
public:
	// models
	Model model;
	// normal texture map
	GLuint normalTexture;
	// model transformation
	Transformation transform;

	NodeModel(std::string modelFilePath, std::string normalFilePath);
	~NodeModel(void);
};