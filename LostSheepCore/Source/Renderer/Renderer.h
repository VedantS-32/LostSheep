#pragma once

typedef struct Clay_RenderCommand Clay_RenderCommand;

void InitRenderer();

void BeginRendering();

void EndRendering();

void OnUpdateRenderer(float deltaTime);

void RenderRectangle(Clay_RenderCommand* cmd);

void RenderRectangleRounded(Clay_RenderCommand* cmd);

void RenderBorder(Clay_RenderCommand* cmd);

void RenderText(Clay_RenderCommand* cmd);

void RenderImage(Clay_RenderCommand* cmd);

void StartClipping(Clay_RenderCommand* cmd);

void EndClipping(Clay_RenderCommand* cmd);

void RenderCustomElement(Clay_RenderCommand* cmd);

void ShutdownRenderer();