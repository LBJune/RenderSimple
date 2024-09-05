#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <windows.h>
#include <chrono>

static int windowsWidth = 800;
static int windowsHeight = 600;

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

	GLint max_vert_arrtibute_num = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_arrtibute_num);	//获取最大支持顶点属性个数
	printf("max_vert_arrtibute:%d \n", max_vert_arrtibute_num);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint createProgram2()
{
	const char* vertexShaderSource = R"(
		#version 320 es

		in vec3 aColor;
		in vec2 aPosition;    
		in mat4 aTranslateMatrix;

		out vec3 color;

		void main() {
			gl_Position = aTranslateMatrix * vec4(aPosition, 0.0, 1.0);
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

	GLint max_vert_arrtibute_num = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_arrtibute_num);	//获取最大支持顶点属性个数
	printf("max_vert_arrtibute:%d \n", max_vert_arrtibute_num);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint createProgram3()
{
	const char* vertexShaderSource = R"(
		#version 320 es

		in vec3 aColor;
		in vec2 aPosition;    
		uniform mat4 aTranslateMatrix;

		out vec3 color;

		void main() {
			gl_Position = aTranslateMatrix * vec4(aPosition, 0.0, 1.0);
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

	GLint max_vert_arrtibute_num = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_arrtibute_num);	//获取最大支持顶点属性个数
	printf("max_vert_arrtibute:%d \n", max_vert_arrtibute_num);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint createProgram4()
{
	const char* vertexShaderSource = R"(
		#version 320 es

		in vec3 aColor;
		in vec2 aPosition;    
		uniform vertexUniforms {
			mat4 aTranslateMatrix;
		};

		out vec3 color;

		void main() {
			gl_Position = aTranslateMatrix * vec4(aPosition, 0.0, 1.0);
			color = aColor;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 320 es
		precision mediump float;

		in vec3 color;
		out vec4 FragColor;

		uniform fragmentUniforms {
				float intensity;
			};

		void main() {
			FragColor = vec4(color * intensity, 1.0);
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

	//设置Attribute属性的Location，（可选，不设置的话，在glLinkProgram会由系统自动分配）
	glBindAttribLocation(program, 0, "aPosition");
	glBindAttribLocation(program, 1, "aColor");

	glLinkProgram(program);

	GLint max_vert_arrtibute_num = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_arrtibute_num);	//获取最大支持顶点属性个数

	GLint max_uniform_buffer_binding_num = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_binding_num);	//获取最大binding point个数
	printf("max_vert_arrtibute_num:%d , max_uniform_buffer_binding_num:%d\n", max_vert_arrtibute_num, max_uniform_buffer_binding_num);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint createProgram5()
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

void DrawTriganle()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f,  0.0f, 0.0f, 1.0f
	};

	GLuint program = createProgram();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	glUseProgram(program);

	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, vertices);
	glEnableVertexAttribArray(posotionLocation);

	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[2]);
	glEnableVertexAttribArray(colorLocation);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteProgram(program);
}

void DrawTriganleVAO()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f,  0.0f, 0.0f, 1.0f
	};

	GLuint program = createProgram();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//指定buffer为ARRAY_BUFFER,后面glVertexAttribPointer最后参数为偏移
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(posotionLocation);

	//aColor参数赋值
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colorLocation);

	glBindVertexArray(0);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);


	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(program);
}

void DrawRectangle()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	//左下
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,	//右下
		-0.5f,  0.5f,  0.0f, 0.0f, 1.0f,	//左上
		 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,	//右上
	};

	GLuint indices[] = {
		0, 1, 2, // 第一个三角形
		1, 3, 2  // 第二个三角形
	};

	GLuint program = createProgram();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	glUseProgram(program);

	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, vertices);
	glEnableVertexAttribArray(posotionLocation);

	//aColor参数赋值
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[2]);
	glEnableVertexAttribArray(colorLocation);

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, indices);

	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteProgram(program);
}

void DrawRectangleVBO()
{
	float vertices[] = {
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	//左下
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,	//右下
		-0.5f,  0.5f,  0.0f, 0.0f, 1.0f,	//左上
		 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,	//右上
	};

	GLuint indices[] = {
		0, 1, 2, // 第一个三角形
		1, 3, 2  // 第二个三角形
	};

	GLuint program = createProgram();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//指定buffer为ARRAY_BUFFER,后面glVertexAttribPointer最后参数为偏移
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(posotionLocation);

	//aColor参数赋值
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colorLocation);

	//将顶点索引数组，绑定到buffer
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	//指定buffer为ELEMENT_ARRAY_BUFFER 索引数组
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	glBindVertexArray(0);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(program);
}

