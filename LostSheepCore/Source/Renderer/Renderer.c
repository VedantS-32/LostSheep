#include "Renderer.h"

#include "Core/Log.h"
#include "Core/Window.h"

#include "Event/Event.h"

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Text.h"

#include "UI/UI.h"

#include "Math/Types.h"

#include "glad/glad.h"

#include "cglm/cglm.h"

#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

static int s_ZIndex = 0;

static float vertices[] = {
    // Coords      // TexCoords
     1.0f,  1.0f,   1.0f,  1.0f,  // top right
     1.0f,  0.0f,   1.0f,  0.0f,  // bottom right
     0.0f,  0.0f,   0.0f,  0.0f,  // bottom left
     0.0f,  1.0f,   0.0f,  1.0f,  // top left
};
static unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

static unsigned int s_VAO;
static unsigned int s_CommonVBO;
static unsigned int s_TextVBO;
static unsigned int s_IBO;

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

    glCreateVertexArrays(1, &s_VAO);
    glBindVertexArray(s_VAO);

    glGenBuffers(1, &s_IBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glCreateBuffers(1, &s_CommonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_CommonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glCreateBuffers(1, &s_TextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_TextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    BindCommonVBO();

    InitShader();
    InitTexture();
    InitText();

	const WindowData* windowData = GetWindowData();
	glViewport(0, 0, windowData->Width, windowData->Height);
    glm_mat4_identity(s_ViewMatrix);
    glm_ortho(0.0f, (float)windowData->Width, (float)windowData->Height, 0.0f, s_ZNear, s_ZFar, s_ProjectionMatrix);

	glm_mat4_mul(s_ProjectionMatrix, s_ViewMatrix, s_ViewProjectionMatrix);

	UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);

    InitUI();

    LSH_TRACE("Renderer initialized");
}

void BeginRendering()
{
    s_ZIndex = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndRendering()
{
}

void BindCommonVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, s_CommonVBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void BindTextVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, s_TextVBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void OnUpdateRenderer(float deltaTime)
{
    const WindowData* windowData = GetWindowData();

    glm_ortho(0.0f, (float)windowData->Width, (float)windowData->Height, 0.0f, s_ZNear, s_ZFar, s_ProjectionMatrix);
    glm_mat4_mul(s_ProjectionMatrix, s_ViewMatrix, s_ViewProjectionMatrix);

    OnUpdateUI(deltaTime);
}

void OnEventRenderer(Event* event)
{
	DispatchEvent(EventTypeWindowResize, event, OnWindowResize);
    OnEventUI(event);
}

void RenderRectangle(Clay_RenderCommand* cmd)
{
    SetActiveShader(UIShaderType_Rectangle);

    Clay_BoundingBox bbox = cmd->boundingBox;
    Clay_RectangleRenderData rectangle = cmd->renderData.rectangle;

    LSHVec3 position = { bbox.x, bbox.y, (float)s_ZIndex++ };

    LSHVec4 color = {
        rectangle.backgroundColor.r,
        rectangle.backgroundColor.g,
        rectangle.backgroundColor.b,
        rectangle.backgroundColor.a
    };

    UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);

    UploadUniform3f("uQuadPos", &position);
	UploadUniform2f("uQuadSize", &(LSHVec2) { bbox.width, bbox.height });
    UploadUniform4f("uColor", &color);
    UploadUniform1f("uCornerRadius", rectangle.cornerRadius.topRight);
    UploadUniform1f("uBorderThickness", 0.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderRectangleRounded(Clay_RenderCommand* cmd)
{
}

void RenderBorder(Clay_RenderCommand* cmd)
{
    SetActiveShader(UIShaderType_Rectangle);

    Clay_BoundingBox bbox = cmd->boundingBox;
    Clay_BorderRenderData border = cmd->renderData.border;
    Clay_RectangleRenderData rectangle = cmd->renderData.rectangle;
    
    LSHVec3 position = { bbox.x, bbox.y, (float)s_ZIndex++ };

	LSHVec4 color = { 1.0f, 0.0f, 1.0f, 0.0f };

    if (rectangle.cornerRadius.topRight > 0.0f)
    {
        color.r = border.color.r;
        color.g = border.color.g;
        color.b = border.color.b;
        color.a = border.color.a;
    }

    UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);

    UploadUniform3f("uQuadPos", &position);
    UploadUniform2f("uQuadSize", &(LSHVec2) { bbox.width, bbox.height });
    UploadUniform4f("uColor", &color);
    UploadUniform1f("uCornerRadius", rectangle.cornerRadius.topRight);
    UploadUniform1f("uBorderThickness", border.width.top);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderText(Clay_RenderCommand* cmd)
{
    SetActiveShader(UIShaderType_Text);

    Clay_BoundingBox bbox = cmd->boundingBox;
    Clay_TextRenderData textData = cmd->renderData.text;
    const char* text = textData.stringContents.chars;

    LSHVec2 position = { 0.0f, 0.0f };
    position.x = bbox.x;
    position.y = bbox.y;

    LSHVec2 bboxDim = { 0.0f, 0.0f };
    bboxDim.x = bbox.width;
    bboxDim.y = bbox.height;

    LSHVec4 color = { 1.0f, 0.0f, 1.0f, 1.0f };
    color.r = textData.textColor.r;
    color.g = textData.textColor.g;
    color.b = textData.textColor.b;
    color.a = textData.textColor.a;

    //LSH_INFO(text);

    UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);

    UploadUniform1f("uQuadPosZ", (float)s_ZIndex++);

    BindTextVBO();
    RenderTextLine(text, textData.stringContents.length, &position, &bboxDim, textData.fontSize, &color);
    BindCommonVBO();
}

void RenderImage(Clay_RenderCommand* cmd)
{
    SetActiveShader(UIShaderType_Image);

    Clay_BoundingBox bbox = cmd->boundingBox;
    Clay_ImageRenderData image = cmd->renderData.image;
    Clay_RectangleRenderData rectangle = cmd->renderData.rectangle;

    LSHVec3 position = { bbox.x, bbox.y, (float)s_ZIndex++ };

    int textureRendererID = *((TextureName*)(image.imageData));
    BindActiveTexture(textureRendererID, 0);

    //vec4 backgroundColor = { 1.0f, 0.0f, 1.0f, 1.0f };

    //if (image.cornerRadius.topRight > 0.0f)
    //{
    //    backgroundColor[0] = image.backgroundColor.r;
    //    backgroundColor[1] = image.backgroundColor.g;
    //    backgroundColor[2] = image.backgroundColor.b;
    //    backgroundColor[3] = image.backgroundColor.a;
    //}

    UploadUniformMat4f("uViewProjection", &s_ViewProjectionMatrix);

    UploadUniform3f("uQuadPos", &position);
    UploadUniform2f("uQuadSize", &(LSHVec2) { bbox.width, bbox.height });
    UploadUniform1i("uTexture", textureRendererID);
    UploadUniform1f("uCornerRadius", rectangle.cornerRadius.topRight);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    ShutdownText();
    ShutdownUI();
    ShutdownTexture();
    ShutdownShader();

    LSH_TRACE("Shutdown renderer");
}
