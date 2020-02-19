#include "sandbox/graphics/render/FrameBuffer.h"
#include <iostream>

#include "sandbox/graphics/RenderState.h"
#include "sandbox/base/EntityComponent.h"

namespace sandbox {

FrameBuffer::FrameBuffer() : version(-1) {
	addType<FrameBuffer>();
}

void FrameBuffer::update() {
    textures.clear();

    std::vector<Entity*> children = getEntity().getChildren();
    for (int f = 0; f < children.size(); f++) {
        Texture* texture = children[f]->getComponent<Texture>();
        if (texture) {
            textures.push_back(texture);
        }
    }

    std::vector<EntityComponent*> entities = getEntity().getComponents<EntityComponent>();
    for (int f = 0; f < entities.size(); f++) {
        Texture* texture = entities[f]->getEntityReference()->getComponent<Texture>();
        if (texture) {
            textures.push_back(texture);
        }
    }

    version++;
}

void FrameBuffer::updateContext(const GraphicsContext& sceneContext) {
	FrameBufferState& state = *contextHandler.getState(sceneContext);

    if (state.initialized && state.version != version) {
        state.reset();
        state.initialized = false;
        state.version = version;
    }

    if (!state.initialized) {
        std::cout << "INitialize frame buffer context" << std::endl;

        glGenFramebuffers(1, &state.frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, state.frameBuffer);

        for (int f = 0; f < textures.size(); f++) {
            attach(sceneContext, state, *textures[f]);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        state.initialized = true;
    }
}

void FrameBuffer::use(const GraphicsContext& sceneContext) {
	FrameBufferState& state = *contextHandler.getState(sceneContext);

	if (state.initialized && !state.inUse) {
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &state.drawFboId);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &state.readFboId);
        glBindFramebuffer(GL_FRAMEBUFFER, state.frameBuffer);
        glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);

        glGetIntegerv(GL_VIEWPORT, state.oldViewport);
        glGetIntegerv(GL_SCISSOR_BOX, state.oldScissorBox);

        glViewport(0, 0, state.width, state.height);
        glScissor(0, 0, state.width, state.height);
        state.inUse = true;
	}
}

void FrameBuffer::release(const GraphicsContext& sceneContext) {
	FrameBufferState& state = *contextHandler.getState(sceneContext);

	if (state.initialized && state.inUse) {
		glBindFramebuffer(GL_FRAMEBUFFER, state.drawFboId);
        glViewport(state.oldViewport[0], state.oldViewport[1], state.oldViewport[2], state.oldViewport[3]);
        glScissor(state.oldScissorBox[0], state.oldScissorBox[1], state.oldScissorBox[2], state.oldScissorBox[3]);
        state.inUse = false;
	}
}

void FrameBuffer::startRender(const GraphicsContext& sceneContext) {
    use(sceneContext);
}

void FrameBuffer::finishRender(const GraphicsContext& sceneContext) {
	release(sceneContext);
}

void FrameBuffer::attach(const GraphicsContext& context, FrameBufferState& state, Texture& texture) {
    if (state.numAttachments == 0) {
        state.width = texture.getWidth();
        state.height = texture.getHeight();
    }

    state.width = state.width > texture.getWidth() ? texture.getWidth() : state.width;
    state.height = state.height > texture.getHeight() ? texture.getHeight() : state.height;

    GLuint attachmentType = texture.getAttachmentType();
    if (attachmentType == GL_COLOR_ATTACHMENT0) {
        attachmentType += state.numColorAttachments;
        state.drawBuffers.push_back(attachmentType);
        state.numColorAttachments++;
    }

    texture.use(context);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, texture.getTarget(context), texture.getId(context), 0/*mipmap level*/);
    state.numAttachments++;
    texture.release(context);

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        std::cout << "Error: Bad Framebuffer Configuration " << status << std::endl;
    }

    if (state.drawBuffers.size() > 0) {
        glDrawBuffers(state.drawBuffers.size(), &state.drawBuffers[0]);
    }
    else {
        glDrawBuffer(GL_NONE);
    }
}

}