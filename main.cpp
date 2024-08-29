#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

// shader
const char* vertexShaderSource = R"(
#version 320 es

in vec3 aColor;
in vec2 aPosition;	

out vec3 color;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
	color = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 320 es
precision mediump float;

in vec3 color;
out vec4 FragColor;

void main() {
    FragColor = vec4(color, 1.0);
}
)";

static float vertices[] =
{
	//---位置---//  //---颜色---//
	-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
	0.0f,  0.5f,  0.0f, 0.0f, 1.0f
};

static unsigned int indices[] = { // 注意索引从0开始！
	0, 1, 2, // 第一组三角形
};

GLuint createProgram()
{
	//创建顶点着色器
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//创建片元着色器
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//创建程序
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	//设置Attribute属性的Location，（可选，不设置的话，在glLinkProgram会由系统自动分配）
	glBindAttribLocation(program, 0, "aPosition");
	glBindAttribLocation(program, 1, "aColor");

	glLinkProgram(program);

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	GLint max_vert_arrtibute_num = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_arrtibute_num);	//获取最大支持顶点属性个数
	printf("max_vert_arrtibute:%d \n", max_vert_arrtibute_num);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint CreateVAO(GLuint &VBO, GLuint &IBO)
{
	//VAO操作
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//复制顶点数组到缓冲中供OpenGL使用
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//设置顶点posotionLocation属性指针
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(0);

	//设置顶点colorLocation属性指针
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

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

	GLuint program = createProgram();

	GLuint VBO, IBO;
	GLuint VAO = CreateVAO(VBO, IBO);

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		////render
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, vertices);//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[2]);
		//glEnableVertexAttribArray(1);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);

		//render with vao
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		printf("===============glGetError:%d============================= \n", glGetError());

		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteProgram(program);

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}