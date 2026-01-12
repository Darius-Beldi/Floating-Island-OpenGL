#version 330 core

// Inputs from vertex shader (interpolated per fragment)
in vec3 fragPos;       // Fragment position in world space
in vec3 fragNormal;    // Surface normal vector
in vec3 fragColor;     // Material color

// Output color
out vec4 FragColor;

// Uniforms - same for all fragments
uniform vec3 viewPos;      // Camera position

// Three light sources
uniform vec3 light1Pos;    
uniform vec3 light1Color;  
uniform vec3 light2Pos;    
uniform vec3 light2Color;  
uniform vec3 light3Pos;    
uniform vec3 light3Color;  

//calculates lighting from one point light using Phong model
vec3 calculateLight(vec3 lightPos, vec3 lightColor, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
    //DIFFUSE LIGHTING
    //makes surfaces brighter when facing the light
    vec3 lightDir = normalize(lightPos - fragPosition);  // Direction to light
    float diff = max(dot(normal, lightDir), 0.0);        // How much surface faces light (0-1)
    vec3 diffuse = diff * lightColor;                    // Apply light color
    
    
    //SPECULAR LIGHTING
    //creates shiny highlights on surfaces
    float specularStrength = 0.1;                        // How shiny the material is
    vec3 reflectDir = reflect(-lightDir, normal);        // Reflection direction
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);  // Shininess 
    vec3 specular = specularStrength * spec * lightColor;
    
    
    //ATTENUATION
    //light weakens with distance
    float distance = length(lightPos - fragPosition);
    float attenuation = 1.0 / (1.0 + 0.5 * distance + 0.3 * (distance * distance));
    
    //applying distance falloff to both diffuse and specular
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}

void main()
{
    //AMBIENT LIGHTING
    //the base lighting that illuminates everything equally
    float ambientStrength = 0.03;                   // How bright the ambient light is
    vec3 ambient = ambientStrength * vec3(0.9, 0.7, 0.8);  // Ambient color (soft purple-pink)
    
    
    // Normalize vectors for calculations
    vec3 norm = normalize(fragNormal);               // Ensure normal is unit length
    vec3 viewDir = normalize(viewPos - fragPos);     // Direction from fragment to camera
    
    
    //Calculate contribution from each light source
    vec3 light1 = calculateLight(light1Pos, light1Color, norm, fragPos, viewDir);
    vec3 light2 = calculateLight(light2Pos, light2Color, norm, fragPos, viewDir);
    vec3 light3 = calculateLight(light3Pos, light3Color, norm, fragPos, viewDir);
    
    
    //Combining all lighting components
    vec3 totalLight = ambient + light1 + light2 + light3;
    
    
    //multiplying lighting by material color to get final result
    vec3 result = totalLight * fragColor;
    
    
    //Output final color (RGB + alpha= 1.0 for opaque)
    FragColor = vec4(result, 1.0);
}
