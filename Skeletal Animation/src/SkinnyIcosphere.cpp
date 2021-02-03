#include "GL/glew.h"

#include "SkinnyIcosphere.h"
#include "Renderer.h"

#include <iostream>
#include <iomanip>
#include <cmath>


///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
SkinnyIcosphere::SkinnyIcosphere(float radius, int sub) : radius(radius), subdivision(sub), interleavedStride(32)
{
    buildVerticesFlat();
}


///////////////////////////////////////////////////////////////////////////////
// compute 12 vertices of icosahedron using spherical coordinates
// The north pole is at (0, 0, r) and the south pole is at (0,0,-r).
// 5 vertices are placed by rotating 72 deg at elevation 26.57 deg (=atan(1/2))
// 5 vertices are placed by rotating 72 deg at elevation -26.57 deg
///////////////////////////////////////////////////////////////////////////////
std::vector<float> SkinnyIcosphere::computeIcosahedronVertices()
{
    const float PI = acos(-1);
    const float H_ANGLE = PI / 180 * 72;    // 72 degree = 360 / 5
    const float V_ANGLE = atanf(1.0f / 2);  // elevation = 26.565 degree

    std::vector<float> vertices(12 * 3);    // 12 vertices
    int i1, i2;                             // indices
    float z, xy;                            // coords
    float hAngle1 = -PI / 2 - H_ANGLE / 2;  // start from -126 deg at 2nd row
    float hAngle2 = -PI / 2;                // start from -90 deg at 3rd row

    // the first top vertex (0, 0, r)
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = radius;

    // 10 vertices at 2nd and 3rd rows
    for (int i = 1; i <= 5; ++i)
    {
        i1 = i * 3;         // for 2nd row
        i2 = (i + 5) * 3;   // for 3rd row

        z = radius * sinf(V_ANGLE);             // elevaton
        xy = radius * cosf(V_ANGLE);

        vertices[i1] = xy * cosf(hAngle1);      // x
        vertices[i2] = xy * cosf(hAngle2);
        vertices[i1 + 1] = xy * sinf(hAngle1);  // x
        vertices[i2 + 1] = xy * sinf(hAngle2);
        vertices[i1 + 2] = z;                   // z
        vertices[i2 + 2] = -z;

        // next horizontal angles
        hAngle1 += H_ANGLE;
        hAngle2 += H_ANGLE;
    }

    // the last bottom vertex (0, 0, -r)
    i1 = 11 * 3;
    vertices[i1] = 0;
    vertices[i1 + 1] = 0;
    vertices[i1 + 2] = -radius;

    return vertices;
}


