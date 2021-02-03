#include "VertexBuffer.h"

#include"Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) : dimension(size)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); //buffer id binded or selected
    //glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); //static means that I can access multiple times to positions var but define it ones
    // set GL_DYNAMIC_DRAW for keep updating positions, etc...
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void  VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Update(const void* data, unsigned int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    //glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    //glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
