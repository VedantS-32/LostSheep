#include "Renderer.h"

#include "Core/Log.h"

#include "glad/glad.h"

#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.00f, 0.51f, 0.65f, 1.0f);\n"
"}\0";


unsigned int VAO;
unsigned int VBO;
unsigned int IBO;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

void InitRenderer()
{
    glEnable(GL_DEPTH_TEST);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LSH_ERROR("SHADER::VERTEX::COMPILATION_FAILED, %s", infoLog);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LSH_ERROR("SHADER::FRAGMENT::COMPILATION_FAILED, %s", infoLog);
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void BeginRendering()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndRendering()
{
}

void OnUpdateRenderer(float deltaTime)
{
}

void RenderRectangle(Clay_RenderCommand* cmd)
{
    Clay_BoundingBox bbox = cmd->boundingBox;
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw rectangle with background color and corner radius
    //DrawRectangleRounded(
    //    (Rectangle) {
    //    bbox.x, bbox.y, bbox.width, bbox.height
    //},
    //    config->cornerRadius,
    //    config->backgroundColor
    //);
}

void RenderRectangleRounded(Clay_RenderCommand* cmd)
{
}

void RenderBorder(Clay_RenderCommand* cmd)
{
}

void RenderText(Clay_RenderCommand* cmd)
{
}

void RenderImage(Clay_RenderCommand* cmd)
{
}

void StartClipping(Clay_RenderCommand* cmd)
{
}

void EndClipping(Clay_RenderCommand* cmd)
{
}

void RenderCustomElement(Clay_RenderCommand* cmd)
{
}

void ShutdownRenderer()
{
}
