#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <io.h>
#include <windows.h>

// shader
const char* vertexShaderSource = R"(
#version 320 es
layout(location = 0) in vec2 position;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 320 es
precision mediump float;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

std::string getExeDirectory() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath = path;
	return exePath.substr(0, exePath.find_last_of("\\/"));
}

GLuint CreateProgram()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// CHECK FOR ERRORS HERE!
	//  ...

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// CHECK FOR ERRORS HERE!
	//  ...

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// CHECK FOR ERRORS HERE!
	//  ...

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint CreateProgramPipeline()
{
	GLuint vertexShaderProgram = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexShaderSource);
	GLuint fragmentShaderProgram = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragmentShaderSource);

	GLuint pipeline;
	glGenProgramPipelines(1, &pipeline);
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexShaderProgram);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentShaderProgram);

	// CHECK FOR ERRORS HERE!
	//  ...

	glDeleteProgram(vertexShaderProgram);
	glDeleteProgram(fragmentShaderProgram);

	return pipeline;
}

// 保存二进制program到文件
void saveProgramBinary(GLuint program, std::string filename) {
	GLint length;
	glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);

	std::vector<char> binary(length);
	GLenum binaryFormat;
	glGetProgramBinary(program, length, NULL, &binaryFormat, binary.data());

	std::ofstream outFile(filename, std::ios::binary);
	outFile.write(binary.data(), length);
	outFile.close();

	//为了方便，这里新建一个文件存储格式
	std::ofstream outFileFormat(filename+"_format", std::ios::binary);
	outFileFormat << binaryFormat;
	outFileFormat.close();
}

GLuint GenProgramByFile(std::string file_path)
{
	std::ifstream inFile(file_path, std::ios::binary);
	std::vector<char> binary((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	inFile.close();

	GLuint program = glCreateProgram();

	//format的格式必须和glGetProgramBinary生成的binaryFormat对应，这里从文件中读取格式
	GLenum binaryFormat;
	std::ifstream inFileFormat(file_path + "_format", std::ios::binary);
	inFileFormat >> binaryFormat;
	inFileFormat.close();

	glProgramBinary(program, binaryFormat, binary.data(), binary.size());

	// CHECK FOR ERRORS HERE!
	//  ...

	return program;
}

GLuint CreateProgramByBinary()
{
	//判断是否支持二进制文件
	GLint numFormats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats == 0)
	{
		return -1;
	}

	std::string file_path = getExeDirectory()+"\\program.bin";

	//避免每次生成文件
	if (_access(file_path.c_str(), 0) == -1)
	{
		GLuint program = CreateProgram();
		saveProgramBinary(program, file_path);
	}

	GLuint program = GenProgramByFile(file_path);

	//如果glProgramBinary失败的话，请用shader的源码再生成二进制文件

	return program;
}

GLuint CreateProgramPipelineByBinary()
{
	//判断是否支持二进制文件
	GLint numFormats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats == 0)
	{
		return -1;
	}

	std::string vertex_file_path = getExeDirectory() + "\\vertexShaderProgram.bin";
	std::string fragment_file_path = getExeDirectory() + "\\fragmentShaderProgram.bin";

	//避免每次生成文件
	if (_access(vertex_file_path.c_str(), 0) == -1)
	{
		GLuint vertexShaderProgram = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexShaderSource);
		saveProgramBinary(vertexShaderProgram, vertex_file_path);
	}
	if (_access(fragment_file_path.c_str(), 0) == -1)
	{
		GLuint fragmentShaderProgram = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragmentShaderSource);
		saveProgramBinary(fragmentShaderProgram, fragment_file_path);
	}


	GLuint vertexShaderProgram = GenProgramByFile(vertex_file_path);
	GLuint fragmentShaderProgram = GenProgramByFile(fragment_file_path);

	//如果glProgramBinary失败的话，请用shader的源码再生成二进制文件

	GLuint pipeline;
	glGenProgramPipelines(1, &pipeline);
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexShaderProgram);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentShaderProgram);

	// CHECK FOR ERRORS HERE!
	//  ...

	glDeleteProgram(vertexShaderProgram);
	glDeleteProgram(fragmentShaderProgram);

	return pipeline;
}

GLuint GenShaderByBinaryFile(std::string file_path, GLenum shaderType)
{
	//判断是否支持二进制文件
	GLint numFormats = 0;
	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &numFormats);
	if (numFormats == 0)
	{
		return -1;
	}

	GLuint shader = 0;
	if (shaderType == GL_VERTEX_SHADER)
	{
		shader = glCreateShader(GL_VERTEX_SHADER);
	} else if (shaderType == GL_FRAGMENT_SHADER)
	{
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	std::ifstream inFile(file_path, std::ios::binary);
	std::vector<char> binary((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	inFile.close();

	glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.data(), binary.size());

	// Specialize the shader
	const char* entry_point_name = "main";
	glSpecializeShader(shader, entry_point_name, 0, nullptr, nullptr);

	// CHECK FOR ERRORS HERE!
	//  ...

	return shader;
}

GLuint CreateProgramByBinaryShader()
{
	std::string vertex_file_path = getExeDirectory() + "\\shader\\vertex.spv";
	std::string fragment_file_path = getExeDirectory() + "\\shader\\fragment.spv";


	GLuint vertexShader = GenShaderByBinaryFile(vertex_file_path, GL_VERTEX_SHADER);
	GLuint fragmentShader = GenShaderByBinaryFile(fragment_file_path, GL_FRAGMENT_SHADER);

	// CHECK FOR ERRORS HERE!
	//  ...

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// CHECK FOR ERRORS HERE!
	//  ...

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
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

	float vertices[] =
	{
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		0.0f,  0.5f
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	//复制顶点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	GLuint id = -1;

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

	
		//if (id == -1) id = CreateProgram();
		//glUseProgram(id);


		//if (id == -1) id = CreateProgramPipeline();
		//glBindProgramPipeline(id);


		//if (id == -1) id = CreateProgramByBinary();
		//glUseProgram(id);


		//if (id == -1) id = CreateProgramPipelineByBinary();
		//glBindProgramPipeline(id);


		if (id == -1) id = CreateProgramByBinaryShader();
		glUseProgram(id);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	if (id > 0)
	{
		if (glIsProgram(id))
		{
			glDeleteProgram(id);
		} else if (glIsProgramPipeline(id))
		{
			glDeleteProgramPipelines(1, &id);
		}
	}

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}