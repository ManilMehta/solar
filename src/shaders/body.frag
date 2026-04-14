#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int  isSun;

out vec4 FragColor;

void main() {
    if (isSun == 1) {
        FragColor = vec4(objectColor * 1.2, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // ambient
    float ambientStrength = 0.08;
    vec3 ambient = ambientStrength * objectColor;

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * objectColor;

    // specular
    float specStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specStrength * spec * vec3(1.0);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
