#pragma once
#include "Includes.h"
#include "Node_Model.h"

// default lighting variables
struct Light {
	GLfloat position[4]			= { 0.0f, 1.0f, 0.0f, 0.0f };
	GLfloat diffuse[4]			= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient[4]			= { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat attenuation[3]		= { 0.35,0.35,0.35 };
};

// default material variables
struct Material {
	GLfloat diffuse[4]			= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient[4]			= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularCol[4]		= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularExp			= 1.0f;
};

class Scene {
private:
	// shader program
	GLuint basicShader;

	// lighting uniform locations
	GLuint uLightPosition		= 0;
	GLuint uLightAmbient		= 0;
	GLuint uLightDiffuse		= 0;
	GLuint uLightAttenuation	= 0;
	GLuint uEyePos				= 0;

	// material uniform locations
	GLuint uMatAmbient			= 0;
	GLuint uMatDiffuse			= 0;
	GLuint uMatSpecularCol		= 0;
	GLuint uMatSpecularExp		= 0;

	// camera pointer
	Camera* camera				= nullptr;

	// mvp components
	glm::mat4 model				= glm::mat4(0.0f);
	glm::mat4 view				= glm::mat4(0.0f);
	glm::mat4 projection		= glm::mat4(0.0f);
	glm::vec3 eyePos			= glm::vec3(0.0f, 0.0f, 0.0f);

public:
	// lighting (default)
	Light light;
	// material (default)
	Material material;
	// temp model
	NodeModel* tempModel = nullptr;

	Scene(Camera*);
	~Scene(void);

	void loadModel(std::string modelName);

	void renderSetup(void);
	void render(void);
};