///////////////////////////////////////////////////////////////////////////////
// generate vertices with flat shading
// each triangle is independent (no shared vertices)
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::buildVerticesFlat()
{
    //const float S_STEP = 1 / 11.0f;         // horizontal texture step
    //const float T_STEP = 1 / 3.0f;          // vertical texture step
    const float S_STEP = 186 / 2048.0f;     // horizontal texture step
    const float T_STEP = 322 / 1024.0f;     // vertical texture step

    // compute 12 vertices of icosahedron
    std::vector<float> tmpVertices = computeIcosahedronVertices();

    // clear memory of prev arrays
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<unsigned int>().swap(indices);
    std::vector<unsigned int>().swap(lineIndices);

    const float* v0, * v1, * v2, * v3, * v4, * v11;          // vertex positions
    float n[3];                                         // face normal
    unsigned int index = 0;

    // compute and add 20 tiangles of icosahedron first
    v0 = &tmpVertices[0];       // 1st vertex
    v11 = &tmpVertices[11 * 3]; // 12th vertex
    for (int i = 1; i <= 5; ++i)
    {
        // 4 vertices in the 2nd row
        v1 = &tmpVertices[i * 3];
        if (i < 5)
            v2 = &tmpVertices[(i + 1) * 3];
        else
            v2 = &tmpVertices[3];

        v3 = &tmpVertices[(i + 5) * 3];
        if ((i + 5) < 10)
            v4 = &tmpVertices[(i + 6) * 3];
        else
            v4 = &tmpVertices[6 * 3];

        // add a triangle in 1st row
        SkinnyIcosphere::computeFaceNormal(v0, v1, v2, n);
        addVertices(v0, v1, v2);
        addNormals(n, n, n);
        addIndices(index, index + 1, index + 2);

        // add 2 triangles in 2nd row
        SkinnyIcosphere::computeFaceNormal(v1, v3, v2, n);
        addVertices(v1, v3, v2);
        addNormals(n, n, n);
        addIndices(index + 3, index + 4, index + 5);

        SkinnyIcosphere::computeFaceNormal(v2, v3, v4, n);
        addVertices(v2, v3, v4);
        addNormals(n, n, n);
        addIndices(index + 6, index + 7, index + 8);

        // add a triangle in 3rd row
        SkinnyIcosphere::computeFaceNormal(v3, v11, v4, n);
        addVertices(v3, v11, v4);
        addNormals(n, n, n);
        addIndices(index + 9, index + 10, index + 11);

        // add 6 edge lines per iteration
        //  i
        //  /   /   /   /   /       : (i, i+1)                              //
        // /__ /__ /__ /__ /__                                              //
        // \  /\  /\  /\  /\  /     : (i+3, i+4), (i+3, i+5), (i+4, i+5)    //
        //  \/__\/__\/__\/__\/__                                            //
        //   \   \   \   \   \      : (i+9,i+10), (i+9, i+11)               //
        //    \   \   \   \   \                                             //
        lineIndices.push_back(index);       // (i, i+1)
        lineIndices.push_back(index + 1);       // (i, i+1)
        lineIndices.push_back(index + 3);     // (i+3, i+4)
        lineIndices.push_back(index + 4);
        lineIndices.push_back(index + 3);     // (i+3, i+5)
        lineIndices.push_back(index + 5);
        lineIndices.push_back(index + 4);     // (i+4, i+5)
        lineIndices.push_back(index + 5);
        lineIndices.push_back(index + 9);     // (i+9, i+10)
        lineIndices.push_back(index + 10);
        lineIndices.push_back(index + 9);     // (i+9, i+11)
        lineIndices.push_back(index + 11);

        // next index
        index += 12;
    }

    // subdivide icosahedron
    subdivideVerticesFlat();

    // generate interleaved vertex array as well
    buildInterleavedVertices();
}


