#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>


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
	/*
	// To show current directory we are working with for shader path error
	std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;*/

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

	glEnable(GL_DEPTH_TEST);

	// Viewport and resize callback
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader("vertex_shader.vs", "fragment_shader.fs");

	Texture texture("Images/brick.jpg");

	// Each vertex: position(x,y,z), color(r,g,b), texcoords(u,v)
	std::vector<float> vertices = {
		// Back face
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
		 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
		 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top-right
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Top-left

		// Front face
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.2f, 0.2f, 0.2f,   0.0f, 1.0f,

		// Left face
		-0.5f, -0.5f, -0.5f,   0.3f, 0.6f, 0.3f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.3f, 0.6f, 0.3f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.3f, 0.6f, 0.3f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.3f, 0.6f, 0.3f,   0.0f, 0.0f,

		// Right face
		 0.5f, -0.5f, -0.5f,   0.6f, 0.3f, 0.3f,   0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.6f, 0.3f, 0.3f,   0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.6f, 0.3f, 0.3f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.6f, 0.3f, 0.3f,   1.0f, 0.0f,

		 // Bottom face
		 -0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.6f,   0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,   0.3f, 0.3f, 0.6f,   1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,   0.3f, 0.3f, 0.6f,   1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f,   0.3f, 0.3f, 0.6f,   0.0f, 0.0f,

		 // Top face
		 -0.5f,  0.5f, -0.5f,   0.6f, 0.6f, 0.3f,   0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   0.6f, 0.6f, 0.3f,   1.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,   0.6f, 0.6f, 0.3f,   1.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f,   0.6f, 0.6f, 0.3f,   0.0f, 1.0f,
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,  2, 3, 0,          // Back face
		4, 5, 6,  6, 7, 4,          // Front face
		8, 9, 10, 10, 11, 8,        // Left face
		12, 13, 14, 14, 15, 12,     // Right face
		16, 17, 18, 18, 19, 16,     // Bottom face
		20, 21, 22, 22, 23, 20      // Top face
	};

	Mesh cube(vertices, indices);

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3)); // Camera backward

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);;

		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)glfwGetTime();
		model = glm::rotate(model, angle, glm::vec3(0, 1, 0));  // Rotate over time

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		texture.bind();
		cube.draw();

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// End
	glfwTerminate();
	return 0;
}