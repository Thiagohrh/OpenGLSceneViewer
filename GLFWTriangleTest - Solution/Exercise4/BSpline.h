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

class BSpline
{
public:
	BSpline();
	~BSpline();
	void genCurve(int resolution);
	void draw();
	void addPoint(glm::vec3 p);
	void drawControlPoints();
protected:
	vector <glm::vec3> points; //pontos de controle
	vector <glm::vec3> curve; //pontos da curva gerada
	glm::mat4 M;
	GLuint VAO, VBO, cVBO, cpVBO, cpVAO, cpcVBO; //os dos CPs poderia fazer diferente
};

