#include "Renderer.h"

#include "Core/Log.h"
#include "Core/Window.h"
#include "Event/Event.h"
#include "Renderer/Shader.h"

#include "glad/glad.h"

#include "cglm/cglm.h"

#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

static int s_ZIndex = 0;

static float vertices[] = {
     1.0f,  1.0f,  // top right
     1.0f,  0.0f,  // bottom right
     0.0f,  0.0f,  // bottom left
     0.0f,  1.0f,  // top left
};
static unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

static unsigned int VAO;
static unsigned int VBO;
static unsigned int IBO;

static float s_ZNear = -1000.0f;
static float s_ZFar = 1000.0f;

static mat4 s_ProjectionMatrix;
static mat4 s_ViewMatrix;
static mat4 s_ViewProjectionMatrix;

static int OnWindowResize(Event* event)
{
    int width = ((int*)event->Data)[0];
    int height = ((int*)event->Data)[1];

    glm_mat4_identity(s_ViewMatrix);
    glm_ortho(0.0f, (float)width, (float)height, 0.0f, s_ZNear, s_ZFar, s_ProjectionMatrix);
    glm_mat4_mul(s_ProjectionMatrix, s_ViewMatrix, s_ViewProjectionMatrix);

	glViewport(0, 0, width, height);
    return 0;
}

void InitRenderer()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    InitShaders();

	const WindowData* windowData = GetWindowData();
	glViewport(0, 0, windowData->Width, windowData->Height);
    glm_mat4_identity(s_ViewMatrix);
    glm_ortho(0.0f, (float)windowData->Width, (float)windowData->Height, 0.0f, s_ZNear, s_ZFar, s_ProjectionMatrix);

	glm_mat4_mul(s_ProjectionMatrix, s_ViewMatrix, s_ViewProjectionMatrix);

	UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);
}

void BeginRendering()
{
    s_ZIndex = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndRendering()
{
}

void OnUpdateRenderer(float deltaTime)
{
    const WindowData* windowData = GetWindowData();

    glm_ortho(0.0f, (float)windowData->Width, (float)windowData->Height, 0.0f, s_ZNear, s_ZFar, s_ProjectionMatrix);
    glm_mat4_mul(s_ProjectionMatrix, s_ViewMatrix, s_ViewProjectionMatrix);
	UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);
}

void OnEventRenderer(Event* event)
{
	DispatchEvent(EventTypeWindowResize, event, OnWindowResize);
}

void RenderRectangle(Clay_RenderCommand* cmd)
{
    const WindowData* windowData = GetWindowData();

    Clay_BoundingBox bbox = cmd->boundingBox;
    Clay_BorderRenderData border = cmd->renderData.border;

    // Draw rectangle with background color and corner radius
    //DrawRectangleRounded(
    //    (Rectangle) {
    //    bbox.x, bbox.y, bbox.width, bbox.height
    //},
    //    config->cornerRadius,
    //    config->backgroundColor
    //);

 //   LSH_TRACE("RenderRectangle: (%.2f, %.2f, %.2f, %.2f) Color: (%.2f, %.2f, %.2f, %.2f)", bbox.x, bbox.y, bbox.width, bbox.height,
	//	border.color.r, border.color.g, border.color.b, border.color.a);

	//LSH_ERROR("Z-Index: %d", s_ZIndex);

	vec4 color = {border.color.r, border.color.g, border.color.b, border.color.a};

    UploadUniform3f("uQuadPos", &(vec3){ bbox.x, bbox.y, (float)s_ZIndex++});
	UploadUniform2f("uQuadSize", &(vec2) { bbox.width, bbox.height });
    UploadUniform4f("uColor", &color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
