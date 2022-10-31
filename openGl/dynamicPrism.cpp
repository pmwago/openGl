#include <iostream>
#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include <array>
using namespace std;

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)

GLuint vao, ibo, points_vbo, colors_vbo, modelID, colorID;
float rotAngle = 0.0f;
int deltaTime, currentTime, lastTime = 0;

float osH = 0.0f, osV = 0.0f, scrollSpd = 0.25f;

glm::mat4 mvp, view, projection;

static unsigned int
program,
vertexShaderId,
fragmentShaderId;


static float R = 2.0; 
static float X = 1.0; 
static float Y = 0.0;
const int MaxNumVertices = 500; 
static int numVertices = 3;
#define PI 3.14159265358979324

float theta = 0.0f;


std::array<glm::vec3, MaxNumVertices> vertices = {};
std::array<glm::vec3, MaxNumVertices> colors = {};

GLfloat shape_vertices[MaxNumVertices][3] = { 0 };
GLfloat shape_colors[MaxNumVertices][3] = { 0 };

static int isWire = 0; 

void createModel(int n)
{
	theta = 0.0f;
	for (int i = 0; i < n; ++i)
	{

		vertices[i] = glm::vec3(X + R * cos(theta), Y + R * sin(theta), 0.0);
		colors[i] = glm::vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);

		theta += 2 * PI / n;
	}

	for (int i = 0; i < n; ++i) {

		shape_vertices[i][0] = vertices[i][0];
		shape_vertices[i][1] = vertices[i][1];
		shape_vertices[i][2] = vertices[i][2];

		shape_colors[i][0] = colors[i][0];
		shape_colors[i][1] = colors[i][1];
		shape_colors[i][2] = colors[i][2];
	}
}



void init(void)
{
	vertexShaderId = setShader((char*)"vertex", (char*)"cubeProjection.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cubeProjection.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "mvp");
	colorID = glGetAttribLocation(program, "vertex_color");



	view = glm::lookAt(
		glm::vec3(0, 0, 1),	
		glm::vec3(0, 0, 0),		
		glm::vec3(0, 1, 0)	
	);


	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	createModel(numVertices);

	
	glEnable(GL_DEPTH_TEST);

	glClearColor(12.0, 13.0, 12.0,13,0);


void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	mvp = projection * view * Model;
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &mvp[0][0]);
}

void display(void)
{
	currentTime = glutGet(GLUT_ELAPSED_TIME); // Gets elapsed time in milliseconds.
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;


	theta += 0.1;
	if (theta > 2.0f * 3.1415f) theta -= 2.0f * 3.1415f;	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -4.0f, 4.0f);

	
	createModel(numVertices);

	glBindVertexArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape_vertices), shape_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape_colors), shape_colors, GL_STATIC_DRAW);


	transformObject(1.0f, X_AXIS, rotAngle = 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	
	if (isWire)
	{
		glDrawArrays(GL_LINE_LOOP, 0, numVertices);
	}
	else
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
	}


	glBindVertexArray(0); 

	glutSwapBuffers(); 

void idle()
{
	glutPostRedisplay();
}

void timer(int id)
{
	glutPostRedisplay();
	glutTimerFunc(33, timer, 0);
}

void keyDown(unsigned char key, int x, int y)
{
	
	switch (key)
	{
	case '+':
		numVertices++;
		
		createModel(numVertices);
		glutPostRedisplay();
		break;
	case '-':
		numVertices--;
		createModel(numVertices);
		glutPostRedisplay();
		break;
	case ' ':
		if (isWire == 0) isWire = 1;
		else isWire = 0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}



void mouseMove(int x, int y)
{
	cout << "Mouse pos: " << x << "," << y << endl;
}

void mouseDown(int btn, int state, int x, int y)
{
	cout << "Clicked: " << (btn == 0 ? "left " : "right ") << (state == 0 ? "down " : "up ") <<
		"at " << x << "," << y << endl;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Dynamic Prism");
	 glutSetWindowTitle("Name: Abu Bakkar Registration number: 101386942");

	glewInit();	
	init();

	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutTimerFunc(33, timer, 0);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutPassiveMotionFunc(mouseMove);
	
}
