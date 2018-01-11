#version 330 core

out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

 
uniform sampler2D texture_diffuse1;

void main()
{    
    // ambient
    vec3 ambient = vec3(0.1,0.1,0.1) * texture(texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(-3.5f, 4.0f, -5.0f) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.7,0.7,0.7) * diff * texture(texture_diffuse1, TexCoords).rgb;  
         
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
