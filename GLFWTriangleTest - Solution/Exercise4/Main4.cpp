#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utilities/glm/glm.hpp>
#include <utilities/glm/gtc/matrix_transform.hpp>
#include <utilities/glm/gtc/type_ptr.hpp>

#include <utilities\Shader.h>
#include "camera.h"
#include "Model.h"
#include "SceneFromFile.h"

#include <iostream>
#include <vector>

//Classe de curvas
#include "BSpline.h"
#include "Bezier.h"
#include "Catmul.h"


// declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
void LoadSceneCGS(string directory);
void SwitchCamera();
void SwitchCamera(int id);
void ResetToCamera();
void SwitchLight();
void ModelCurveMove(float speed);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

struct Transformations
{
	bool rotate_clock = false;
	bool rotate_counter_clock = false;
	bool rotate_x_up = false;
	bool rotate_x_down = false;
	bool rotate_z_positive = false;
	bool rotate_z_negative = false;

	bool scale_up = false;
	bool scale_down = false;

	bool translate_right = false;
	bool translate_left = false;
	bool translate_back = false;
	bool translate_forward = false;
	bool translate_up = false;
	bool translate_down = false;
};

enum transformation_mode { ROTATING, TRANSLATING, SCALING };
int currentmode = 0;

// ------------------
// models variables
std::vector<glm::mat4> modelMat4;
std::vector<Model> modelObj;
int modelsCount = 0;
int current_model = 0;
bool changingModel = false;

// ------------------
// cameras variables
std::vector<CameraInfo> cameraPositions;
int camerasCount = 0;
int currentCamera = 0;
bool changingCamera = false;

// ------------------
// lights variables
std::vector<LightInfo> lightPositions;
int lightsCount = 0;
int currentLight = 0;
bool changingLight = false;

// ------------------
// scene variables
SceneFromFile currentScene;
string sceneDirectory = "scenes/sceneTest.cgs"; // Sets which scene will be loaded


// ------------------
// animations variables
vector<glm::vec3> animationpoints;
int animationSize;
int currentAnimation;

