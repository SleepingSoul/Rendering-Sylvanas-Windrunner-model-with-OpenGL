/*Copyright [2018] <Tihran Katolikian>*/
// class LightCaster implements the light
// caster interface

#ifndef LIGHT_CASTER
#define LIGHT_CASTER

#include <glm/glm.hpp>
#include "shader.hpp"
#include "LightCaster.h"

class LightCaster
{
public:
    LightCaster();
    ~LightCaster();
    //---------------------------
    // setters
    void setPosition(const glm::vec3 &new_pos);
    void setAttenuation(const glm::vec3 &new_attenuation);
    void setAmbient(const glm::vec3 &new_ambient);
    void setDiffuse(const glm::vec3 &new_diffuse);
    void setSpecular(const glm::vec3 &new_specular);
    
    //---------------------------
    // getters
    glm::vec3 getPosition() const;
    glm::vec3 getAttenuation() const;
    glm::vec3 getAmbient() const;
    glm::vec3 getDiffuse() const;
    glm::vec3 getSpecular() const;
    
    //---------------------------
    // function sets up shader uniform fields
    // to the values from this class
    void setUpForShader(Shader &shader, const unsigned index) const;
private:
    //---------------------------
    // position of light caster in
    // global OpenGL coordinates
    glm::vec3 position;
    
    //---------------------------
    // attenuation vector represents
    // 3 attenuation factor of point
    // light source
    glm::vec3 attenuation;
    
    //---------------------------
    // 3 light components of Phong
    // light model
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    //---------------------------
    // this is the static counter of light caster instances which will
    // help to keep light_casters_num uniform value corrent in the
    // fragment shader
    static unsigned light_casters_num;
};

#endif // LIGHT_CASTER
