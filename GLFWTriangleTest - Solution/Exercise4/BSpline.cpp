#include "BSpline.h"



BSpline::BSpline()
{
	float scalar = 1.0 / 6.0;

	M = glm::mat4x4(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0
	);

	M = M * scalar;
}


BSpline::~BSpline()
{
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &cVBO);
}

void BSpline::genCurve(int resolution)
{

	float step = 1.0 / (float)resolution;

	float t = 0;

	int nControlPoints = points.size();

	for (int i = 0; i < points.size() - 4; i++)
	{
		int k = 0;
		for (float t = 0.0; k < resolution; t += step, k++)
		{
			glm::vec3 p;
			//glm::vec4 T(1, t, t*t, t*t*t);

			glm::vec4 T(t*t*t, t*t, t, 1);

			glm::mat4x3 G(points[i].x, points[i].y, points[i].z,
				points[i + 1].x, points[i + 1].y, points[i + 1].z,
				points[i + 2].x, points[i + 2].y, points[i + 2].z,
				points[i + 3].x, points[i + 3].y, points[i + 3].z);

			p = G * M * T;  //---------

			curve.push_back(p);
		}
	}

	vector <glm::vec3> colors;
	for (int i = 0; i < curve.size(); i++)
	{
		colors.push_back(glm::vec3(0, 1, 0));
	}

	//Gera os buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &cVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, curve.size() * sizeof(GLfloat) * 3, &curve[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat) * 3, &colors[0], GL_STATIC_DRAW);

	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	colors.clear();
	for (int i = 0; i < points.size(); i++)
	{
		colors.push_back(glm::vec3(0, 0, 0));
	}
	glGenVertexArrays(1, &cpVAO);
	glGenBuffers(1, &cpVBO);
	glGenBuffers(1, &cpcVBO);

	glBindVertexArray(cpVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cpVBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat) * 3, &points[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cpcVBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat) * 3, &colors[0], GL_STATIC_DRAW);

	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, cpVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glBindBuffer(GL_ARRAY_BUFFER, cpcVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

}

void BSpline::draw()
{
	glLineWidth(5);
	glPointSize(5);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, curve.size());
	glBindVertexArray(0);

}

void BSpline::addPoint(glm::vec3 p)
{
	points.push_back(p);
}

void BSpline::drawControlPoints()
{
	glLineWidth(5);
	glPointSize(10);
	glBindVertexArray(cpVAO);
	glDrawArrays(GL_POINTS, 0, points.size());
	glBindVertexArray(0);
}


