/* Copyright Joey de Vries 
   (original code : https://github.com/JoeyDeVries/LearnOpenGL)
   Modified by Tihran Katolikian 05.07.2018
   Updates:
   - code style changed;
   - member values incapsulated, getters and setters created;
   - enumeration and default values incapsulated;
   - few tiny optimizations (referense passing in constructors, if/else,
     changed if if if on switch case);
   - now constructors initialize all member values. Before, fields
     were initialized by default constructors, and then was redeclared */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <stdexcept>
#include <iostream>

// -------------------------
// A camera class that processes input and calculates
// the corresponding Euler Angles, Vectors and Matrices for use
// in OpenGL
class Camera
{
public:
    // ------------------------
    // Defines several possible options for camera movement.
    // Used as abstraction to stay away from window-system
    // specific input methods
    enum Movement {FORWARD, BACKWARD, LEFT, RIGHT};
    
    /*
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    
    // ------------
    // Euler Angles
    float yaw;
    float pitch;
    
    // --------------
    // Camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;*/
    
    // ------------------------
    // Constructor with vectors
    Camera(const glm::vec3 &init_position = {0.0f, 0.0f, 0.0f}, 
           const glm::vec3 &init_up = {0.0f, 1.0f, 0.0f},
           const float init_yaw = def_yaw,
           const float init_pitch = def_pitch)
	: position(init_position), front(0.0f, 0.0f, -1.0f),
      up(init_up), world_up(init_up), yaw(init_yaw),
      pitch(init_pitch), movement_speed(def_speed),
      mouse_sensitivity(def_sensitivity), zoom(def_zoom)
    {
        updateCameraVectors();
    }
    
    //-------------------------
    // Constructor with scalar values
    Camera(const float init_pos_x, const float init_pos_y,
           const float init_pos_z, const float init_up_x,
           const float init_up_y, const float init_up_z,
           const float init_yaw, const float init_pitch)
    : position(init_pos_x, init_pos_y, init_pos_z),
      front(0.0f, 0.0f, -1.0f),
      up(init_up_x, init_up_y, init_up_z),
      world_up(init_up_x, init_up_y, init_up_z),
      yaw(init_yaw), pitch(init_pitch),
      movement_speed(def_speed),
      mouse_sensitivity(def_sensitivity), zoom(def_zoom)
    {
        updateCameraVectors();
    }

    ~Camera() = default;

    // ------------------------
    // Returns the view matrix calculated using 
    // Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    // ------------------------
    // Processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void processKeyboard(const Movement direction, const float delta_time)
    {
        float velocity = movement_speed * delta_time;
        switch (direction) {
            case FORWARD:
                position += front * velocity;
                break;
            case BACKWARD:
                position -= front * velocity;
                break;
            case LEFT:
                position -= right * velocity;
                break;
            case RIGHT:
                position += right * velocity;
                break;
            default:
                throw std::runtime_error("Uninitialized direction.\n");
        }
    }

    // ------------------------
    // Processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void processMouseMovement(const float xoffset, const float yoffset,
                              const bool constrain_pitch = true)
    {
        yaw += xoffset * mouse_sensitivity;
        pitch += yoffset * mouse_sensitivity;

        // -----------------------
        // Make sure that when pitch is out of bounds, screen
        // doesn't get flipped
        if (constrain_pitch) {
            if (pitch > 89.f)
                pitch = 89.f;
            else if (pitch < -89.f)
                pitch = -89.f;
        }

        // ----------------------
        // Update front, right and up vectors using the updated Euler angles
        updateCameraVectors();
    }

    // -----------------------------
    // Processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void processMouseScroll(const float yoffset)
    {
        if (zoom >= 1.f && zoom <= 45.f)
            zoom -= yoffset;
        if (zoom <= 1.f)
            zoom = 1.f;
        if (zoom >= 45.f)
            zoom = 45.f;
    }
    
    // -----------------------------
    // getters and setters
    glm::vec3 getPosition() const
    {
        return position;
    }
    
    glm::vec3 getFront() const
    {
        return front;
    }
    
    glm::vec3 getUp() const
    {
        return up;
    }
    
    glm::vec3 getRight() const
    {
        return right;
    }
    
    glm::vec3 getWorldUp() const
    {
        return world_up;
    }
    
    float getYaw() const
    {
        return yaw;
    }
    
    float getPitch() const
    {
        return pitch;
    }
    
    float getMovementSpeed() const
    {
        return movement_speed;
    }
    
    float getMouseSensitivity() const
    {
        return mouse_sensitivity;
    }
    
    float getZoom() const
    {
        return zoom;
    }
    
    void setPosition(const glm::vec3 &new_position)
    {
        position = new_position;
    }
    
    void setFront(const glm::vec3 &new_front)
    {
        front = new_front;
    }
    
    void setUp(const glm::vec3 &new_up)
    {
        up = new_up;
    }
    
    void setRight(const glm::vec3 &new_right)
    {
        right = new_right;
    }
    
    void setWorldUp(glm::vec3 &new_world_up)
    {
        world_up = new_world_up;
    }
    
    void setYaw(const float new_yaw)
    {
        yaw = new_yaw;
    }
    
    void setPitch(const float new_pitch)
    {
        pitch = new_pitch;
    }
    
    void setMovementSpeed(const float new_mov_speed)
    {
        movement_speed = new_mov_speed;
    }
    
    void setMouseSensitivity(const float new_mouse_sens)
    {
        mouse_sensitivity = new_mouse_sens;
    }
    
    void setZoom(const float new_zoom)
    {
        zoom = new_zoom;
    }

private:
    // ---------------------
    // Default camera values
    inline static const float def_yaw = -90.f;
    inline static const float def_pitch = 0.f;
    inline static const float def_speed = 2.5f;
    inline static const float def_sensitivity = 0.1f;
    inline static const float def_zoom = 45.0f;
    
    // -----------------
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    
    // ------------
    // Euler Angles
    float yaw;
    float pitch;
    
    // --------------
    // Camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
    
    // -----------------
    // Calculates the front vector from the Camera's
    // (updated) Euler Angles
    void updateCameraVectors()
    {
        // --------------------
        // Calculate the new Front vector
        front = glm::vec3{std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
                          std::sin(glm::radians(pitch)),
                          std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))};

        front = glm::normalize(front);

        // --------------------------
        // Also re-calculate the right and up vector
        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif  //CAMERA_HPP
