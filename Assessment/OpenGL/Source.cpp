// imgui includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Includes.h"
#include "Scene.h"
#include <stdio.h>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Camera_settings camera_settings{ 1280, 720, 0.1, 100.0 };
Camera camera(camera_settings, glm::vec3(0.0, 1.3, 4.0));

Timer timer;

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

// boolean for gui window hover/interaction 
bool imguiConsumingInput;

const int FSAA_Multiplier = 8;

std::string FSAA_state = "Disabled";

int main(void) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, FSAA_Multiplier);


	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "3D Viewer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSwapInterval(1);		
	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_CULL_FACE);		
	glFrontFace(GL_CCW);		

	// disable multisampling
	glDisable(GL_MULTISAMPLE); 

	// initialising scene
	Scene scene(&camera);

	// imgui initialise
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// file menu options
	bool showModelLoader = false;
	bool showModelTransform = false;
	bool showLightSetting = false;

	// model directory
	std::string path = "./Resources/Models"; 


	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		timer.tick();

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// render scene setup
		scene.renderSetup();

		scene.render();

		// imgui section
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		// checking if imgui is being hovered or interacted with
		imguiConsumingInput = ImGui::IsWindowHovered() || ImGui::IsAnyItemActive();

		// file menu section
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Model Loader")) {
					showModelLoader = true;
				}
				if (ImGui::MenuItem("Clear Scene")) {
					delete scene.tempModel;
					scene.tempModel = nullptr;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		// edit menu section
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Light Settings")) {
					showLightSetting = true;
				}
				if (ImGui::MenuItem("Transformations")) {
					showModelTransform = true;
				}
				if (ImGui::MenuItem(static_cast<std::string>("FSAA x8: " + FSAA_state).c_str())) {
					if (FSAA_state == "Disabled") {
						glEnable(GL_MULTISAMPLE); //Enable multisampling.
						FSAA_state = "Enabled";
					}
					else {
						glDisable(GL_MULTISAMPLE); //Disable multisampling.
						FSAA_state = "Disabled";
					}

				}


				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// fps overlay
		ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize); // Set size to full display
		if (ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
			ImGui::Text("FPS: %d", static_cast<int>(timer.averageFPS()));
			ImGui::Text("FSAA x8: %s", FSAA_state.c_str());
		}
		ImGui::End();

		// model loader
		if (showModelLoader) {
			// setting window size
			ImGui::SetNextWindowPos(ImVec2(width - 300, 20));

			ImGui::SetNextWindowSize(ImVec2(300, 200));
			if (ImGui::Begin("Model Loader", &showModelLoader)) {
				ImGui::Text("Select a model to load:");

				// reads the directory and show the folders
				for (const auto& entry : std::filesystem::directory_iterator(path)) {
					if (ImGui::MenuItem(entry.path().string().c_str())) {
						scene.loadModel(entry.path().filename().string());
					}
				}
			}
			ImGui::End();
		}

		// light settings
		if (showLightSetting) {
			// setting window size
			ImGui::SetNextWindowSize(ImVec2(350, 130));
			if (ImGui::Begin("Light Settings", &showLightSetting)) {
				//ImGui::Text("Light Position");

				ImGui::SliderFloat3("Position (xyz)", &scene.light.position[0], -5.0f, 5.0f);
				if (ImGui::Button("Reset Position")) {
					scene.light.position[0] = 0.0f;
					scene.light.position[1] = 1.0f;
					scene.light.position[2] = 0.0f;
				}

				ImGui::SliderFloat("Attenuation", &scene.light.attenuation[0], 0.1f, 5.0f);
				scene.light.attenuation[1] = scene.light.attenuation[0];
				scene.light.attenuation[2] = scene.light.attenuation[0];
				if (ImGui::Button("Reset Attenuation")) {
					scene.light.attenuation[0] = 0.35f;
					scene.light.attenuation[1] = 0.35f;
					scene.light.attenuation[2] = 0.35f;
				}
			}
			ImGui::End();
		}

		// model transfrom
		if (showModelTransform) {
			// setting window size
			ImGui::SetNextWindowSize(ImVec2(350, 190));
			if (ImGui::Begin("Transformation", &showModelTransform)) {
				
				if (scene.tempModel != nullptr) {
					ImGui::Text("Transformations");
					ImGui::SliderFloat3("Position (xyz)", &scene.tempModel->transform.position[0], -10.0f, 10.0f);
					if (ImGui::Button("Reset Position")) {
						scene.tempModel->transform.position[0] = 0.0f;
						scene.tempModel->transform.position[1] = 0.0f;
						scene.tempModel->transform.position[2] = 0.0f;
					}

					ImGui::SliderFloat3("Rotation (xyz)", &scene.tempModel->transform.rotation[0], 0.0f, 360.0f);
					if (ImGui::Button("Reset Rotation")) {
						scene.tempModel->transform.rotation[0] = 0.0f;
						scene.tempModel->transform.rotation[1] = 0.0f;
						scene.tempModel->transform.rotation[2] = 0.0f;
					}

					ImGui::SliderFloat("Scale", &scene.tempModel->transform.scale[0], 0.1f, 1.0f);
					scene.tempModel->transform.scale[1] = scene.tempModel->transform.scale[0];
					scene.tempModel->transform.scale[2] = scene.tempModel->transform.scale[0];
					if (ImGui::Button("Reset Scale")) {
						scene.tempModel->transform.scale[0] = 0.1f;
						scene.tempModel->transform.scale[1] = 0.1f;
						scene.tempModel->transform.scale[2] = 0.1f;
					}
				}

			}
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	// OpenGL camera control (only if ImGui is not consuming input)
	if (!imguiConsumingInput) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			camera.processMouseMovement(xoffset, yoffset);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll(yoffset);
}