///////////////////////////////////////////////////////////////////////////////
// divide a trinage into 4 sub triangles and repeat N times
// If subdivision=0, do nothing.
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::subdivideVerticesFlat()
{
    std::vector<float> tmpVertices;
    std::vector<unsigned int> tmpIndices;
    int indexCount;
    const float* v1, * v2, * v3;          // ptr to original vertices of a triangle
    float newV1[3], newV2[3], newV3[3]; // new vertex positions
    float normal[3];                    // new face normal
    unsigned int index = 0;             // new index value
    int i, j;

    // iteration
    for (i = 1; i <= subdivision; ++i)
    {
        // copy prev arrays
        tmpVertices = vertices;
        tmpIndices = indices;

        // clear prev arrays
        vertices.clear();
        normals.clear();
        indices.clear();
        lineIndices.clear();

        index = 0;
        indexCount = (int)tmpIndices.size();
        for (j = 0; j < indexCount; j += 3)
        {
            // get 3 vertice and texcoords of a triangle
            v1 = &tmpVertices[tmpIndices[j] * 3];
            v2 = &tmpVertices[tmpIndices[j + 1] * 3];
            v3 = &tmpVertices[tmpIndices[j + 2] * 3];

            // get 3 new vertices by spliting half on each edge
            computeHalfVertex(v1, v2, radius, newV1);
            computeHalfVertex(v2, v3, radius, newV2);
            computeHalfVertex(v1, v3, radius, newV3);

            // add 4 new triangles
            addVertices(v1, newV1, newV3);
            computeFaceNormal(v1, newV1, newV3, normal);
            addNormals(normal, normal, normal);
            addIndices(index, index + 1, index + 2);

            addVertices(newV1, v2, newV2);
            computeFaceNormal(newV1, v2, newV2, normal);
            addNormals(normal, normal, normal);
            addIndices(index + 3, index + 4, index + 5);

            addVertices(newV1, newV2, newV3);
            computeFaceNormal(newV1, newV2, newV3, normal);
            addNormals(normal, normal, normal);
            addIndices(index + 6, index + 7, index + 8);

            addVertices(newV3, newV2, v3);
            computeFaceNormal(newV3, newV2, v3, normal);
            addNormals(normal, normal, normal);
            addIndices(index + 9, index + 10, index + 11);

            // next index
            index += 12;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// generate interleaved vertices: V/N/T
// stride must be 32 bytes
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::buildInterleavedVertices()
{
    std::vector<float>().swap(interleavedVertices);

    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);
    }
}


///////////////////////////////////////////////////////////////////////////////
// add single vertex to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::addVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}


///////////////////////////////////////////////////////////////////////////////
// add 3 vertices of a triangle to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::addVertices(const float v1[3], const float v2[3], const float v3[3])
{
    vertices.push_back(v1[0]);  // x
    vertices.push_back(v1[1]);  // y
    vertices.push_back(v1[2]);  // z
    vertices.push_back(v2[0]);
    vertices.push_back(v2[1]);
    vertices.push_back(v2[2]);
    vertices.push_back(v3[0]);
    vertices.push_back(v3[1]);
    vertices.push_back(v3[2]);
}


///////////////////////////////////////////////////////////////////////////////
// add single normal to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::addNormal(float nx, float ny, float nz)
{
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}


///////////////////////////////////////////////////////////////////////////////
// add 3 normals of a triangle to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::addNormals(const float n1[3], const float n2[3], const float n3[3])
{
    normals.push_back(n1[0]);  // nx
    normals.push_back(n1[1]);  // ny
    normals.push_back(n1[2]);  // nz
    normals.push_back(n2[0]);
    normals.push_back(n2[1]);
    normals.push_back(n2[2]);
    normals.push_back(n3[0]);
    normals.push_back(n3[1]);
    normals.push_back(n3[2]);
}


///////////////////////////////////////////////////////////////////////////////
// add 3 indices to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}


// static functions ===========================================================
///////////////////////////////////////////////////////////////////////////////
// return face normal (4th param) of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::computeFaceNormal(const float v1[3], const float v2[3], const float v3[3], float n[3])
{
    const float EPSILON = 0.000001f;

    // default return value (0, 0, 0)
    n[0] = n[1] = n[2] = 0;

    // find 2 edge vectors: v1-v2, v1-v3
    float ex1 = v2[0] - v1[0];
    float ey1 = v2[1] - v1[1];
    float ez1 = v2[2] - v1[2];
    float ex2 = v3[0] - v1[0];
    float ey2 = v3[1] - v1[1];
    float ez2 = v3[2] - v1[2];

    // cross product: e1 x e2
    float nx, ny, nz;
    nx = ey1 * ez2 - ez1 * ey2;
    ny = ez1 * ex2 - ex1 * ez2;
    nz = ex1 * ey2 - ey1 * ex2;

    // normalize only if the length is > 0
    float length = sqrtf(nx * nx + ny * ny + nz * nz);
    if (length > EPSILON)
    {
        // normalize
        float lengthInv = 1.0f / length;
        n[0] = nx * lengthInv;
        n[1] = ny * lengthInv;
        n[2] = nz * lengthInv;
    }
}


///////////////////////////////////////////////////////////////////////////////
// get the scale factor for vector to resize to the given length of vector
///////////////////////////////////////////////////////////////////////////////
float SkinnyIcosphere::computeScaleForLength(const float v[3], float length)
{
    // and normalize the vector then re-scale to new radius
    return length / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}


///////////////////////////////////////////////////////////////////////////////
// find middle point of 2 vertices
// NOTE: new vertex must be resized, so the length is equal to the given length
///////////////////////////////////////////////////////////////////////////////
void SkinnyIcosphere::computeHalfVertex(const float v1[3], const float v2[3], float length, float newV[3])
{
    newV[0] = v1[0] + v2[0];
    newV[1] = v1[1] + v2[1];
    newV[2] = v1[2] + v2[2];
    float scale = SkinnyIcosphere::computeScaleForLength(newV, length);
    newV[0] *= scale;
    newV[1] *= scale;
    newV[2] *= scale;
}
