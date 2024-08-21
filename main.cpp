#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
	//Initialize GLFW library
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "RenderSimple Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// GLAD:: Load opengl fuction
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		printf("Failed to initialize OpenGL context!\n");
		return -1;
	}

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}