#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>

#include <string.h>
#include <math.h>

#include "math.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool isCursorVisible = false;

float cameraRotX = 0.f;
float cameraRotY = 0.f;
float cameraPosX = 0.f;
float cameraPosY = 0.f;
float cameraPosZ = 5.f;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (isCursorVisible) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isCursorVisible = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isCursorVisible = true;
        }
    }
    float cosVel = cos(cameraRotY) * 0.2;
    float sinVel = sin(cameraRotY) * 0.2;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPosZ -= cosVel;
        cameraPosX += sinVel;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPosZ += cosVel;
        cameraPosX -= sinVel;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPosZ += sinVel;
        cameraPosX += cosVel;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPosZ -= sinVel;
        cameraPosX -= cosVel;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPosY += 0.2f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPosY -= 0.2f;   
}

double mouseSensitivity = 0.003;
double mouseLastX = 0;
double mouseLastY = 0;

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    double xOffset = (xPos - mouseLastX) * mouseSensitivity;
    double yOffset = (yPos - mouseLastY) * mouseSensitivity;
    mouseLastX = xPos;
    mouseLastY = yPos;

    cameraRotX += yOffset;
    cameraRotY += xOffset;

    if (cameraRotX < -M_PI * 0.5)
        cameraRotX = -M_PI * 0.5;
    if (cameraRotX > M_PI * 0.5)
        cameraRotX = M_PI * 0.5;
}


const char *vertexShaderSource = R"glsl(
    #version 460 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;

    out vec2 TexCoord;
    out vec3 Normal;
    out vec3 FragPos;

    uniform mat4 posMat;
    uniform mat4 modMat;

    void main()
    {
        gl_Position = posMat * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
        FragPos = vec3(modMat * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(modMat))) * aNormal;
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 460 core

    struct Material {
        vec3 ambient;
        float shininess;
    };

    struct Light {
        vec3 position;
    
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };


    in vec2 TexCoord;
    in vec3 Normal;
    in vec3 FragPos;
    
    uniform vec3 viewPos;

    uniform Material material;
    uniform Light light;


    uniform sampler2D diffuseMap;
    uniform sampler2D specularMap;

    out vec4 FragColor;

    void main()
    {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);

        float specularStrength = 0.5;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = (vec3(texture(specularMap, TexCoord)) * spec) * light.specular;

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffMapSample = vec3(texture(diffuseMap, TexCoord));
        vec3 diffuse = (diff * diffMapSample) * light.diffuse;
        vec3 ambient = light.ambient * diffMapSample;
        
        FragColor = vec4(diffuse + ambient + specular, 1.0);
    }
)glsl";

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &mouseLastX, &mouseLastY);
    glfwSetCursorPosCallback(window, mouse_callback);


    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int indices[36];
    for (int i = 0; i < 36; i++)
        indices[i] = i;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    int width, height, nrChannels;
    unsigned char *data = stbi_load("res/container2.png", &width, &height, &nrChannels, 0);
    
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "Failed to load image\n";
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("res/container2_specular.png", &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "Failed to load image\n";
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);



    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    int positionMatrixLocation = glGetUniformLocation(shaderProgram, "posMat");
    int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modMat");
    int viewPositionLocation = glGetUniformLocation(shaderProgram, "viewPos");

    int lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
    int lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");
    int lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
    int lightPositionLoc = glGetUniformLocation(shaderProgram, "light.position");

    int matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    // int matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    // int matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    int matShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "specularMap"), 1);

    glBindVertexArray(VAO);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    eng::Vec3f light = { 1.0f, 1.0f, 1.0f };
    eng::Vec3f lightPos = { 2.0f, 2.0f, 2.0f };

    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();    

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = glfwGetTime();

        eng::Mat4f proj = eng::Matrix4x4<float>::GL_Projection(90.f, 800, 600, 0.1, 100.f);
        eng::Mat4f rot = eng::Matrix4x4<float>::yRotation(sin(timeValue));
        eng::Mat4f view = eng::Mat4f::xRotation(cameraRotX)
            * eng::Mat4f::yRotation(cameraRotY)
            * eng::Mat4f::translation(-cameraPosX, -cameraPosY, -cameraPosZ);
        eng::Mat4f model = rot;
        eng::Mat4f pos = proj * view * model;

        glUniformMatrix4fv(positionMatrixLocation, 1, false, pos[0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, false, model[0]);
        glUniform3f(viewPositionLocation, cameraPosX, cameraPosY, cameraPosZ);

        glUniform3f(lightPositionLoc, lightPos[0], lightPos[1], lightPos[2]);
        glUniform3f(lightAmbientLoc, 0.2f, 0.3f, 0.3f);
        glUniform3f(lightSpecularLoc, 0.5f, 0.5f, 0.5f);
        glUniform3f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f);

        glUniform3f(matAmbientLoc, 0.2f, 0.3f, 0.3f);
        // glUniform3f(matDiffuseLoc, 1.0f, 0.5f, 0.31f);
        // glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
        glUniform1f(matShininessLoc, 32.0f);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
