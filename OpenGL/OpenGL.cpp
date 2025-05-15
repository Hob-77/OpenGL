#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>

// Window size constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Input processing
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	// Viewport and resize callback
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Triangle data (position + color)
	float triangle[] = {
		// Position         // Color
		-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // right
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top
	};

	// Vertex shader
	const char* vertexShaderSource = R"(
		#version 460 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aColor;
		out vec3 ourColor;
		uniform vec2 offset;
		void main()
		{
			vec3 newPos = aPos + vec3(offset, 0.0);
			gl_Position = vec4(newPos, 1.0);
			ourColor = aColor;
		}
	)";

	// Compile vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	// Fragment shader
	const char* fragmentShaderSource = R"(
		#version 460 core
		out vec4 FragColor;
		in vec3 ourColor;
		void main()
		{
			FragColor = vec4(ourColor, 1.0);
		}
	)";

	// Compile fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// Link shaders into a program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Vertex Array and Buffer setup
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Offset uniform (only need to get once)
	glUseProgram(shaderProgram);
	int offsetLocation = glGetUniformLocation(shaderProgram, "offset");

	// Movement variables
	float offsetX = 0.0f;
	float speed = 0.5f;
	double lastTime = glfwGetTime();
	const double targetFPS = 60.0;
	const double targetFrameTime = 1.0 / targetFPS;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Timing
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Update triangle position
		offsetX += speed * static_cast<float>(deltaTime);
		if (offsetX >= 1.0f)
			offsetX = -1.0f;

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		// Main triangle
		glUniform2f(offsetLocation, offsetX, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Ghost triangles
		if (offsetX > 0.5f)
		{
			glUniform2f(offsetLocation, offsetX - 2.0f, 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else if (offsetX < -0.5f)
		{
			glUniform2f(offsetLocation, offsetX + 2.0f, 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Frame limiting
		double frameEndTime = glfwGetTime();
		double frameDuration = frameEndTime - currentTime;
		if (frameDuration < targetFrameTime)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - frameDuration));
		}
	}

	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// End
	glfwTerminate();
	return 0;
}
