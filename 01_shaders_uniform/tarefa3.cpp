/*
"Hello Triangle". Just the basics + shaders + 2 buffers
Novelty:
Adds index buffer (Element Array Buffer)
*/

#include <GL/gl3w.h> // here: we need compile gl3w.c - utils dir
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <cmath>

#include "gl_utils.h" // parser for shader source files

static int width = 800, height = 600;
static float x_location = 0.0f;
static float y_location = 0.0f;

struct vertexColor
{
    float red;  
    float green;    
    float blue; 
};

struct vertexColor vertex;


bool keys[1024];

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void reshape_callback(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
void init_colors();
void process_color_change(int colorCode);
void process_moviment();

int main()
{

    GLFWwindow* g_window;

    if (!glfwInit())
        return -1;
#ifdef APPLE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    g_window = glfwCreateWindow(width, height, "Hello Triangle Indexed", NULL, NULL);
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
      -0.5f,  0.5f, 0.0f,		// Top left
      0.5f,  0.5f, 0.0f,		// Top right
      0.5f, -0.5f, 0.0f,		// Bottom right
      -0.5f, -0.5f, 0.0f		// Bottom left
    };

    GLuint indices[] = {
      0, 1, 2,  // First Triangle
      0, 2, 3   // Second Triangle
    };


    GLuint vbo, ibo, vao;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    //
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    parse_file_into_str("vs_uniform.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("fs_uniform.glsl", fragment_shader, 1024 * 256);


    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* p = (const GLchar*)vertex_shader;
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
    p = (const GLchar*)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
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
        std::cout << "Error! Shader program linker failure. " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    init_colors();

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

        // update shader uniform
        
        process_moviment();

        //uniforms
        int vertexColor = glGetUniformLocation(shaderProgram, "ourColor");


        glUniform4f(vertexColor, vertex.red, vertex.green, vertex.blue, 1.0f);

        int objectLocation = glGetUniformLocation(shaderProgram, "ourPosition");
        glUniform4f(objectLocation, x_location, y_location, 0.0f, 1.0f);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap front and back buffers
        glfwSwapBuffers(g_window);
    }

    // Clean up
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

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

    if (action == GLFW_PRESS) {
        keys[key] = true;

        int keyResult = 0;

        switch (key)
        {
        case GLFW_KEY_1:
            keyResult = 1;
            break;
        case GLFW_KEY_2:
            keyResult = 2;
            break;
        case GLFW_KEY_3:
            keyResult = 3;
            break;
        case GLFW_KEY_4:
            keyResult = 4;
            break;
        case GLFW_KEY_5:
            keyResult = 5;
            break;
        case GLFW_KEY_6:
            keyResult = 6;
            break;
        case GLFW_KEY_7:
            keyResult = 7;
            break;
        case GLFW_KEY_8:
            keyResult = 8;
            break;
        default:
            break;
        }
        process_color_change(keyResult);
    }

    if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void process_color_change(int colorCode) {
    switch (colorCode)
    {
    case 1:
        vertex.blue = 1.0f;
        vertex.green = 0.0f;
        vertex.red = 0.0f;
        break;
    case 2:
        vertex.blue = 0.0f;
        vertex.green = 1.0f;
        vertex.red = 0.0f;
        break;
    case 3:
        vertex.blue = 0.0f;
        vertex.green = 0.0f;
        vertex.red = 1.0f;
        break;    
    case 4:
        vertex.blue = 1.0f;
        vertex.green = 0.5f;
        vertex.red = 0.0f;
        break;    
    case 5:
        vertex.blue = 1.0f;
        vertex.green = 0.0f;
        vertex.red = 0.5f;
        break;    
    case 6:
        vertex.blue = 0.5f;
        vertex.green = 0.5f;
        vertex.red = 1.0f;
        break;    
    case 7:
        vertex.blue = 1.0f;
        vertex.green = 0.5f;
        vertex.red = 1.0f;
        break;    
    case 8:
        vertex.blue = 1.0f;
        vertex.green = 1.0f;
        vertex.red = 0.5f;
        break;
    default:
        break;
    }
}

void process_moviment() {


    if (keys[GLFW_KEY_UP] && !keys[GLFW_KEY_DOWN] && y_location < 0.5f) {
        y_location += 0.05f;
    }

    if (!keys[GLFW_KEY_UP] && keys[GLFW_KEY_DOWN] && y_location > -0.5f) {
        y_location -= 0.05f;
    }

    if (keys[GLFW_KEY_LEFT] && !keys[GLFW_KEY_RIGHT] && x_location > -0.5f) {
        x_location -= 0.05f;
    }

    if (!keys[GLFW_KEY_LEFT] && keys[GLFW_KEY_RIGHT] && x_location < 0.5f) {
        x_location += 0.05f;
    }
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void reshape_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void init_colors() {
    vertex.blue = 1.0f;
    vertex.green = 0.0f;
    vertex.red = 0.0f;
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
            << "Shaders:: uniforms" << "    "
            << "FPS: " << fps << "    "
            << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(window, outs.str().c_str());

        // Reset for next average.
        frameCount = 0;
    }

    frameCount++;
}
