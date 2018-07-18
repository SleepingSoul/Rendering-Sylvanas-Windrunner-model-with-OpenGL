/*Copywrite [2018] <Tihran Katolikian>*/

#include <string>
#include "LightCaster.h"

unsigned LightCaster::light_casters_num = 0;

LightCaster::LightCaster()
{
    ++light_casters_num;
}

LightCaster::~LightCaster()
{
    --light_casters_num;
}

void LightCaster::setPosition(const glm::vec3 &new_pos)
{
    position = new_pos;
}

void LightCaster::setAttenuation(const glm::vec3 &new_attenuation)
{
    attenuation = new_attenuation;
}

void LightCaster::setAmbient(const glm::vec3 &new_ambient)
{
    ambient = new_ambient;
}

void LightCaster::setDiffuse(const glm::vec3 &new_diffuse)
{
    diffuse = new_diffuse;
}

void LightCaster::setSpecular(const glm::vec3 &new_specular)
{
    specular = new_specular;
}

glm::vec3 LightCaster::getPosition() const
{
    return position;
}

glm::vec3 LightCaster::getAttenuation() const
{
    return attenuation;
}

glm::vec3 LightCaster::getAmbient() const
{
    return ambient;
}

glm::vec3 LightCaster::getDiffuse() const
{
    return diffuse;
}

glm::vec3 LightCaster::getSpecular() const
{
    return specular;
}

void LightCaster::setUpForShader(Shader &shader, const unsigned index) const
{
    std::string uniform_address("plight[" + std::to_string(index) + ']');
    shader.use();
    shader.setInt("current_lights_num", light_casters_num);
    shader.setVec3(uniform_address + ".position", position);
    shader.setVec3(uniform_address + ".attenuation", attenuation);
    shader.setVec3(uniform_address + ".ambient", ambient);
    shader.setVec3(uniform_address + ".diffuse", diffuse);
    shader.setVec3(uniform_address + ".specular", specular);
}