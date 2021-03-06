#version 330 core

out vec4 FragColor;


uniform samplerCube texture1;
uniform samplerCube durabilityTexture;
uniform sampler2D shadowMap;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight[4];
uniform SpotLight spotLight;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcShadow(vec4 fragPosLightSpace);


float mixFactor = 0.0;
vec4 texColor;
float shadow;

void main()
{  
    // properties
    vec3 norm = normalize(Normal);      
    texColor =  texture(texture1, TexCoords);
    shadow = CalcShadow(FragPosLightSpace); 
    if(texture(texture1, TexCoords).a < 0.1)
    {
        discard;
    }
    if(texture(durabilityTexture, TexCoords).a > 0.9)
    {
        texColor  = texColor * texture(durabilityTexture, TexCoords);
    }
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    if(pointLight[0].constant > 0.9 && pointLight[0].constant < 1.1)
    {
            result += CalcPointLight(pointLight[0], norm, FragPos, viewDir);
    }
    if(pointLight[1].constant > 0.9 && pointLight[1].constant < 1.1)
    {
            result += CalcPointLight(pointLight[1], norm, FragPos, viewDir);
    }
    if(pointLight[2].constant > 0.9 && pointLight[2].constant < 1.1)
    {
        result += CalcPointLight(pointLight[2], norm, FragPos, viewDir);
    }
    if(pointLight[3].constant > 0.9 && pointLight[3].constant < 1.1)
    {
        result += CalcPointLight(pointLight[3], norm, FragPos, viewDir);
    }
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);   
    //FragColor = vec4(result,1.0);
    FragColor = mix(dirLight.diffuse.x*vec4(0.6, 0.6, 0.6, texColor.a),vec4(result,texColor.a),min(8.0/length(spotLight.position - FragPos),1.0));  
}


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * texColor;
    return (ambient + (1.0 - shadow) * (diffuse + specular));
    //return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * texColor;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    //return (ambient + (1.0 - shadow) * (diffuse + specular));
    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * texColor;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    //return (ambient + (1.0 - shadow) * (diffuse + specular));
    return (ambient + diffuse + specular);
}


float CalcShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = 0.003;
    
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(0, 0) * texelSize).r; 
    shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;     

    pcfDepth = texture(shadowMap, projCoords.xy + vec2(0, 1) * texelSize).r; 
    shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;  
    
    pcfDepth = texture(shadowMap, projCoords.xy + vec2(1, 0) * texelSize).r; 
    shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;  
    
    pcfDepth = texture(shadowMap, projCoords.xy + vec2(1, 1) * texelSize).r; 
    shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;  
        
    shadow /= 4.0;
    
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}