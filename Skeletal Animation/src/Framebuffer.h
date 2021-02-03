#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include<iostream>

class Framebuffer
{
public:

    Framebuffer(const bool _antiAliasing, const unsigned int _SCR_WIDTH, const unsigned int _SCR_HEIGHT)
        : antiAliasing (_antiAliasing), SCR_WIDTH (_SCR_WIDTH), SCR_HEIGHT (_SCR_HEIGHT)
    {   
        if (antiAliasing == true) {
            glGenFramebuffers(1, &frameBufferID);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

            glGenTextures(1, &texColorBuffer);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer, 0);

            glGenRenderbuffers(1, &rboDepthStencil);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
            //void glRenderbufferStorageMultisample(GLenum  target, GLsizei samples, GLenum  internalFormat, GLsizei width, GLsizei height); 
                //for anti-aliasing, samples decides the number of img attached for better results
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

            bool status = checkFramebufferStatus(frameBufferID);
            if (!status)
                std::cout << "scemo" << std::endl;

            glGenFramebuffers(1, &intermediateframeBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, intermediateframeBuffer);

            // Create texture to hold color buffer
            glGenTextures(1, &intermediatetexColorBuffer);
            glBindTexture(GL_TEXTURE_2D, intermediatetexColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //GL_RGBA8 GL_RGBA

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediatetexColorBuffer, 0);

            bool statusint = checkFramebufferStatus(intermediateframeBuffer);
            if (!statusint)
                std::cout << "scemo.2" << std::endl;
        }
        else {
            glGenFramebuffers(1, &frameBufferID);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

            glGenTextures(1, &texColorBuffer);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0); //sttaching 2D texture image to FBO

        
            glGenRenderbuffers(1, &rboDepthStencil);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
            //void glRenderbufferStorageMultisample(GLenum  target, GLsizei samples, GLenum  internalFormat, GLsizei width, GLsizei height); 
                //for anti-aliasing, samples decides the number of img attached for better results
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
            
            bool status = checkFramebufferStatus(frameBufferID);
            if (!status)
                std::cout << "scemo" << std::endl;
        }
    }

    ~Framebuffer(){}

    const GLfloat* getQuadVertices() const          { return quadVertices; }
    const GLuint getQuadVerticesSize() const        { return sizeof(quadVertices); }
    const GLuint getFrameBufferID() const           { return frameBufferID; }
    const GLuint getTexColorBuffer() const          { return texColorBuffer; }
    const GLuint getRboDepthStencil() const         { return rboDepthStencil; }
    const GLuint getIntermediateframeBuffer() const { return intermediateframeBuffer; }

    void Draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Bind()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
    }

    void Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    }

    void SetAntiAliasing()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    }

    void BindAntiAliasing()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateframeBuffer);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, intermediatetexColorBuffer);
        //glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    }

    bool checkFramebufferStatus(GLuint fbo)
    {
        // check FBO status
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); // bind
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status)
        {
        case GL_FRAMEBUFFER_COMPLETE:
            std::cout << "Framebuffer complete." << std::endl;
            return true;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "[ERROR] Framebuffer incomplete: Multisample." << std::endl;
            return false;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
            return false;

        default:
            std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);   // unbind
    }

private:

    bool antiAliasing;
    unsigned int SCR_WIDTH, SCR_HEIGHT;

    GLuint frameBufferID, texColorBuffer, rboDepthStencil, intermediateframeBuffer, intermediatetexColorBuffer;

    GLfloat quadVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

};
