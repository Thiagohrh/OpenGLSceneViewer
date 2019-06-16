#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utilities/glm/glm.hpp>
#include <utilities/glm/gtc/matrix_transform.hpp>
#include <utilities/glm/gtc/type_ptr.hpp>

#include <utilities\Shader.h>
#include "camera.h"
#include "Model.h"


#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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

glm::mat4 model;
glm::mat4 model2;

glm::vec3 modelscale(.2f, .2f, .2f);

std::vector<glm::mat4> all_models;
int current_model = 0;

bool changingModel = false;

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

	// load models
	// -----------
	Model ourModel("models/nanosuit/nanosuit.obj");


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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


	// render loop
	// -----------
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, modelscale);	// it's a bit too big for our scene, so scale it down

	model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(1.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model2 = glm::scale(model2, modelscale);	// it's a bit too big for our scene, so scale it down

	all_models.push_back(model);
	all_models.push_back(model2);


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

		// don't forget to enable shader before setting uniforms
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);

		// material properties
		lightingShader.setFloat("material.shininess", 32.0f);

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

		// render the loaded model

		for (int i = 0; i < all_models.size(); i++)
		{
			lightingShader.setMat4("model", all_models[i]);
			ourModel.Draw(lightingShader);
		}

		glBindVertexArray(cubeVAO);
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
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
		if (current_model == 0) current_model = 1;
		else current_model = 0;
		changingModel = true;
	}
}


glm::mat4 rotate_model(float rx, float ry, float rz, float _angle)
{
	return glm::rotate(all_models[current_model], glm::radians(_angle), glm::vec3(rx, ry, rz));
}
glm::mat4 translate_model(float tx, float ty, float tz)
{
	glm::vec3 pos(tx, ty, tz);
	return glm::translate(all_models[current_model], pos);
}
glm::mat4 scale_model(float s)
{

	glm::vec3 modelscale(s, s, s);

	return glm::scale(all_models[current_model], modelscale);
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
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) // TAB Switches which model is being transformed
		SwitchModel();
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) // TAB Switches which model is being transformed
		changingModel = false;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { SwitchMode(0); } // 1 Switches transformation mode to ROTATING
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { SwitchMode(1); } // 2 Switches transformation mode to TRANSLATING
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { SwitchMode(2); } // 3 Switches transformation mode to SCALING

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(0.0f, 0.0f, 5.0f), -90.0f, 0.0f); } // 4 Go to view: FRONT
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(0.0f, 0.0f, -5.0f), 90.0f, 0.0f); } // 5 Go to view: BACK
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(-5.0f, 0.0f, 0.0f), 0.0f, 0.0f); } // 6 Go to view: LEFT
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(5.0f, 0.0f, 0.0f), 180.0f, 0.0f); } // 7 Go to view: RIGHT
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(0.0f, 5.0f, 0.0f), -90.0f, -90.0f); } // 8 Go to view: ABOVE
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(0.0f, -5.0f, 0.0f), -90.0f, 90.0f); } // 9 Go to view: BELOW
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) { camera.CameraViewpoint(glm::vec3(0.0f, 0.0f, 0.0f), -90.0f, 0.0f); } // 0 Go to view: ORIGIN

	float angle = .45f;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(0.0f, -0.5f, 0.0f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(-1.0f, 0.0f, 0.0f);
			break;
		default:
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(0.0f, 0.5f, 0.0f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(1.0f, 0.0f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(0.5f, 0.0f, 0.0f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(0.0f, -0.5f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(-0.5f, 0.0f, 0.0f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(0.0f, 0.5f, 0.0f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(0.0f, 0.0f, 0.5f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(0.0f, 0.0f, 0.5f);
			break;
		case SCALING:
			all_models[current_model] = scale_model(.999f);
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		switch (currentmode)
		{
		case ROTATING:
			all_models[current_model] = rotate_model(0.0f, 0.0f, -0.5f, angle);
			break;
		case TRANSLATING:
			all_models[current_model] = translate_model(0.0f, 0.0f, -0.5f);
			break;
		case SCALING:
			all_models[current_model] = scale_model(1.001f);
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