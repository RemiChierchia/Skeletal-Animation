//#include <GL/glew.h>

#include <iostream>
#include <iomanip>
#include <cmath>
#include "SkinnyCylinder.h"


// constants //////////////////////////////////////////////////////////////////
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 1;


///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
SkinnyCylinder::SkinnyCylinder(float baseRadius, float topRadius, float height, int sectors,
    int stacks) : interleavedStride(32)
{
    set(baseRadius, topRadius, height, sectors, stacks);
}


///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::set(float baseRadius, float topRadius, float height, int sectors,
    int stacks)
{
    this->baseRadius = baseRadius;
    this->topRadius = topRadius;
    this->height = height;
    this->sectorCount = sectors;
    if (sectors < MIN_SECTOR_COUNT)
        this->sectorCount = MIN_SECTOR_COUNT;
    this->stackCount = stacks;
    if (stacks < MIN_STACK_COUNT)
        this->stackCount = MIN_STACK_COUNT;

    // generate unit circle vertices first
    buildUnitCircleVertices();

    buildVerticesFlat();
}

void SkinnyCylinder::setBaseRadius(float radius)
{
    if (this->baseRadius != radius)
        set(radius, topRadius, height, sectorCount, stackCount);
}

void SkinnyCylinder::setTopRadius(float radius)
{
    if (this->topRadius != radius)
        set(baseRadius, radius, height, sectorCount, stackCount);
}

void SkinnyCylinder::setHeight(float height)
{
    if (this->height != height)
        set(baseRadius, topRadius, height, sectorCount, stackCount);
}

void SkinnyCylinder::setSectorCount(int sectors)
{
    if (this->sectorCount != sectors)
        set(baseRadius, topRadius, height, sectors, stackCount);
}

void SkinnyCylinder::setStackCount(int stacks)
{
    if (this->stackCount != stacks)
        set(baseRadius, topRadius, height, sectorCount, stacks);
}


///////////////////////////////////////////////////////////////////////////////
// dealloc vectors
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::clearArrays()
{
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<unsigned int>().swap(indices);
    std::vector<unsigned int>().swap(lineIndices);
}


///////////////////////////////////////////////////////////////////////////////
// generate vertices with flat shading
// each triangle is independent (no shared vertices)
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::buildVerticesFlat()
{
    // tmp vertex definition (x,y,z,s,t)
    struct Vertex
    {
        float x, y, z, s, t;
    };
    std::vector<Vertex> tmpVertices;

    int i, j, k;    // indices
    float x, y, z, s, t, radius;

    // put tmp vertices of SkinnyCylinder side to array by scaling unit circle
    //NOTE: start and end vertex positions are same, but texcoords are different
    //      so, add additional vertex at the end point
    for (i = 0; i <= stackCount; ++i)
    {
        z = -(height * 0.5f) + (float)i / stackCount * height;      // vertex position z
        radius = baseRadius + (float)i / stackCount * (topRadius - baseRadius);     // lerp
        t = 1.0f - (float)i / stackCount;   // top-to-bottom

        for (j = 0, k = 0; j <= sectorCount; ++j, k += 3)
        {
            x = unitCircleVertices[k];
            y = unitCircleVertices[k + 1];
            s = (float)j / sectorCount;

            Vertex vertex;
            vertex.x = x * radius;
            vertex.y = y * radius;
            vertex.z = z;
            vertex.s = s;
            vertex.t = t;
            tmpVertices.push_back(vertex);
        }
    }

    // clear memory of prev arrays
    clearArrays();

    Vertex v1, v2, v3, v4;      // 4 vertex positions v1, v2, v3, v4
    std::vector<float> n;       // 1 face normal
    int vi1, vi2;               // indices
    int index = 0;

    // v2-v4 <== stack at i+1
    // | \ |
    // v1-v3 <== stack at i
    for (i = 0; i < stackCount; ++i)
    {
        vi1 = i * (sectorCount + 1);            // index of tmpVertices
        vi2 = (i + 1) * (sectorCount + 1);

        for (j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
        {
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // compute a face normal of v1-v3-v2
            n = computeFaceNormal(v1.x, v1.y, v1.z, v3.x, v3.y, v3.z, v2.x, v2.y, v2.z);

            // put quad vertices: v1-v2-v3-v4
            addVertex(v1.x, v1.y, v1.z);
            addVertex(v2.x, v2.y, v2.z);
            addVertex(v3.x, v3.y, v3.z);
            addVertex(v4.x, v4.y, v4.z);

            // put normal
            for (k = 0; k < 4; ++k)  // same normals for all 4 vertices
            {
                addNormal(n[0], n[1], n[2]);
            }

            // put indices of a quad
            addIndices(index, index + 2, index + 1);    // v1-v3-v2
            addIndices(index + 1, index + 2, index + 3);    // v2-v3-v4

            // vertical line per quad: v1-v2
            lineIndices.push_back(index);
            lineIndices.push_back(index + 1);
            // horizontal line per quad: v2-v4
            lineIndices.push_back(index + 1);
            lineIndices.push_back(index + 3);
            if (i == 0)
            {
                lineIndices.push_back(index);
                lineIndices.push_back(index + 2);
            }

            index += 4;     // for next
        }
    }

    // remember where the base index starts
    baseIndex = (unsigned int)indices.size();
    unsigned int baseVertexIndex = (unsigned int)vertices.size() / 3;

    // put vertices of base of SkinnyCylinder
    z = -height * 0.5f;
    addVertex(0, 0, z);
    addNormal(0, 0, -1);
    for (i = 0, j = 0; i < sectorCount; ++i, j += 3)
    {
        x = unitCircleVertices[j];
        y = unitCircleVertices[j + 1];
        addVertex(x * baseRadius, y * baseRadius, z);
        addNormal(0, 0, -1);
    }

    // put indices for base
    for (i = 0, k = baseVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
            addIndices(baseVertexIndex, k + 1, k);
        else
            addIndices(baseVertexIndex, baseVertexIndex + 1, k);
    }

    // remember where the top index starts
    topIndex = (unsigned int)indices.size();
    unsigned int topVertexIndex = (unsigned int)vertices.size() / 3;

    // put vertices of top of SkinnyCylinder
    z = height * 0.5f;
    addVertex(0, 0, z);
    addNormal(0, 0, 1);
    for (i = 0, j = 0; i < sectorCount; ++i, j += 3)
    {
        x = unitCircleVertices[j];
        y = unitCircleVertices[j + 1];
        addVertex(x * topRadius, y * topRadius, z);
        addNormal(0, 0, 1);
    }

    for (i = 0, k = topVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
            addIndices(topVertexIndex, k, k + 1);
        else
            addIndices(topVertexIndex, k, topVertexIndex + 1);
    }

    // generate interleaved vertex array as well
    buildInterleavedVertices();
}


///////////////////////////////////////////////////////////////////////////////
// generate interleaved vertices: V/N/T
// stride must be 32 bytes
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::buildInterleavedVertices()
{
    std::vector<float>().swap(interleavedVertices);

    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        //interleavedVertices.push_back(vertices[i]);
        //interleavedVertices.push_back(vertices[i+1]);
        //interleavedVertices.push_back(vertices[i+2]);
        interleavedVertices.insert(interleavedVertices.end(), &vertices[i], &vertices[i] + 3);

        //interleavedVertices.push_back(normals[i]);
        //interleavedVertices.push_back(normals[i+1]);
        //interleavedVertices.push_back(normals[i+2]);
        interleavedVertices.insert(interleavedVertices.end(), &normals[i], &normals[i] + 3);
    }
}


