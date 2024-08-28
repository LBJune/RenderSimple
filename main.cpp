#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

const GLchar* vertexShaderSrc = R"(
	#version 320 es
    layout(location = 0) in float inValue;
    out float geoValue;
    void main()
    {
        geoValue = sqrt(inValue);
    }
)";

//几何着色器
const GLchar* geoShaderSrc = R"(
	#version 320 es

	layout (points) in;
	layout (points, max_vertices = 1) out;

	in float[] geoValue;
    out float outValue;
	out float outValue2;
    void main()
    {
		outValue = geoValue[0] + 1.0;
		outValue2 = geoValue[0] + 10.0;
        EmitVertex();
        EndPrimitive();
    }

)";

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

	//=============创建shader=====================
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);

	//几何着色器
	GLuint geoShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geoShader, 1, &geoShaderSrc, nullptr);
	glCompileShader(geoShader);

	//=============创建program=====================
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, geoShader);

	const GLchar* feedbackVaryings[] = { "outValue", "outValue2" };
	glTransformFeedbackVaryings(program, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);	//捕获哪些变量，该函数必须在链接程序 glLinkProgram 之前调用，因为链接过程会固定所有的输入和输出变量。也就是说，当链接过程完成后，输出变量的位置和数量就不能再更改了。
	//glTransformFeedbackVaryings(program, 2, feedbackVaryings, GL_SEPARATE_ATTRIBS);	//分离模式

	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(geoShader);

	//==============创建vao=====================
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	 
	GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 1, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//TBO 用于存储feedback的数据
	GLuint tbo;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 2.0, nullptr, GL_STATIC_DRAW);//有2个out，这里*2

	GLuint transformfeedback;
	glGenTransformFeedbacks(1, &transformfeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedback);	//设置当前TRANSFORM_FEEDBACK为transformfeedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);//将tbo绑定到当前transformfeedback对象

	////TBO 用于存储feedback的数据，分离模式
	//GLuint tbo[2];
	//glGenBuffers(2, tbo);
	//for (int i = 0; i < 2; i++)
	//{
	//	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo[i]);	//如果只是读取的话，可以只设置成GL_TRANSFORM_FEEDBACK_BUFFER，如果后面渲染需要用这个buffer的话，可以设置成GL_ARRAY_BUFFER类型
	//	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(data), NULL, GL_STATIC_READ);
	//	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, i, tbo[i]);	//i是索引，代表的是当前feedback的第2块buffer
	//}


	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_RASTERIZER_DISCARD);	//丢弃光栅化，因为不需要绘制到屏幕

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedback);
		//渲染操作
		glUseProgram(program);
		glBeginTransformFeedback(GL_POINTS);	//启动Transform Feedback，记得一定要放到glUseProgram的后面，不然不知道feedback哪个程序
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, 5);
		glBindVertexArray(0);

		//结束 Transform Feedback
		glEndTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);
		glFlush();

		// 获取数据
		 GLfloat feedback[10];
		 glBindBuffer(GL_ARRAY_BUFFER, tbo);//绑定获取tbo的数值
		 glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
		 printf("%f %f %f %f %f %f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4], feedback[5], feedback[6], feedback[7], feedback[8], feedback[9]);


		////获取数据方式二
		//glBindBuffer(GL_ARRAY_BUFFER, tbo);//绑定获取tbo的数值
		//GLvoid* data = glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);
		//GLfloat* feedback = static_cast<GLfloat *>(data);
		//printf("%f %f %f %f %f %f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4], feedback[5], feedback[6], feedback[7], feedback[8], feedback[9]);
		//glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);


		//分离模式的获取数据示例
		//for (int i = 0; i < 2; i++)
		//{
		//	GLfloat feedback[5];
		//	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo[i]);
		//	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
		//	printf("%f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4]);

		//}

	

		//后续可以继续使用修改后的tbo作为顶点buffer
		//glBindBuffer(GL_ARRAY_BUFFER, tbo);
		 
		printf("===============glGetError:%d============================= \n", glGetError());
		
		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glDeleteBuffers(1, &tbo);
	//glDeleteBuffers(2, tbo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	glDeleteTransformFeedbacks(1, &transformfeedback);


	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}