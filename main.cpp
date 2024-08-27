#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>


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

	GLuint program = CreateProgram();

	//创建查询的对象名
	GLuint queryObjectName[5];
	glGenQueries(5, queryObjectName);

	//创建查询对象的另外一种方式，可预先初始化查询对象的状态，这会使得第一次使用查询对象的开销比 glGenQueries 小。
	GLuint samplePassed;
	glCreateQueries(GL_ANY_SAMPLES_PASSED, 1, &samplePassed);
	GLuint timeStamp[2];
	glCreateQueries(GL_TIMESTAMP, 2, timeStamp);

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glBeginQueryIndexed(GL_ANY_SAMPLES_PASSED, 0, samplePassed);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEndQueryIndexed(GL_ANY_SAMPLES_PASSED, 0);


		//查询状态
		glBeginQuery(GL_ANY_SAMPLES_PASSED, queryObjectName[0]);
		//glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, queryObjectName[1]);不可以和GL_ANY_SAMPLES_PASSED同时查询，他们都是通过采样的查询，同时查会报glError。
		glBeginQuery(GL_PRIMITIVES_GENERATED, queryObjectName[2]);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryObjectName[3]);
		glBeginQuery(GL_TIME_ELAPSED, queryObjectName[4]);//仅支持OpengGL3以上，不支持OpenGLES任何版本。

		glQueryCounter(timeStamp[0], GL_TIMESTAMP);	//记录GL开始渲染的时间戳

		glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glQueryCounter(timeStamp[1], GL_TIMESTAMP);	//记录GL结束渲染的时间戳

		GLint timeElapsedQueryObject;
		glGetQueryiv(GL_TIME_ELAPSED, GL_CURRENT_QUERY, &timeElapsedQueryObject);	//获取GL_TIME_ELAPSED的查询对象，结果即queryObjectName[4]。在查询状态下才能获取，如果end之后的话，会返回0
		printf("timeElapsedQueryObject:%d \n", timeElapsedQueryObject);

		//结束查询
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		//glEndQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE);
		glEndQuery(GL_PRIMITIVES_GENERATED);
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		glEndQuery(GL_TIME_ELAPSED);

		//获取和打印查询结果
		GLuint isGLClearSamplePassed;
		glGetQueryObjectuiv(samplePassed, GL_QUERY_RESULT, &isGLClearSamplePassed);	//glClear不会执行sample，所以这里返回值为0
		printf("isGLClearSamplePassed:%u \n", isGLClearSamplePassed);

		GLuint isSamplePassed;
		glGetQueryObjectuiv(queryObjectName[0], GL_QUERY_RESULT, &isSamplePassed);
		//glGetQueryObjectuiv(queryObjectName[1], GL_QUERY_RESULT, &isSamplePassed);
		printf("isSamplePassed:%u \n", isSamplePassed);

		GLuint primitivesNum;
		glGetQueryObjectuiv(queryObjectName[2], GL_QUERY_RESULT, &primitivesNum);	//这里图元是三角形，所以生成的图元数量算1个，如果glDrawArrays的图元类型是GL_POINTS的话，这里生成的图元就是3了。
		printf("primitivesNum:%u \n", primitivesNum);

		GLuint transformFeedBackPrimitivesNum;
		glGetQueryObjectuiv(queryObjectName[3], GL_QUERY_RESULT, &transformFeedBackPrimitivesNum);	//这里没有使用变换反馈，所以这里数量为0
		printf("transformFeedBackPrimitivesNum:%u \n", transformFeedBackPrimitivesNum);


		GLuint timeElapsed;
		glGetQueryObjectuiv(queryObjectName[4], GL_QUERY_RESULT, &timeElapsed);
		printf("timeElapsed:%f \n", timeElapsed / 1000000.0f);//单位为纳秒，此处转换成毫秒

		GLuint64  startTime;
		glGetQueryObjectui64v(timeStamp[0], GL_QUERY_RESULT, &startTime);//需要用64位，不然精度不够
		printf("startTime:%llu \n", startTime);

		GLuint64  entTime;
		glGetQueryObjectui64v(timeStamp[1], GL_QUERY_RESULT, &entTime);
		printf("entTime:%llu \n", entTime);
		printf("timeElapsed2:%f \n", (entTime - startTime) / 1000000.0f);//如果为了计算耗时的话，其实和glBeginQuery/glEndQuery的GL_TIME_ELAPSED是等价的


		glBeginConditionalRender(samplePassed, GL_QUERY_WAIT); //根据查询对象，进行有条件的渲染。如果这个查询对象是queryObjectName[0]的话，屏幕会变成绿色
		glClearColor(0.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEndConditionalRender();

		int error = glGetError();
		printf("===============glGetError:%d============================= \n", error);
		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	//删除查询对象
	glDeleteQueries(5, queryObjectName);
	glDeleteQueries(1, &samplePassed);
	glDeleteQueries(2, timeStamp);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteProgram(program);

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}