///////////////////////////////////////////////////////////////////////////////
// generate 3D vertices of a unit circle on XY plance
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::buildUnitCircleVertices()
{
    const float PI = acos(-1);
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle;  // radian

    std::vector<float>().swap(unitCircleVertices);
    for (int i = 0; i <= sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(cos(sectorAngle)); // x
        unitCircleVertices.push_back(sin(sectorAngle)); // y
        unitCircleVertices.push_back(0);                // z
    }
}


///////////////////////////////////////////////////////////////////////////////
// add single vertex to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::addVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}


///////////////////////////////////////////////////////////////////////////////
// add single normal to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::addNormal(float nx, float ny, float nz)
{
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}


///////////////////////////////////////////////////////////////////////////////
// add 3 indices to array
///////////////////////////////////////////////////////////////////////////////
void SkinnyCylinder::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}


///////////////////////////////////////////////////////////////////////////////
// generate shared normal vectors of the side of SkinnyCylinder
///////////////////////////////////////////////////////////////////////////////
std::vector<float> SkinnyCylinder::getSideNormals()
{
    const float PI = acos(-1);
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle;  // radian

    // compute the normal vector at 0 degree first
    // tanA = (baseRadius-topRadius) / height
    float zAngle = atan2(baseRadius - topRadius, height);
    float x0 = cos(zAngle);     // nx
    float y0 = 0;               // ny
    float z0 = sin(zAngle);     // nz

    // rotate (x0,y0,z0) per sector angle
    std::vector<float> normals;
    for (int i = 0; i <= sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        normals.push_back(cos(sectorAngle) * x0 - sin(sectorAngle) * y0);   // nx
        normals.push_back(sin(sectorAngle) * x0 + cos(sectorAngle) * y0);   // ny
        normals.push_back(z0);  // nz
        /*
        //debug
        float nx = cos(sectorAngle)*x0 - sin(sectorAngle)*y0;
        float ny = sin(sectorAngle)*x0 + cos(sectorAngle)*y0;
        std::cout << "normal=(" << nx << ", " << ny << ", " << z0
                  << "), length=" << sqrtf(nx*nx + ny*ny + z0*z0) << std::endl;
        */
    }

    return normals;
}


///////////////////////////////////////////////////////////////////////////////
// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
///////////////////////////////////////////////////////////////////////////////
std::vector<float> SkinnyCylinder::computeFaceNormal(float x1, float y1, float z1,  // v1
    float x2, float y2, float z2,  // v2
    float x3, float y3, float z3)  // v3
{
    const float EPSILON = 0.000001f;

    std::vector<float> normal(3, 0.0f);     // default return value (0,0,0)
    float nx, ny, nz;

    // find 2 edge vectors: v1-v2, v1-v3
    float ex1 = x2 - x1;
    float ey1 = y2 - y1;
    float ez1 = z2 - z1;
    float ex2 = x3 - x1;
    float ey2 = y3 - y1;
    float ez2 = z3 - z1;

    // cross product: e1 x e2
    nx = ey1 * ez2 - ez1 * ey2;
    ny = ez1 * ex2 - ex1 * ez2;
    nz = ex1 * ey2 - ey1 * ex2;

    // normalize only if the length is > 0
    float length = sqrtf(nx * nx + ny * ny + nz * nz);
    if (length > EPSILON)
    {
        // normalize
        float lengthInv = 1.0f / length;
        normal[0] = nx * lengthInv;
        normal[1] = ny * lengthInv;
        normal[2] = nz * lengthInv;
    }

    return normal;
}
