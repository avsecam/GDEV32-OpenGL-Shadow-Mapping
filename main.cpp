// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath);
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource);

void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height);

struct Vertex
{
	GLfloat x, y, z;	  // Position
	GLubyte r, g, b;		// Color
	GLfloat nx, ny, nz;	// Normals
};


int main()
{
	// Initialize GLFW
	int glfwInitStatus = glfwInit();
	if (glfwInitStatus == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return 1;
	}

	// Tell GLFW that we prefer to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW that we prefer to use the modern OpenGL
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tell GLFW to create a window
	float windowWidth = 800;
	float windowHeight = 800;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Shadow Mapping 👻", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Tell GLFW to use the OpenGL context that was assigned to the window that we just created
	glfwMakeContextCurrent(window);

	// Register the callback function that handles when the framebuffer size has changed
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

	// Tell GLAD to load the OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return 1;
	}

	// vertex specification
	// 							Position									Color

	Vertex vertices[28];

	// CUBE
	// front
	vertices[0] = { -0.5f, 0.5f, 0.5f,        255, 0, 0 };
	vertices[1] = { 0.5f, 0.5f, 0.5f,         255, 0, 0 };
	vertices[2] = { 0.5f, -0.5f, 0.5f,        255, 0, 0 };
	vertices[3] = { -0.5f, -0.5f, 0.5f,       255, 0, 0 };

	// back
	vertices[4] = { 0.5f, 0.5f, -0.5f,        0, 255, 0 };
	vertices[5] = { -0.5f, 0.5f, -0.5f,       0, 255, 0 };
	vertices[6] = { -0.5f, -0.5f, -0.5f,      0, 255, 0 };
	vertices[7] = { 0.5f, -0.5f, -0.5f,     	0, 255, 0 };

	// left
	vertices[8] = { -0.5f, 0.5f, -0.5f,    	  0, 0, 255 };
	vertices[9] = { -0.5f, 0.5f, 0.5f,        0, 0, 255 };
	vertices[10] = { -0.5f, -0.5f, 0.5f,      0, 0, 255 };
	vertices[11] = { -0.5f, -0.5f, -0.5f,     0, 0, 255 };

	// right
	vertices[12] = { 0.5f, 0.5f, 0.5f,     		255, 255, 0 };
	vertices[13] = { 0.5f, 0.5f, -0.5f,       255, 255, 0 };
	vertices[14] = { 0.5f, -0.5f, -0.5f,      255, 255, 0 };
	vertices[15] = { 0.5f, -0.5f, 0.5f,      	255, 255, 0 };

	// top
	vertices[16] = { -0.5f, 0.5f, -0.5f,      255, 0, 255 };
	vertices[17] = { 0.5f, 0.5f, -0.5f,       255, 0, 255 };
	vertices[18] = { 0.5f, 0.5f, 0.5f,    	  255, 0, 255 };
	vertices[19] = { -0.5f, 0.5f, 0.5f,     	255, 0, 255 };

	// bottom
	vertices[20] = { -0.5f, -0.5f, 0.5f,      0, 255, 255 };
	vertices[21] = { 0.5f, -0.5f, 0.5f,       0, 255, 255 };
	vertices[22] = { 0.5f, -0.5f, -0.5f,      0, 255, 255 };
	vertices[23] = { -0.5f, -0.5f, -0.5f,     0, 255, 255 };
	
	// PLANE that faces upwards
	vertices[24] = { -0.5f, 0.5f, -0.5f,      250, 250, 250 };
	vertices[25] = { 0.5f, 0.5f, -0.5f,       250, 250, 250 };
	vertices[26] = { 0.5f, 0.5f, 0.5f,    	  250, 250, 250 };
	vertices[27] = { -0.5f, 0.5f, 0.5f,     	250, 250, 250 };

	// normals
	for(int i = 0; i < 28; i += 4) {
		GLfloat x0 = vertices[i].x;
		GLfloat y0 = vertices[i].y;
		GLfloat z0 = vertices[i].z;

		GLfloat x1 = vertices[i + 1].x;
		GLfloat y1 = vertices[i + 1].y;
		GLfloat z1 = vertices[i + 1].z;
		
		GLfloat x2 = vertices[i + 2].x;
		GLfloat y2 = vertices[i + 2].y;
		GLfloat z2 = vertices[i + 2].z;

		GLfloat v0x = x2 - x0;
		GLfloat v0y = y2 - y0;
		GLfloat v0z = z2 - z0;

		GLfloat v1x = x1 - x0;
		GLfloat v1y = y1 - y0;
		GLfloat v1z = z1 - z0;

		glm::vec3 normal;
		normal = glm::cross(glm::vec3(v0x, v0y, v0z), glm::vec3(v1x, v1y, v1z));

		vertices[i].nx = normal.x;
		vertices[i].ny = normal.y;
		vertices[i].nz = normal.z;
		vertices[i + 1].nx = normal.x;
		vertices[i + 1].ny = normal.y;
		vertices[i + 1].nz = normal.z;
		vertices[i + 2].nx = normal.x;
		vertices[i + 2].ny = normal.y;
		vertices[i + 2].nz = normal.z;
		vertices[i + 3].nx = normal.x;
		vertices[i + 3].ny = normal.y;
		vertices[i + 3].nz = normal.z;
	}
	
	// vertex order for EBO
	GLuint cubeIndices[] = {
		0,1,2,    0,2,3,
		4,5,6,    4,6,7,
		8,9,10,   8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	};

	GLuint planeIndices[] = {
		24,25,26, 24,26,27
	};

	// VBO setup
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO setup
	GLuint cubeEbo;
	glGenBuffers(1, &cubeEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	GLuint planeEbo;
	glGenBuffers(1, &planeEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO setup
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));
	glBindVertexArray(0);

	// FBO setup
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Depth Texture
	GLuint depthTexture;
	GLuint depthTextureWidth = 1024;
	GLuint depthTextureHeight = 1024;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthTextureWidth, depthTextureHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer incomplete...");
		return 1;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint mainShader = CreateShaderProgram("main.vsh", "main.fsh");
	GLuint depthShader = CreateShaderProgram("depth.vsh", "depth.fsh");

	// glViewport(0, 0, windowWidth, windowHeight);

	glEnable(GL_DEPTH_TEST);

	
	GLint cubeIndicesSize = sizeof(cubeIndices) / sizeof(cubeIndices[0]);
	GLint planeIndicesSize = sizeof(planeIndices) / sizeof(planeIndices[0]);

	// camera and movement
	GLfloat horizontalAngle = M_PI;
	GLfloat verticalAngle = 0.0f;
	GLfloat speed = 4.0f;
	GLfloat mouseSpeed = 0.8f;
	glm::vec3 direction, right, up;

	// starting view position
	glm::vec3 position(0.0f, 3.0f, 5.0f);
	GLdouble xpos, ypos;

	GLfloat lastTime = glfwGetTime();

	// DIRECTIONAL LIGHT
	glm::vec3 directionalLightPosition(3.0f, 3.0f, -7.0f);
	glm::vec3 directionalLightDirection(-1.0f, -1.0f, 1.0f);
	glm::vec3 directionalLightAmbient(1.0f, 1.0f, 1.0f);
	glm::vec3 directionalLightDiffuse(0.75f, 0.75f, 0.75f);
	glm::vec3 directionalLightSpecular(0.5f, 0.5f, 0.5f);
	glm::mat4 directionalLightProjectionMatrix = glm::ortho(-15.0f, 10.0f, -5.0f, 10.0f, 0.0f, 20.0f);
	glm::mat4 directionalLightViewMatrix = glm::lookAt(directionalLightPosition, directionalLightPosition + directionalLightDirection, glm::vec3(0, 1, 0));

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentTime = glfwGetTime();
		GLfloat deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//camera movement
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, (double)windowWidth / 2, (double)windowHeight / 2);

		horizontalAngle += mouseSpeed * deltaTime * float(windowWidth / 2 - xpos);
		verticalAngle += mouseSpeed * deltaTime * float(windowHeight / 2 - ypos);

		direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
		right = glm::vec3(sin(horizontalAngle - M_PI / 2.0f), 0.0f, cos(horizontalAngle - M_PI / 2.0f));
		up = glm::cross(right, direction);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position += direction * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position -= direction * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position += right * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position -= right * deltaTime * speed;
		}


		// identity matrix
		glm::mat4 iMatrix(1.0f);

		// MVP uniforms
		glm::mat4 viewMatrix = glm::lookAt(position, position + direction, up);
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), windowWidth / windowHeight, 0.1f, 100.0f);
		
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(vao);
		// SET OBJECT TRANSFORMS
		// cube
		glm::mat4 firstMatrix = glm::scale(iMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
		firstMatrix = glm::translate(firstMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
		firstMatrix = glm::rotate(firstMatrix, glm::radians(23.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 secondMatrix = glm::scale(iMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
		secondMatrix = glm::translate(secondMatrix, glm::vec3(1.5f, 0.5f, 1.5f));
		secondMatrix = glm::rotate(secondMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 thirdMatrix = glm::scale(iMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		thirdMatrix = glm::translate(thirdMatrix, glm::vec3(2.5f, 2.0f, -2.0f));
		thirdMatrix = glm::rotate(thirdMatrix, glm::radians(currentTime * 40.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 fourthMatrix = glm::scale(iMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		fourthMatrix = glm::translate(fourthMatrix, glm::vec3(5.0f, 3.5f, 5.0f));
		fourthMatrix = glm::rotate(fourthMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		fourthMatrix = glm::rotate(fourthMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 fifthMatrix = glm::scale(iMatrix, glm::vec3(0.5f, 2.0f, 0.5f));
		fifthMatrix = glm::translate(fifthMatrix, glm::vec3(-2.0f, 0.8f, -5.0f));
		fifthMatrix = glm::rotate(fifthMatrix, glm::radians(23.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		fifthMatrix = glm::rotate(fifthMatrix, glm::radians(currentTime * 60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// plane
		glm::mat4 planeMatrix = glm::scale(iMatrix, glm::vec3(10.0f, 1.0f, 10.0f));
		planeMatrix = glm::translate(planeMatrix, glm::vec3(0, -0.5f, 0));


		// FIRST PASS
		glUseProgram(depthShader);
		glViewport(0, 0, depthTextureWidth, depthTextureHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightProjection"), 1, GL_FALSE, glm::value_ptr(directionalLightProjectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightView"), 1, GL_FALSE, glm::value_ptr(directionalLightViewMatrix));

		// DRAW 📝
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(firstMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(secondMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(thirdMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(fourthMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(fifthMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEbo);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(planeMatrix));
		glDrawElements(GL_TRIANGLES, planeIndicesSize, GL_UNSIGNED_INT, 0);


		// SECOND PASS
		glUseProgram(mainShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(glGetUniformLocation(mainShader, "shadowMap"), 0);
		
		glUniform3fv(glGetUniformLocation(mainShader, "viewPosition"), 1, glm::value_ptr(position));

		// directional light uniforms
		glUniform3fv(glGetUniformLocation(mainShader, "directionalLightDirection"), 1, glm::value_ptr(directionalLightDirection));
		glUniform3fv(glGetUniformLocation(mainShader, "directionalLightAmbient"), 1, glm::value_ptr(directionalLightAmbient));
		glUniform3fv(glGetUniformLocation(mainShader, "directionalLightDiffuse"), 1, glm::value_ptr(directionalLightDiffuse));
		glUniform3fv(glGetUniformLocation(mainShader, "directionalLightSpecular"), 1, glm::value_ptr(directionalLightSpecular));
		
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "lightProjection"), 1, GL_FALSE, glm::value_ptr(directionalLightProjectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "lightView"), 1, GL_FALSE, glm::value_ptr(directionalLightViewMatrix));
		
		// DRAW AGAIN 😎
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(firstMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(secondMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(thirdMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(fourthMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(fifthMatrix));
		glDrawElements(GL_TRIANGLES, cubeIndicesSize, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEbo);
		glUniformMatrix4fv(glGetUniformLocation(mainShader, "model"), 1, GL_FALSE, glm::value_ptr(planeMatrix));
		glDrawElements(GL_TRIANGLES, planeIndicesSize, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteProgram(mainShader);
	glDeleteProgram(depthShader);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &fbo);

	glfwTerminate();

	return 0;
}

GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
	GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	// Check shader program link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "program link error: " << infoLog << std::endl;
	}

	return program;
}

GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath)
{
	std::ifstream shaderFile(shaderFilePath);
	if (shaderFile.fail())
	{
		std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
		return 0;
	}

	std::string shaderSource;
	std::string temp;
	while (std::getline(shaderFile, temp))
	{
		shaderSource += temp + "\n";
	}
	shaderFile.close();

	return CreateShaderFromSource(shaderType, shaderSource);
}

GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char* shaderSourceCStr = shaderSource.c_str();
	GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
	glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
	glCompileShader(shader);

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "shader compilation error: " << infoLog << std::endl;
	}

	return shader;
}

void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
	// Whenever the size of the framebuffer changed (due to window resizing, etc.),
	// update the dimensions of the region to the new size
	glViewport(0, 0, width, height);
}
