#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 strength;

void main()
{  
   // FragColor = vec4(1.0f,1.0f,1.0f,1.0f);  
   
   
   FragColor = strength.x * texture(skybox, TexCoords);
}