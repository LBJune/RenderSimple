#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>
#include <windows.h>

#include <chrono>

double getCurrentTime() {
	// 获取当前时间点
	auto now = std::chrono::high_resolution_clock::now();
	// 转换为自epoch（1970-01-01 00:00:00）以来的毫秒数
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	return duration.count();
}

std::string getExeDirectory() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath = path;
	return exePath.substr(0, exePath.find_last_of("\\/"));
}


GLuint createProgram()
{
	const char* vertexShaderSource = R"(
		#version 320 es
		in vec2 aPosition;    

		out vec2 textureCoordinate;

		void main() {
			gl_Position = vec4(aPosition, 0.0, 1.0);
			textureCoordinate = aPosition * 0.5 + 0.5;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 320 es
		precision mediump float;

		in vec2 textureCoordinate;

		uniform sampler2D ourTexture;
	
		out vec4 FragColor;

		void main() {
			FragColor = texture(ourTexture, textureCoordinate);
		}
	)";

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

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

int main()
{
	//Initialize GLFW library
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	int width, height, channels;
	std::string path = getExeDirectory() + "\\image\\picture.png";
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	printf("width:%d, height:%d!\n", width, height);


	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, "RenderSimple Window", NULL, NULL);
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
		//---位置---//
		-1, -1,   //左下
		 1, -1,	//右下
		-1,  1,	//左上
		 1,  1,	//右上
	};

	GLuint indices[] = {
		0, 1, 2, // 第一个三角形
		1, 3, 2  // 第二个三角形
	};

	GLuint program = createProgram();

	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint textureLocation = glGetUniformLocation(program, "ourTexture");

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//指定buffer为ARRAY_BUFFER,后面glVertexAttribPointer最后参数为偏移
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(posotionLocation);

	//将顶点索引数组，绑定到buffer
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	//指定buffer为ELEMENT_ARRAY_BUFFER 索引数组
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	glBindVertexArray(0);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 分配纹理内存
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	//首次为某个纹理对象设定纹理图像时，必须使用这个函数。它会为纹理图像分配存储空间，并将你提供的图像数据上传到这个新分配的存储空间中.
	//glTexSubImage2D则是用于替换已存在的纹理对象的一部分图像数据。它不会改变纹理图像的大小或其内部格式，只会改变图像的内容

	int DATA_SIZE = 4 * width * height * sizeof(unsigned char);
	GLuint PBO;
	glGenBuffers(1, &PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	GLuint doublePBO[2];
	glGenBuffers(2, doublePBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, doublePBO[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, data, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, doublePBO[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	int currentPBOId = 0;

	//创建查询的对象名
	GLuint queryObjectName[2];
	glGenQueries(2, queryObjectName);

	double cpuAverageTime = 0;
	double gpuAverageTime = 0;
	unsigned long count = 0;
	
	double cpuAverageTime2 = 0;
	double gpuAverageTime2 = 0;
	unsigned char * dataBuffer = new unsigned char[4 * width * height];

	GLuint PBO_READ;
	glGenBuffers(1, &PBO_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO_READ);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	GLuint doublePBO_READ[2];
	glGenBuffers(2, doublePBO_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, doublePBO_READ[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, doublePBO_READ[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		glUseProgram(program);

		glBeginQuery(GL_TIME_ELAPSED, queryObjectName[0]);
		double startTime = getCurrentTime();

		//立即模式刷新纹理
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		////PBO刷新纹理
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
		//GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		//if (ptr)
		//{
		//	// CPU将数据发送到PBO，但是在这个过程中可以进行其他操作。
		//	memcpy(ptr, data, DATA_SIZE);
		//	// 此时数据已经在PBO中，但可能还没准备好发送到GPU。
		//	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		//}
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);		// copy pixels from PBO to texture object
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		////双缓冲PBO刷新纹理
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, doublePBO[currentPBOId]);	    // bind the texture and PBO
		//glBindTexture(GL_TEXTURE_2D, texture);  
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// copy pixels from PBO to texture object

		//currentPBOId = 1 - currentPBOId;
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, doublePBO[currentPBOId]);
		//// Buffer orphaning
		//// map the buffer object into client's memory
		//// Note that glMapBuffer() causes sync issue.
		//// If GPU is working with this buffer, glMapBuffer() will wait(stall)
		//// for GPU to finish its job. To avoid waiting (stall), you can call
		//// first glBufferData() with NULL pointer before glMapBuffer().
		//// If you do that, the previous data in PBO will be discarded and
		//// glMapBuffer() returns a new allocated pointer immediately
		//// even if GPU is still working with the previous data.
		//glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);	//可选，如果此buffer正在被使用的话，此接口可以返回一个新的指针, CPU可以立刻向新的缓冲区写入数据，而不需要等待GPU读取完旧的缓冲区。
		//GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		//if (ptr)
		//{
		//	// CPU将数据发送到PBO，但是在这个过程中可以进行其他操作。
		//	memcpy(ptr, data, DATA_SIZE);
		//	// 此时数据已经在PBO中，但可能还没准备好发送到GPU。
		//	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		//}
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		double endTime = getCurrentTime();
		double cpuTimeElapsed = endTime - startTime;
		glEndQuery(GL_TIME_ELAPSED);

		GLuint timeElapsed;
		glGetQueryObjectuiv(queryObjectName[0], GL_QUERY_RESULT, &timeElapsed);
		//printf("Update Texture CPU time:%f, GPU time:%f \n", cpuTimeElapsed, timeElapsed / 1000000.0f);//单位为纳秒，此处转换成毫秒

		cpuAverageTime += cpuTimeElapsed;
		gpuAverageTime += timeElapsed / 1000000.0f;
		count++;

		GLint textureUnit = 0;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureLocation, textureUnit);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glBeginQuery(GL_TIME_ELAPSED, queryObjectName[1]);
		startTime = getCurrentTime();
		//常规读取
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, dataBuffer);

		////PBO读取纹理
		//glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO_READ);
		//glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		//GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		//if (src)
		//{
		//	memcpy(dataBuffer, src, DATA_SIZE);
		//	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);        // release pointer to the mapped buffer
		//}
		//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		////双缓冲PBO读取纹理
		//glBindBuffer(GL_PIXEL_PACK_BUFFER, doublePBO_READ[currentPBOId]);
		//glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		//currentPBOId = 1 - currentPBOId;
		//glBindBuffer(GL_PIXEL_PACK_BUFFER, doublePBO_READ[currentPBOId]);
		//GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		//if (src)
		//{
		//	memcpy(dataBuffer, src, DATA_SIZE);
		//	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		//}
		//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		endTime = getCurrentTime();
		cpuTimeElapsed = endTime - startTime;
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectuiv(queryObjectName[1], GL_QUERY_RESULT, &timeElapsed);
		cpuAverageTime2 += cpuTimeElapsed;
		gpuAverageTime2 += timeElapsed / 1000000.0f;

		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		if (count > 1000) break;
	}

	path = getExeDirectory() + "\\image\\output.png";
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path.c_str(), width, height, 4, dataBuffer, 4 * width * sizeof(unsigned char));

	printf("Update Texture CPU average time:%f, GPU average time:%f \n", cpuAverageTime / count, gpuAverageTime / count);
	printf("Dump Texture CPU average time:%f, GPU average time:%f \n", cpuAverageTime2 / count, gpuAverageTime2 / count);

	glDeleteQueries(2, queryObjectName);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &PBO);
	glDeleteBuffers(2, doublePBO);
	glDeleteBuffers(1, &PBO_READ);
	glDeleteBuffers(2, doublePBO_READ);
	glDeleteProgram(program);
	glDeleteTextures(1, &texture);

	delete [] dataBuffer;

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}