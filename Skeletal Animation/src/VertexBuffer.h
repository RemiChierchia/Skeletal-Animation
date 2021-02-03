#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;

	unsigned int dimension;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
	void Update(const void* data, unsigned int size);

	const unsigned int getSize() const { return dimension; };
};
