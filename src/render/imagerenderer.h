#pragma once

#include "renderer.h"
#include "gle/gle.h"



class ImageRenderer : public Renderer {

public:

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

private:

    GLE::ShaderProgram imageShader;
    GLE::VertexBuffer imageVBO;
    GLE::VertexArray imageVAO;

    GLE::Texture2D imageTexture;
    GLE::Uniform imageTextureUnitUniform;

};