/*
"Hello Triangle". Just the basics + shaders + 2 buffers
Dependencies:
GL3W and GLFW3 to start GL.
Based on OpenGL 4 Example Code.
*/

#include <GL/gl3w.h> // here: we need compile gl3w.c - utils dir
#include <GLFW/glfw3.h>
#include <iostream>

#include "gl_utils.h" // parser for shader source files

static int width = 800, height = 600;

static void reshape_callback (GLFWwindow *window, int w, int h)
{
  width = w > 1 ? w : 1;
  height = h > 1 ? h : 1;
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}


int main() {

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

  g_window = glfwCreateWindow(800,600 , "Hello Triangle VIZLAB", NULL, NULL);
  if (!g_window)
    {
      glfwTerminate();
      return -1;
    }

  glfwMakeContextCurrent(g_window);
  glfwSetKeyCallback(g_window, key_callback);
  glfwSetFramebufferSizeCallback(g_window, reshape_callback);

  if (gl3wInit()) { // glad - loaders
      std::cout << "failed to initialize OpenGL\n" << std::endl;
      return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
      std::cout << "OpenGL 3.2 not supported\n" << std::endl;
      return -1;
  }










  /* Vertices / Colors */
  GLfloat points[] = { 0.0f, 0.5f, 0.0f,  // V0
                       0.5f, -0.5f, 0.0f, // V1
                      -0.5f, -0.5f, 0.0f  // V2
                      };
  GLfloat colours[] = { 1.0f, 0.0f, 0.0f, // C0
                        0.0f, 1.0f, 0.0f, // C1
                        0.0f, 0.0f, 1.0f  // C2
                       };

  GLuint points_vbo;
  glGenBuffers( 1, &points_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );

  GLuint colours_vbo;
  glGenBuffers( 1, &colours_vbo );
  glBindBuffer( GL_ARRAY_BUFFER, colours_vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), colours, GL_STATIC_DRAW );


  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
  glBindBuffer( GL_ARRAY_BUFFER, colours_vbo );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
  glEnableVertexAttribArray( 0 );
  glEnableVertexAttribArray( 1 );

  char vertex_shader[1024 * 256];
  char fragment_shader[1024 * 256];
  parse_file_into_str( "test_vs.glsl", vertex_shader, 1024 * 256 );
  parse_file_into_str( "test_fs.glsl", fragment_shader, 1024 * 256 );

  GLuint vs = glCreateShader( GL_VERTEX_SHADER );
  const GLchar *p = (const GLchar *)vertex_shader;
  glShaderSource( vs, 1, &p, NULL );
  glCompileShader( vs );

  // check for compile errors
  int params = -1;
  glGetShaderiv( vs, GL_COMPILE_STATUS, &params );
  if ( GL_TRUE != params ) {
    fprintf( stderr, "ERROR: GL shader index %i did not compile\n", vs );
    return 0; // or exit or something
  }

  GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
  p = (const GLchar *)fragment_shader;
  glShaderSource( fs, 1, &p, NULL );
  glCompileShader( fs );

  // check for compile errors
  glGetShaderiv( fs, GL_COMPILE_STATUS, &params );
  if ( GL_TRUE != params ) {
    fprintf( stderr, "ERROR: GL shader index %i did not compile\n", fs );
    return 0; // or exit or something
  }

  GLuint shader_programme = glCreateProgram();
  glAttachShader( shader_programme, fs );
  glAttachShader( shader_programme, vs );
  glLinkProgram( shader_programme );

  glGetProgramiv( shader_programme, GL_LINK_STATUS, &params );
  if ( GL_TRUE != params ) {
    fprintf( stderr, "ERROR: could not link shader programme GL index %i\n",
             shader_programme );
    return 0;
  }

  while ( !glfwWindowShouldClose( g_window ) ) {

    // wipe the drawing surface clear
    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( shader_programme );
    glBindVertexArray( vao );

    glDrawArrays( GL_TRIANGLES, 0, 3 );

    glfwPollEvents();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers( g_window );
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
