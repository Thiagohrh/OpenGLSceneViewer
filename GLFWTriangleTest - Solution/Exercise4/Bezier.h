#pragma once

// GLEW e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM
#include <utilities/glm/glm.hpp>
#include <utilities/glm/gtc/matrix_transform.hpp>
#include <utilities/glm/gtc/type_ptr.hpp>

#include <vector>

using namespace std;

class Bezier
{
public:
	Bezier();
	~Bezier();
	void genCurve(int resolution);
	void draw();
	void addPoint(glm::vec3 p);
protected:
	vector <glm::vec3> points; //pontos de controle
	vector <glm::vec3> curve; //pontos da curva gerada
	glm::mat4 M;
	GLuint VAO, VBO, cVBO;
};

