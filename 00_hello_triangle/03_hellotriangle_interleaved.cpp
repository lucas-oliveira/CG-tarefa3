#include <iostream>

#include <GL/gl3w.h> // here: we need compile gl3w.c - utils dir
#include <GLFW/glfw3.h>
#include "gl_utils.h" // parser for shader source files

static int width = 800, height = 600;


// The MAIN function, from here we start the application and run the game loop
int main( )
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

  g_window = glfwCreateWindow(width,height , "Hello Triangle Interleaved", NULL, NULL);
  if (!g_window)
    {
      glfwTerminate();
      return -1;
    }


  int screenWidth, screenHeight;
  glfwGetFramebufferSize( g_window, &screenWidth, &screenHeight );

  if (!g_window)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate( );

      return EXIT_FAILURE;
    }

  glfwMakeContextCurrent( g_window );

  if (gl3wInit()) {
      std::cout << "failed to initialize OpenGL\n" << std::endl;
      return -1;
  }

  if (!gl3wIsSupported(3, 2)) {
      std::cout << "OpenGL 3.2 not supported\n" << std::endl;
      return -1;
  }


  glViewport( 0, 0, screenWidth, screenHeight );


  char vertex_shader[1024 * 256];
  char fragment_shader[1024 * 256];
  parse_file_into_str( "core_vs.glsl", vertex_shader, 1024 * 256 );
  parse_file_into_str( "core_frag.glsl", fragment_shader, 1024 * 256 );

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

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
  if (!result)
    {
      glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Shader program linker failure. " << infoLog  << std::endl;
    }
  glDeleteShader(vs);
  glDeleteShader(fs);


  GLfloat vertices[] =
    {
      // Positions         // Colors
      0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
      -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
      0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top
    };
  GLuint VBO, VAO;
  glGenVertexArrays( 1, &VAO );
  glGenBuffers( 1, &VBO );
  // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
  glBindVertexArray( VAO );

  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

  // Position attribute
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * ) 0 );
  glEnableVertexAttribArray( 0 );
  // Color attribute
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
  glEnableVertexAttribArray( 1 );
  glBindVertexArray( 0 ); // Unbind VAO

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // rendering loop
  while ( !glfwWindowShouldClose( g_window ) )
    {
      glfwPollEvents( );
     
      glClear( GL_COLOR_BUFFER_BIT );

      glUseProgram(shaderProgram);
      glBindVertexArray( VAO );
      glDrawArrays( GL_TRIANGLES, 0, 3 );
      glBindVertexArray(0);
      // Swap the screen buffers
      glfwSwapBuffers( g_window );
    }

  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays( 1, &VAO );
  glDeleteBuffers( 1, &VBO );

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate( );

  return EXIT_SUCCESS;
}