void LoadSceneCGS(string directory)
{
	currentScene.LoadFile(directory);
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader lightingShader("shaders/light_casters.vs", "shaders/light_casters.fs");
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");

	LoadSceneCGS(sceneDirectory);

	for each (ModelInfo var in currentScene.GetModels()) // Gathers all models' information
	{
		modelObj.push_back(var.GetModel());

		glm::vec3 translation(var.xpos, var.ypos, var.zpos);
		glm::vec3 rotation(var.xrot, var.yrot, var.zrot);
		glm::vec3 scale(var.xsc, var.ysc, var.zsc);

		glm::mat4 modelmat = glm::mat4(1.0f);
		modelmat = glm::translate(modelmat, translation);
		modelmat = glm::rotate(modelmat, glm::radians(var.xrot), glm::vec3(1.0f, 0.0f, 0.0f));
		modelmat = glm::rotate(modelmat, glm::radians(var.yrot), glm::vec3(0.0f, 1.0f, 0.0f));
		modelmat = glm::rotate(modelmat, glm::radians(var.zrot), glm::vec3(0.0f, 0.0f, 1.0f));
		modelmat = glm::scale(modelmat, scale);

		modelMat4.push_back(modelmat);

		modelsCount++;

		cout << "Getting model: " << modelsCount << endl;
	}

	for each (CameraInfo var in currentScene.GetCameras()) // Gathers all cameras' information
	{
		cameraPositions.push_back(var);
		camerasCount++;
	}

	for each (LightInfo var in currentScene.GetLights()) // Gathers all lights' information
	{
		lightPositions.push_back(var);
		lightsCount++;
	}

	SwitchCamera(0);

	//cout << "Models count: " << to_string(modelsCount) << endl;
	//cout << "Cameras count: " << to_string(camerasCount) << endl;
	//cout << "Camera positions size: " << to_string(cameraPositions.size()) << endl;
	//cout << "Lights count: " << to_string(lightsCount) << endl;

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	float controlPoints[] = {
		0.8 ,-1.88, 0.79,-1.74, 1.51,-1.36, 1.70,-0.80, 1.94, 0.07,
		1.76, 0.96, 1.47, 1.65, 0.96, 1.96, 0.0 , 2.07,-0.74, 2.18,
		-1.42, 2.08,-1.89, 1.63,-2.01, 0.87,-1.68, 0.38,-1.05, 0.26,
		-0.45,-0.15,-0.43,-0.50,-0.68,-0.79,-0.80,-1.27,-0.70,-1.56,
		-0.62,-1.92,-0.35,-1.92, 0.03,-1.90, 0.25,-1.90
	};

	BSpline bspline;
	Bezier bezier;
	Catmul catmul;

	for (int i = 0; i < 48; i += 2)
	{
		bspline.addPoint(glm::vec3(controlPoints[i], controlPoints[i + 1], 0.0));
		bezier.addPoint(glm::vec3(controlPoints[i], controlPoints[i + 1], 0.0));
		catmul.addPoint(glm::vec3(controlPoints[i], controlPoints[i + 1], 0.0));
	}

	bspline.genCurve(100);
	bezier.genCurve(100);
	catmul.genCurve(100);

	animationpoints = bspline.GetPointsFromCurve();
	animationSize = animationpoints.size();

	cout << "Animation size: " << animationSize << endl;

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("images/container2.png");
	unsigned int specularMap = loadTexture("images/container2_specular.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light stuff
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 30.0f);

		LightInfo lightInfo = lightPositions[currentLight];
		glm::vec3 lightPosition = glm::vec3(lightInfo.xpos, lightInfo.ypos, lightInfo.zpos);

		lightingShader.setVec3("light.position", lightPosition);
		lightingShader.setVec3("light.ambient", .6f, .6f, .6f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// render all models
		for (int i = 0; i < modelsCount; i++)
		{
			lightingShader.setMat4("model", modelMat4[i]);
			modelObj[i].Draw(lightingShader);
		}

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		glBindVertexArray(lightVAO);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		bspline.draw();
		//ModelCurveMove();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void SwitchMode(int _mode_choice)
{
	string output = "";
	switch (_mode_choice)
	{
	case 0:
		currentmode = ROTATING;
		output = "ROTATING";
		break;
	case 1:
		currentmode = TRANSLATING;
		output = "TRANSLATING";
		break;
	case 2:
		currentmode = SCALING;
		output = "SCALING";
		break;
	default:
		break;
	}
	cout << "Switching transforming mode to " << output << endl;
}

void SwitchModel() {
	if (!changingModel)
	{
		changingModel = true;
		current_model++;
		if (current_model >= modelsCount) current_model = 0;
		cout << "Changed to model: " << current_model << endl;
	}
}

void SwitchLight()
{
	changingLight = true;
	currentLight++;
	if (currentLight >= lightsCount) currentLight = 0;

	cout << "Changed to light: " << currentLight << endl;

	LightInfo li = lightPositions[currentLight];

	glm::vec3 pos = glm::vec3(li.xpos, li.ypos, li.zpos);

}

glm::mat4 rotate_model(float rx, float ry, float rz, float _angle)
{
	return glm::rotate(modelMat4[current_model], glm::radians(_angle), glm::vec3(rx, ry, rz));
}
glm::mat4 translate_model(float tx, float ty, float tz)
{
	cout << "translating model" << endl;
	glm::vec3 pos(tx, ty, tz);
	return glm::translate(modelMat4[current_model], pos);
}
glm::mat4 scale_model(float s)
{

	glm::vec3 modelscale(s, s, s);

	return glm::scale(modelMat4[current_model], modelscale);
}

void ModelCurveMove(float speed)
{
	currentAnimation += speed;
	if (currentAnimation >= animationSize) currentAnimation = 0;
	glm::vec3 a = animationpoints[currentAnimation];
	modelMat4[current_model] = translate_model(a.x, a.y, a.z);
}

void SwitchCamera()
{
	changingCamera = true;
	currentCamera++;
	if (currentCamera >= camerasCount) currentCamera = 0;

	cout << "Changed to camera: " << currentCamera << endl;

	CameraInfo ci = cameraPositions[currentCamera];

	glm::vec3 pos = glm::vec3(ci.xpos, ci.ypos,ci.zpos);
	float yaw = ci.yaw; float pitch = ci.pitch;

	camera.CameraViewpoint(pos, yaw, pitch);
}

void SwitchCamera(int id)
{
	changingCamera = true;
	if (id < camerasCount) {
		CameraInfo ci = cameraPositions[currentCamera];

		glm::vec3 pos = glm::vec3(ci.xpos, ci.ypos, ci.zpos);
		float yaw = ci.yaw; float pitch = ci.pitch;

		camera.CameraViewpoint(pos, yaw, pitch);
		cout << "Changed to camera: " << currentCamera << endl;
	}
	else {
		cout << "There isn't a camera with the id: " << id << endl;
	}

}

void ResetToCamera()
{
	SwitchCamera(currentCamera);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // W Moves forward
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // S Moves backward
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // A Moves leftward
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // D Moves rightward
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // SPACE Moves upwards
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // LEFT SHIFT Moves downwards
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) // TAB Switches which model is being transformed
		SwitchModel();
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		changingModel = false;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { SwitchMode(0); } // 1 Switches transformation mode to ROTATING
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { SwitchMode(1); } // 2 Switches transformation mode to TRANSLATING
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { SwitchMode(2); } // 3 Switches transformation mode to SCALING

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && camerasCount > 1 && !changingCamera) { SwitchCamera(); } // C Switches Camera
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && camerasCount > 1) { changingCamera = false; }

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && lightsCount > 1 && !changingLight) { SwitchLight(); } // L Switches Light
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && lightsCount > 1) { changingLight = false; }


	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { ModelCurveMove(5); } // L Switches Light

	float angle = .45f;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(0.0f, -0.5f, 0.0f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(-1.0f, 0.0f, 0.0f);
			break;
		default:
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(0.0f, 0.5f, 0.0f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(1.0f, 0.0f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(0.5f, 0.0f, 0.0f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(0.0f, -0.5f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(-0.5f, 0.0f, 0.0f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(0.0f, 0.5f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(0.0f, 0.0f, 0.5f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(0.0f, 0.0f, 0.5f);
			break;
		case SCALING:
			modelMat4[current_model] = scale_model(.999f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			modelMat4[current_model] = rotate_model(0.0f, 0.0f, -0.5f, angle);
			break;
		case TRANSLATING:
			modelMat4[current_model] = translate_model(0.0f, 0.0f, -0.5f);
			break;
		case SCALING:
			modelMat4[current_model] = scale_model(1.001f);
			break;
		default:
			break;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}