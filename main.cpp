/*Copyright [2018] <Tihran Katolikian>*/

#include <glad/glad.h>
// ------------------------------
// this define is needed because we use
// dynamic glfw3 library instead of static.
#define GLFW_DLL
#include <GL/glfw3.h>

#include <iostream>
#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "Model.hpp"
#include "LightCaster.h"

namespace GL
{  
// ------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);    
    
// screen settings
unsigned int screen_w = 800;
unsigned int screen_h = 600;

// initialize FPS-like camera in a position
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// variables for knowing previous positions
float last_x = screen_w / 2.0f;
float last_y = screen_h / 2.0f;

//todo...
bool first_mouse = true;

// variables for setting up convenient camera moving speed depending on
// processing speed
float delta_time = 0.0f;
float last_frame = 0.0f;

// positions of point light sources
std::vector <glm::vec3> light_positions({glm::vec3(1.2f, 1.0f, 2),
                                        glm::vec3(-1.2f, 1.0f, 2)});
//-------------------------------
// for directional lighting
glm::vec3 light_direction(-1, -1, -1);
}

int main() 
{
    // ------------------------------
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ------------------------------
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(GL::screen_w, GL::screen_h,
                                          "Rendering Sylvanas Windrunner", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return 0;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, GL::framebufferSizeCallback);
    glfwSetCursorPosCallback(window, GL::mouseCallback);
    glfwSetScrollCallback(window, GL::scrollCallback);

    // ------------------------------
    // setting up mouse input mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ------------------------------
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return 0;
    }

    //-------------------------------
    // enable stencill test
    glEnable(GL_DEPTH_TEST);

    // ------------------------------
    // create shader program object for sylvanas
    Shader sylvanas_shader("SylvanasVS.vs", "SylvanasFS.fs");
    
    // ------------------------------
    // this is out Sylvanas model, loaded via
    // assimp
    Model sylvanas_model("resources/sylvanas.obj");
    
    
    LightCaster lc1;
    lc1.setPosition(GL::light_positions.at(0));
    lc1.setAttenuation({1, 0.3, 0.032});
    lc1.setAmbient({1.f, 1.f, 0.f});
    lc1.setDiffuse({1.f, 1.f, 0.f});
    lc1.setSpecular({1.f, 1.f, 0.f});
    
    LightCaster lc2;
    lc2.setPosition(GL::light_positions.at(1));
    lc2.setAttenuation({1, 0.3, 0.032});
    lc2.setAmbient({0.f, 0.f, 1.f});
    lc2.setDiffuse({0.f, 0.f, 1.f});
    lc2.setSpecular({0.f, 0.f, 1.f});
    
    lc1.setUpForShader(sylvanas_shader, 0);
    lc2.setUpForShader(sylvanas_shader, 1);
    
    //-------------------------------
    // we should inform out fragment shader about
    // the actual number of light casters we use
    sylvanas_shader.setInt("current_lights_num", 2);

    //-------------------------------
    // set clear color to dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    // ------------------------------
    // render loop starts here
    while (!glfwWindowShouldClose(window)) {
        // ------------------------------
        // per-frame time logic
        float currentFrame = glfwGetTime();
        GL::delta_time = currentFrame - GL::last_frame;
        GL::last_frame = currentFrame;

        GL::processInput(window);

        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // ------------------------------
        // updating view and projection matrices (uniform fields) in vertex
        // shader
        glm::mat4 projection = glm::perspective(glm::radians(GL::camera.getZoom()),
                                                static_cast <float>
                                                (GL::screen_w) /
                                                static_cast <float>
                                                (GL::screen_h),
                                                0.1f, 100.0f);
        glm::mat4 view = GL::camera.getViewMatrix();

        glm::mat4 model;
        model = glm::translate(model, {0, -0.5f, 0});
        
        // ------------------------------
        // rendering the sylvanas
        sylvanas_shader.use();
        sylvanas_shader.setVec3("viewer_pos", GL::camera.getPosition());
        sylvanas_shader.setFloat("material.shininess", 8.f);

        sylvanas_shader.setMVP(model, view, projection);

        glStencilMask(0x00);
        
        sylvanas_model.draw(sylvanas_shader);
        
        model = glm::translate(model, {-1, 0, 0});
        model = glm::rotate(model, glm::radians(90.f), {0, 1, 0});
        sylvanas_shader.setMat4("model", model);
        
        sylvanas_model.draw(sylvanas_shader);
        
        model = glm::mat4();
        model = glm::translate(model, {1, -0.5f, 0});
        model = glm::rotate(model, glm::radians(180.f), {0, 1, 0});
        sylvanas_shader.setMat4("model", model);

        sylvanas_model.draw(sylvanas_shader);

        // ------------------------------
        // glfw: swap buffers and poll IO events (keys pressed/released,
        // mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// ------------------------------
// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
void GL::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        GL::camera.processKeyboard(Camera::FORWARD, GL::delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        GL::camera.processKeyboard(Camera::BACKWARD, GL::delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        GL::camera.processKeyboard(Camera::LEFT, GL::delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        GL::camera.processKeyboard(Camera::RIGHT, GL::delta_time);
}

// ------------------------------
// glfw: whenever the window size changed (by OS or user resize) this
// callback function executes
void GL::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // ------------------------------
    // make sure the viewport matches the new window dimensions; note that
    // width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    GL::screen_h = height;
    GL::screen_w = width;
}

// ------------------------------
// glfw: whenever the mouse moves, this callback is called
void GL::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (GL::first_mouse) {
        GL::last_x = xpos;
        GL::last_y = ypos;
        GL::first_mouse = false;
    }

    float xoffset = xpos - GL::last_x;
    // reversed since y-coordinates go from bottom to top
    float yoffset = GL::last_y - ypos;

    GL::last_x = xpos;
    GL::last_y = ypos;

    GL::camera.processMouseMovement(xoffset, yoffset);
}

// ------------------------------
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void GL::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    GL::camera.processMouseScroll(yoffset);
}