//通过Attribute的mat4矩阵，设置偏移（正常来说不会通过Attribute设置偏移矩阵，只在多实例的时候，每个实例只有参数不同的情况下会考虑）
void DrawTriganleAttributeTranslate()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f,  0.0f, 0.0f, 1.0f
	};

	float translateMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0.5f, 0.5f, 0, 1		//往右上偏移
	};

	GLuint program = createProgram2();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");
	GLint translateLocation = glGetAttribLocation(program, "aTranslateMatrix");//mat4矩阵

	printf("posotionLocation:%d, colorLocation:%d, translateLocation:%d \n ", posotionLocation, colorLocation, translateLocation);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//指定buffer为ARRAY_BUFFER,后面glVertexAttribPointer最后参数为偏移
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(posotionLocation);

	//aColor参数赋值
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colorLocation);

	//Attribute的类型为mat4的传参
	GLuint VBO2;
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(translateMatrix), translateMatrix, GL_STATIC_DRAW);

	for (int i = 0; i < 4; i++)
	{
		glVertexAttribPointer(translateLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(4 * i * sizeof(float)));
		glEnableVertexAttribArray(translateLocation + i);
		glVertexAttribDivisor(translateLocation + i, 1);  // tell OpenGL to update this attribute every instance, not every vertex  这里只有1个mat4，所有顶点都共用的，所以每个实例(每次渲染)只需要更新1次。
	}

	////如下是使用全量的偏移矩阵, 全量的话可以为每个顶点设置不同的偏移值
	//float fulleTranslateMatrix[3 * 16] = {
	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0.1f, 0.1f, 0, 1,		//往右上偏移

	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0.2f, 0.2f, 0, 1,		//往右上偏移

	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0.3f, 0.3f, 0, 1,		//往右上偏移
	//};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(fulleTranslateMatrix), fulleTranslateMatrix, GL_STATIC_DRAW);

	//for (int i = 0; i < 4; i++)
	//{
	//	glVertexAttribPointer(translateLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(4 * i * sizeof(float)));
	//	glEnableVertexAttribArray(translateLocation + i);
	//	//glVertexAttribDivisor(translateLocation + i, 1);  // no need
	//}

	glBindVertexArray(0);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);


	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(program);
}

//通过Uniform的mat4矩阵，设置偏移
void DrawTriganleUniformTranslate()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f,  0.0f, 0.0f, 1.0f
	};

	float translateMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0.5f, 0.5f, 0, 1		//往右上偏移
	};

	GLuint program = createProgram3();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");
	GLint translateLocation = glGetUniformLocation(program, "aTranslateMatrix");

	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	glUseProgram(program);

	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, vertices);
	glEnableVertexAttribArray(posotionLocation);

	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[2]);
	glEnableVertexAttribArray(colorLocation);

	glUniformMatrix4fv(translateLocation, 1, GL_FALSE, translateMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteProgram(program);
}

//通过Uniformblock的mat4矩阵，设置偏移
void DrawTriganleUniformBlockTranslate()
{
	float vertices[] =
	{
		//---位置---//  //---颜色---//
		-0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.5f,  0.0f, 0.0f, 1.0f
	};

	float translateMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0.5f, 0.5f, 0, 1		//往右上偏移
	};


	GLuint program = createProgram4();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint colorLocation = glGetAttribLocation(program, "aColor");
	GLuint blockIndex = glGetUniformBlockIndex(program, "vertexUniforms");	
	GLuint fragmentblockIndex = glGetUniformBlockIndex(program, "fragmentUniforms");

	glUniformBlockBinding(program, blockIndex, 0);//如果shader中有显式设置bindingpoint，可以不调用这个接口。如果设置的话，会覆盖layout(binding = N)的值
	glUniformBlockBinding(program, fragmentblockIndex, 1);
	printf("posotionLocation:%d, colorLocation:%d \n ", posotionLocation, colorLocation);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//指定buffer为ARRAY_BUFFER,后面glVertexAttribPointer最后参数为偏移
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 	//复制顶点数组到buffer缓冲区，供OpenGL使用

	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);	//(void*)0指向的是buffer数据的偏移
	glEnableVertexAttribArray(posotionLocation);

	//aColor参数赋值
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colorLocation);

	glBindVertexArray(0);

	//一个UBO可以与一个或多个UniformBlocks(不同program的UniformBlock)的绑定点关联，反之亦然
	GLuint UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(translateMatrix), translateMatrix, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);	//glUniformBlockBinding设置为0，这里需要匹配
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	float intensity = 0.5f;
	GLuint fragmentUBO;
	glGenBuffers(1, &fragmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, fragmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(intensity), &intensity, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, fragmentUBO);	//glUniformBlockBinding设置为1，这里需要匹配
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glDisableVertexAttribArray(posotionLocation);
	glDisableVertexAttribArray(colorLocation);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &UBO);
	glDeleteBuffers(1, &fragmentUBO);
	glDeleteProgram(program);
}

