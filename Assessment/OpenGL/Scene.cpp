#include "Scene.h"

Scene::Scene(Camera* camera) : camera(camera) {

	// creating shader program
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("Resources\\Shaders\\vert.shader"),
		string("Resources\\Shaders\\frag.shader"),&basicShader);

	// Get unifom locations in shader
	uLightPosition = glGetUniformLocation(basicShader, "lightPosition");
	uLightAmbient = glGetUniformLocation(basicShader, "lightAmbient");
	uLightDiffuse = glGetUniformLocation(basicShader, "lightDiffuse");
	uLightAttenuation = glGetUniformLocation(basicShader, "lightAttenuation");
	uEyePos = glGetUniformLocation(basicShader, "eyePos");

	// Get material unifom locations in shader
	uMatAmbient = glGetUniformLocation(basicShader, "matAmbient");
	uMatDiffuse = glGetUniformLocation(basicShader, "matDiffuse");
	uMatSpecularCol = glGetUniformLocation(basicShader, "matSpecularColour");
	uMatSpecularExp = glGetUniformLocation(basicShader, "matSpecularExponent");
}

Scene::~Scene(void) {
	delete tempModel;
	tempModel = nullptr;
}

void Scene::loadModel(std::string modelName) {
	if (tempModel != nullptr)
		delete tempModel;

	if (modelName == "House")
		tempModel = new NodeModel("Resources\\Models\\House\\house.obj", "Resources\\Models\\House\\normal.png");
	else if (modelName == "Ground")
		tempModel = new NodeModel("Resources\\Models\\Ground\\ground.obj", "Resources\\Models\\Ground\\normal.jpg");
	else if (modelName == "Rune")
		tempModel = new NodeModel("Resources\\Models\\Rune\\rune.obj", "Resources\\Models\\Rune\\normal.jpg");
	else if (modelName == "Pillar")
		tempModel = new NodeModel("Resources\\Models\\Pillar\\new_pillar.obj", "Resources\\Models\\Pillar\\normal.png");
	else if (modelName == "Crystal")
		tempModel = new NodeModel("Resources\\Models\\Crystal\\new_crystal.obj", "Resources\\Models\\Crystal\\normal.jpg");
	else {
		tempModel = nullptr;
		std::cout << "model not found" << std::endl;
	}
}

void Scene::renderSetup(void) {
	model = glm::mat4(1.0);
	view = camera->getViewMatrix();
	projection = camera->getProjectionMatrix();
	eyePos = camera->getCameraPosition();

	glUseProgram(basicShader);

	glUniform4fv(uLightPosition, 1, (GLfloat*)&light.position);
	glUniform4fv(uLightDiffuse, 1, (GLfloat*)&light.diffuse);
	glUniform4fv(uLightAmbient, 1, (GLfloat*)&light.ambient);
	glUniform3fv(uLightAttenuation, 1, (GLfloat*)&light.attenuation);
	glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);

	//Pass material data
	glUniform4fv(uMatAmbient, 1, (GLfloat*)&material.ambient);
	glUniform4fv(uMatDiffuse, 1, (GLfloat*)&material.diffuse);
	glUniform4fv(uMatSpecularCol, 1, (GLfloat*)&material.specularCol);
	glUniform1fv(uMatSpecularExp, 1, (GLfloat*)&material.specularExp);

	glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Scene::render(void) { 
	if (tempModel != nullptr) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(tempModel->transform.position[0], tempModel->transform.position[1], tempModel->transform.position[2])) *
			glm::rotate(glm::mat4(1.0f), glm::radians(tempModel->transform.rotation[0]), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(tempModel->transform.rotation[1]), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(tempModel->transform.rotation[2]), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(tempModel->transform.scale[0], tempModel->transform.scale[1], tempModel->transform.scale[2]));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

		tempModel->model.draw(basicShader);
	}
}