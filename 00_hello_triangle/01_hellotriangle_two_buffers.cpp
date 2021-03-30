/*
"Hello Triangle". Just the basics + shaders + 2 buffers
Novelty:
Adds index buffer (Element Array Buffer)
*/

#include <GL/gl3w.h> // here: we need compile gl3w.c - utils dir
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

#include "gl_utils.h" // parser for shader source files

static int width = 800, height = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void reshape_callback(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);


int main()
{

   GLFWwindow* g_window;

  if (!glfwInit())
    return -1;
#ifdef APPLE
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  g_window = glfwCreateWindow(width,height , "Hello Triangle Indexed", NULL, NULL);
  if (!g_window)
    {
      glfwTerminate();
      return -1;
    }

  glfwMakeContextCurrent(g_window);
  glfwSetKeyCallback(g_window, key_callback);
  glfwSetFramebufferSizeCallback(g_window, reshape_callback);


  if (gl3wInit()) {
	  std::cout << "failed to initialize OpenGL\n" << std::endl;
	  return -1;
  }

  if (!gl3wIsSupported(3, 2)) {
	  std::cout << "OpenGL 3.2 not supported\n" << std::endl;
	  return -1;
  }


  GLfloat vertices[] = {
    -0.9f,  0.5f, 0.0f,		// Top left
    0.5f,  0.5f, 0.0f,		// Top right
    -0.5f, -0.5f, 0.0f		// Bottom right
  };
  GLfloat vertices2[] = {
	  0.7f,  0.1f, 0.0f,		// Top left
	  -0.5f, -0.5f, 0.0f,		// Bottom right
	  0.5f,  0.5f, 0.0f,		// Top right
  };

  GLuint indices[] = {
    1, 0, 2  // First Triangle
   // 0, 2, 3   // Second Triangle
  };


  GLuint vbo, vbo2, vao,vao2;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

   glGenVertexArrays(1, &vao2);
  glBindVertexArray(vao2);

  glGenBuffers(1, &vbo2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  char vertex_shader[1024 * 256];
  char fragment_shader[1024 * 256];
  char fragment_shader2[1024 * 256];
  parse_file_into_str( "vs.glsl", vertex_shader, 1024 * 256 );
  parse_file_into_str( "fs.glsl", fragment_shader, 1024 * 256 );
  parse_file_into_str("fs2.glsl", fragment_shader2, 1024 * 256);

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  const GLchar *p = (const GLchar *)vertex_shader;
  glShaderSource(vs, 1, &p, NULL);
  glCompileShader(vs);


  GLint result;
  GLchar infoLog[512];
  glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Vertex shader failed to compile. " << infoLog << std::endl;
    }


  GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
  p = (const GLchar *)fragment_shader;
  glShaderSource( fs, 1, &p, NULL );
  glCompileShader(fs);

  GLint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
  p = (const GLchar *)fragment_shader2;
  glShaderSource(fs2, 1, &p, NULL);
  glCompileShader(fs2);


  glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
    }

  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);

  GLint shaderProgram2 = glCreateProgram();
  glAttachShader(shaderProgram2, vs);
  glAttachShader(shaderProgram2, fs2);
  glLinkProgram(shaderProgram2);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
  if (!result)
  {
	  glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
	  std::cout << "Error! Shader program linker failure. " << infoLog << std::endl;
  }

  glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &result);
  if (!result)
    {
      glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Shader program linker failure. " << infoLog  << std::endl;
    }

  glDeleteShader(vs);
  glDeleteShader(fs);
  glDeleteShader(fs2);


  // Rendering loop
  while (!glfwWindowShouldClose(g_window))
    {
      showFPS(g_window);

		// Poll for and process events
      glfwPollEvents();

      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT);

      // Render the quad (two triangles)

	  glUseProgram(shaderProgram);
	  glBindVertexArray(vao);
	  glDrawArrays(GL_TRIANGLES, 0, 3);
	  glBindVertexArray(0);

	  glUseProgram(shaderProgram2);
	  glBindVertexArray(vao2);
	  glDrawArrays(GL_TRIANGLES, 0, 3);
	  glBindVertexArray(0);

     

      // Swap front and back buffers
      glfwSwapBuffers(g_window);
    }

  // Clean up
  glDeleteProgram(shaderProgram);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  glfwTerminate();

  return 0;
}


//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  static bool gWireframe = 0;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void reshape_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< "Hello Triangle Indexed" << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}
