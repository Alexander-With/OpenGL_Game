#include <stdio.h>
#include <cmath>
#include <vector>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void windowCloseCallback(GLFWwindow* window);
void processInput(GLFWwindow* window);

void createRectObject();
void writeRect(int shaderProgram, int gridSize);

void createLineObject();
void writeLine(int shaderProgram, int gridSize);

const char* vertexShaderSource = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"layout (location = 2) in vec2 aTexCoord;\n"
	"out vec3 color;\n"
	"out vec2 texCoord;\n"
	"uniform mat4 transform;\n"
	"void main(){\n"
	"	gl_Position = transform * vec4(aPos, 1.0);\n"
	"	color = aColor;\n"
	"	texCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
	"}\n"
;

const char* fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 color;\n"
	"in vec2 texCoord;\n"
	"uniform sampler2D tex;\n"
	"void main(){\n"
	"	FragColor = vec4(color, 1.0f);\n"
	"	FragColor = texture(tex, texCoord);\n"
	"}\n"
;

unsigned int VAO, VBO, EBO;
unsigned int VAOgrid, VBOgrid, EBOgrid;
unsigned int VAOline, VBOline, EBOline;

unsigned int texture;
int gridSize = 10;

bool rerun = 1;
bool rotate = 0;

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   
#endif
	
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if(window == NULL){
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		printf("Failed to initialize GLAD");
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowCloseCallback(window, windowCloseCallback);

	// OpenGL Erorr Variables
	int shaderSuccess;
	char infoLog[512];

	// OpenGL Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderSuccess);
	if(!shaderSuccess){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
		glfwTerminate();
		return -1;
	}

	// OpenGL Fragment Shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderSuccess);
	if(!shaderSuccess){
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
		glfwTerminate();
		return -1;
	}

	// Shader Linking
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderSuccess);
	if(!shaderSuccess){
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
		glfwTerminate();
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//------------------------------------------------------------------------ 

	// // Vertex Objects
	createRectObject();
	createLineObject();

	// writeRect(shaderProgram, gridSize);
	writeLine(shaderProgram, gridSize);

	float timeOld = 0;
	float timeValue = 0;

	// Render Loop
	while(!glfwWindowShouldClose(window)){
		// Input Handler
		processInput(window);

		// Render
		glClearColor(0.1f, 0.1f, 0.33f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		timeOld = timeValue;
		timeValue = glfwGetTime();

		glUseProgram(shaderProgram);
		glm::mat4 trans = glm::mat4(1.0f);
		if(rotate){
			trans = glm::rotate(trans, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::rotate(trans, glm::radians(-60.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		}
		trans = glm::translate(trans, glm::vec3(-0.6f, -0.55f, 0.0f));
		trans = glm::scale(trans, glm::vec3(0.15f, 0.15f, 0.0f));

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		

		if(rerun){
			printf("Remapping graphics data...\n");
			writeRect(shaderProgram, gridSize);
			rerun = 0;
		}


		// Draw Grid Tiles
		glBindVertexArray(VAOgrid);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, 6*gridSize*gridSize, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Draw Grid Lines
		glBindVertexArray(VAOline);
		glDrawElements(GL_LINES, 4*gridSize*gridSize, GL_UNSIGNED_INT, 0);

		// Events and Swap
		glfwSwapBuffers(window);
		glfwPollEvents();

		// printf("%f\n", 1/(timeValue - timeOld));
		glBindVertexArray(0);
	}

	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

void windowCloseCallback(GLFWwindow* window){
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool polymode = 0;
int counterPolymode = 0;
int counterRotate = 0;

void processInput(GLFWwindow* window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		windowCloseCallback(window);
	}

	if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS){
		counterPolymode++;
		if(polymode && counterPolymode == 1){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			polymode = !polymode;
		}
		else if(!polymode  && counterPolymode == 1){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			polymode = !polymode;
		}
	}
	if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE){
		counterPolymode = 0;
	}

	if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS){
		rerun = 1;
	}

	if(glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS){
		counterRotate++;
		if(rotate && counterRotate == 1){
			rotate = !rotate;
		}
		else if(!rotate  && counterRotate == 1){
			rotate = !rotate;
		}
	}
	if(glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE){
		counterRotate = 0;
	}
}

void createRectObject(){
	glGenVertexArrays(1, &VAOgrid);
	glGenBuffers(1, &VBOgrid);
	glGenBuffers(1, &EBOgrid);

	// // OpenGL Object
	glBindVertexArray(VAOgrid);

	glBindBuffer(GL_ARRAY_BUFFER, VBOgrid);
	glBufferData(GL_ARRAY_BUFFER, gridSize * gridSize * 4 * 8 * sizeof(float), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOgrid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridSize * gridSize * 6 * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// OpenGL Texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// Texture Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load Image
	int width, height, nrChannels;
	unsigned char *data = stbi_load("../textures/grass02.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture");
	}
	stbi_image_free(data);

	glBindVertexArray(0);
}

void writeRect(int shaderProgram, int gridSize){
	glBindVertexArray(VAOgrid);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBOgrid);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBOgrid);

	glGenBuffers(1, &VBOgrid);
	glGenBuffers(1, &EBOgrid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOgrid);
	glBufferData(GL_ARRAY_BUFFER, gridSize*gridSize*4*8*sizeof(float), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOgrid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridSize*gridSize*6*sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	char *verticesPtr = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	char *indicesPtr = (char*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	for(int y = 0; y < gridSize; y++){
		for(int x = 0; x < gridSize; x++){
			unsigned int square = (gridSize*y+x);
			float colorRed = (float)rand()/RAND_MAX;
			float colorGreen = (float)rand()/RAND_MAX;
			float colorBlue = (float)rand()/RAND_MAX;
			float vertices[] = {
				0.5f+x, 0.5f+y, 0.0f,		colorRed, colorGreen, colorBlue,	1.0f, 1.0f,
				0.5f+x, -0.5f+y, 0.0f,		colorRed, colorGreen, colorBlue,	1.0f, 0.0f,

				-0.5f+x, -0.5f+y, 0.0f,		colorRed, colorGreen, colorBlue,	0.0f, 0.0f,
				-0.5f+x, 0.5f+y, 0.0f,		colorRed, colorGreen, colorBlue,	0.0f, 1.0f
			};

			unsigned int indices[] = {
				0+square*4, 1+square*4, 3+square*4,
				1+square*4, 2+square*4, 3+square*4
			};

			memcpy(verticesPtr, vertices, sizeof(vertices));
			verticesPtr += sizeof(vertices);

			memcpy(indicesPtr, indices, sizeof(indices));
			indicesPtr += sizeof(indices);
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindVertexArray(0);
}

void createLineObject(){
	glGenVertexArrays(1, &VAOline);
	glGenBuffers(1, &VBOline);
	glGenBuffers(1, &EBOline);

	// // OpenGL Object
	glBindVertexArray(VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, VBOline);
	glBufferData(GL_ARRAY_BUFFER, gridSize*gridSize*4*6*sizeof(float), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOline);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridSize*gridSize*4*sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void writeLine(int shaderProgram, int gridSize){
	glBindVertexArray(VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBOline);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBOline);

	glGenBuffers(1, &VBOline);
	glGenBuffers(1, &EBOline);
	glBindBuffer(GL_ARRAY_BUFFER, VBOline);
	glBufferData(GL_ARRAY_BUFFER, gridSize*gridSize*4*6*sizeof(float), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOline);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridSize*gridSize*4*sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));

	char *verticesPtr = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	char *indicesPtr = (char*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	for(int y = 0; y < gridSize; y++){
		for(int x = 0; x < gridSize; x++){
			unsigned int square = (gridSize*y+x);
			float vertices[] = {
				0.5f+x, 0.5f+y, 0.0f,		0.0f, 0.0f, 0.0f,
				0.5f+x, -0.5f+y, 0.0f,		0.0f, 0.0f, 0.0f
			};

			unsigned int indices[] = {
				0+square*2, 1+square*2
			};

			memcpy(verticesPtr, vertices, sizeof(vertices));
			verticesPtr += sizeof(vertices);

			memcpy(indicesPtr, indices, sizeof(indices));
			indicesPtr += sizeof(indices);
		}
	}

	for(int y = 0; y < gridSize; y++){
		for(int x = 0; x < gridSize; x++){
			unsigned int square = (gridSize*y+x);
			float vertices[] = {
				0.5f+x, 0.5f+y, 0.0f,		0.0f, 0.0f, 0.0f,
				-0.5f+x, 0.5f+y, 0.0f,		0.0f, 0.0f, 0.0f
			};

			unsigned int indices[] = {
				(2*gridSize*gridSize)+0+square*2, (2*gridSize*gridSize)+1+square*2
			};

			memcpy(verticesPtr, vertices, sizeof(vertices));
			verticesPtr += sizeof(vertices);

			memcpy(indicesPtr, indices, sizeof(indices));
			indicesPtr += sizeof(indices);
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindVertexArray(0);
}