#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


int main()
{
	//Intialize our OpenGL library we also have to state what version we are using
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Making the window of our OpenGl project and leaving a error if it fails
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << "\n";
		glfwTerminate();
		return -1;

	}
	glfwMakeContextCurrent(window);

	//Making sure GLAD loads since we need it for OpenGL pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << "\n";
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float triangle[] = {
		// Triangle        // RGB
	   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// left
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,// right
		0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f// top
	};

	// Our vertex shader
	const char* vertexShaderSource = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 ourColor;\n"
		"uniform vec2 offset;\n"
		"void main()\n"
		"{\n"
		" vec3 newPos = aPos + vec3(offset, 0.0);"
		" gl_Position = vec4(newPos, 1.0);\n"
		" ourColor = aColor;\n"
		"}\n";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	// Frag shader
	const char* fragmentShader = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		" FragColor = vec4(ourColor, 1.0);\n"
		"}\n";

	// Shader assignment and delete shader at the end of the assignment
	unsigned int fragmentShader1;
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShader1);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader1);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader1);

	// VAO AND VBO assignment
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO AND VBO for Triangle 
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	// Vertex pointer for vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);
	// Vertex pointer for colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// variables for triangle to move
	
	float offsetX = 0.0f;
	float speed = 0.5f;
	float lastTime = glfwGetTime();

	//Render loop
	while (!glfwWindowShouldClose(window))
	{
		//Input
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Time logic
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Move the tirangle to the right
		offsetX += speed * deltaTime;

		//wrap Around when off screen
		if (offsetX > 1.0f)
			offsetX = -1.0f;

		// Call Shader Program
		glUseProgram(shaderProgram);
        // Set the uniform
		int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
		glUniform2f(offsetLocation, offsetX, 0.0f);
		// Render the triangle
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Draw ghost copy if near the right edge
		if (offsetX > 0.5f)
		{
			glUniform2f(offsetLocation, offsetX - 2.0f, 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		//Draw ghost copy if near left edge
		if (offsetX < -0.5f)
		{
			glUniform2f(offsetLocation, offsetX + 2.0f, 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		//check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;

}