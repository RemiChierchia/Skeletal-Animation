#pragma once

#include "VertexBuffer.h"
//#include "VertexBufferLayout.h" //circular inclusion
class VertexBufferLayout;

class VertexArray
{
private:
	unsigned int m_RendererID;

	//unsigned int offset = 0;
	unsigned int previousArray = 0;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void AddBuffer(const VertexBuffer& vb);
	void AddNormalsBuffer(const VertexBuffer& vb);

	void Bind() const;
	void UnBind() const;
};
