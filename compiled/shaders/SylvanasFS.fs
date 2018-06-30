#version 330 core

out vec4 frag_color;
in vec2 tex_coords;
in vec3 frag_pos;
in vec3 normal;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

struct PointLight
{
    vec3 position;
    vec3 attenuation;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

uniform Material material;
uniform vec3 viewer_pos;

uniform DirLight dlight;

//-----------------------------------
// we create a static array of light positions.
// so upper bound of light sources number is
// 32
uniform PointLight plight[32];
//-----------------------------------
// current lights value will change every time
// we use "setUpForShader()" function of class
// LightCaster. It should never be more then
// 32
uniform int current_lights_num;

//-----------------------------------
// function calculates the direcional light component of exact direcional
// light source, using normal parameter for diffuse lighting component
// calculating and view_dir parameter for specular.
vec3 calcDirLight(const DirLight dlight, const vec3 normal, const vec3 view_dir);

//-----------------------------------
// function calculates point light component
vec3 calcPointLight(const PointLight light, const vec3 normal,
                    const vec3 frag_pos, const vec3 view_dir);

void main()
{
    vec3 view_dir = normalize(viewer_pos - frag_pos);
    vec3 norm = normalize(normal);
	vec3 result = calcDirLight(dlight, norm, view_dir);
    for (int i = 0; i < current_lights_num; ++i) {
        result += calcPointLight(plight[i], norm, frag_pos, view_dir);
    }
	frag_color = vec4(result, 1);
}

vec3 calcDirLight(const DirLight dlight, const vec3 normal, const vec3 view_dir)
{
    // ambient component
    vec3 ambient = dlight.ambient * texture(material.texture_diffuse1, tex_coords).rgb;
    
    // calculate light dir. It is required for diffuse component calculating.
    vec3 light_dir = normalize(-dlight.direction);
    vec3 diffuse = max(dot(light_dir, normal), 0) * dlight.diffuse * texture(material.texture_diffuse1, tex_coords).rgb;
    
    //calculating reflect direction required for next specular component calc.
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(reflect_dir, view_dir), 0), material.shininess);
    vec3 specular = texture(material.texture_specular1, tex_coords).rgb * spec * dlight.specular;
    
    return ambient + diffuse + specular;
}

vec3 calcPointLight(const PointLight plight, const vec3 normal,
                    const vec3 frag_pos, const vec3 view_dir)
{
    vec3 light_dir = normalize(plight.position - frag_pos);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    // attenuation
    float distance = length(plight.position - frag_pos);
    float attenuation = 1.f / (plight.attenuation.x + plight.attenuation.y * distance +
  			                   plight.attenuation.z * (distance * distance));
    // combine results
    vec3 ambient  = plight.ambient * texture(material.texture_diffuse1, tex_coords).rgb;
    vec3 diffuse  = plight.diffuse * diff * vec3(texture(material.texture_diffuse1, tex_coords));
    vec3 specular = plight.specular * spec * vec3(texture(material.texture_specular1, tex_coords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}