void DrawImage(unsigned char *data, int width, int height)
{
	if (!data || !width || !height) return;

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

	GLuint program = createProgram5();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint textureLocation = glGetUniformLocation(program, "ourTexture");

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 加载纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	//首次为某个纹理对象设定纹理图像时，必须使用这个函数。它会为纹理图像分配存储空间，并将你提供的图像数据上传到这个新分配的存储空间中.
	//glTexSubImage2D则是用于替换已存在的纹理对象的一部分图像数据。它不会改变纹理图像的大小或其内部格式，只会改变图像的内容

	glUseProgram(program);

	GLint textureUnit = 0;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureLocation, textureUnit);	//使用 glUniform1i() 将纹理单元编号传给指定的sampler。在大多数实现中，glUniform1i() 的调用将覆盖 layout(binding = N) 的设置，注意：这种行为从OpenGL规范来看是未定义的，它可能在部分OpenGL的实现中能正常工作，但不能保证在所有实现中都能正常工作。最好二者选其一。

	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, vertices);
	glEnableVertexAttribArray(posotionLocation);

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, indices);

	glDisableVertexAttribArray(posotionLocation);

	glDeleteProgram(program);
	glDeleteTextures(1, &texture);
}

void DrawImagePBO(unsigned char* data, int width, int height)
{
	if (!data || !width || !height) return;

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

	GLuint program = createProgram5();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint textureLocation = glGetUniformLocation(program, "ourTexture");

	GLuint pbo;
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4 * sizeof(unsigned char), data, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	glUseProgram(program);

	GLint textureUnit = 0;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureLocation, textureUnit);	//使用 glUniform1i() 将纹理单元编号传给指定的sampler。在大多数实现中，glUniform1i() 的调用将覆盖 layout(binding = N) 的设置，注意：这种行为从OpenGL规范来看是未定义的，它可能在部分OpenGL的实现中能正常工作，但不能保证在所有实现中都能正常工作


	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, vertices);
	glEnableVertexAttribArray(posotionLocation);

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, indices);

	glDisableVertexAttribArray(posotionLocation);

	glDeleteProgram(program);
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &pbo);
}

//只在OpenGL4.5以上支持，DSA主要是为了提供代码的可读性和编程效率，而不是为了提升程序的运行速度
void DrawImageDSA(unsigned char* data, int width, int height)
{
	if (!data || !width || !height) return;

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

	GLuint program = createProgram5();

	//获取Attribute属性的Location
	GLint posotionLocation = glGetAttribLocation(program, "aPosition");
	GLint textureLocation = glGetUniformLocation(program, "ourTexture");

	unsigned int texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);	//生成一个纹理ID，并立即初始化纹理对象

	//直接设置纹理属性，无需绑定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 指定纹理的存储并加载数据
	glTextureStorage2D(texture, 1, GL_RGBA8, width, height);	//仅分配内存，不加载数据
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);	//用于加载纹理数据

	glUseProgram(program);

	GLint textureUnit = 0;
	glBindTextureUnit(textureUnit, texture);

	//没有绑定buffer的情况下，最后的参数是数据指针地址，如果绑了buffer，则是buffer的偏移
	//aPosition参数赋值
	glVertexAttribPointer(posotionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, vertices);
	glEnableVertexAttribArray(posotionLocation);

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, indices);

	glDisableVertexAttribArray(posotionLocation);

	glDeleteProgram(program);
	glDeleteTextures(1, &texture);
}

int main()
{
	//Initialize GLFW library
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	int channels;
	std::string path = getExeDirectory() + "\\image\\picture.png";
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &windowsWidth, &windowsHeight, &channels, STBI_rgb_alpha);

	printf("width:%d, height:%d!\n", windowsWidth, windowsHeight);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(windowsWidth, windowsHeight, "RenderSimple Window", NULL, NULL);
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

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//DrawTriganle();
		//DrawTriganleVAO();
		//DrawRectangle();
		//DrawRectangleVBO();
		//DrawTriganleAttributeTranslate();
		//DrawTriganleUniformTranslate();
		//DrawTriganleUniformBlockTranslate();
		//DrawImage(data, windowsWidth, windowsHeight);
		//DrawImagePBO(data, windowsWidth, windowsHeight);
		DrawImageDSA(data, windowsWidth, windowsHeight);

		printf("===============glGetError:%d============================= \n", glGetError());

		// Render here (swap front and back buffers)
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	//Destroy window and resources
	glfwDestroyWindow(window);
	glfwTerminate();

	stbi_image_free(data);

	return 0;
}