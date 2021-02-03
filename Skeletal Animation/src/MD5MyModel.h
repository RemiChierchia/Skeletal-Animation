#pragma once

#include "MD5MyAnimation.h"

#include <fstream>
#include <sstream>


class MD5MyModel
{
public:
    MD5MyModel();
    virtual ~MD5MyModel();

    bool LoadModel(const std::string& filename);
    bool LoadAnim(const std::string& filename);
    void Update(float fDeltaTime);

    unsigned int getJointsNum() const { return m_iNumJoints; };

    const glm::vec3* getJoints() const { return &m_Joints.data()->m_Pos; };
    const glm::vec3 getJointsF(int i) const { return m_Joints[i].m_Pos; };

    const glm::vec3 getJointsAnimation(int i, int j) const { return *m_Animation.getJoints(i,j); };

    const float getFrames(int i, int j) const { return *m_Animation.getFrames(i,j); };

    const unsigned int  getMeshesSize() const { return m_Meshes.size(); };

    const unsigned int  getPositionNumber(int i) const   { return m_Meshes[i].m_PositionBuffer.size(); };
    const glm::vec3* getPositionBuffer(int i) const { return &m_Meshes[i].m_PositionBuffer[0]; };
    const glm::vec3*    getNormalBuffer() const     { return m_Meshes[0].m_NormalBuffer.data(); };
    const glm::vec2*    getTex2DBuffer() const      { return m_Meshes[0].m_Tex2DBuffer.data(); };
    const unsigned int  getIndexNumber(int i) const { return m_Meshes[i].m_IndexBuffer.size(); };
    const GLuint* getIndexBuffer(int i) const { return &m_Meshes[i].m_IndexBuffer[0]; };

    void ComputeAllIndices();
    const GLuint* getAllIndices() const { return &m_AllIndices[0]; };

    void ComputeAllData();
    const float* getAllData(int i) const { return &m_AllData[i][0]; };

    const unsigned int getAllDataDimension(int i) const { return m_AllData[i].size(); };

    const GLuint getTexture(int i) const { return m_Meshes[i].m_TexID; };

protected:
    typedef std::vector<glm::vec3> PositionBuffer;
    typedef std::vector<glm::vec3> NormalBuffer;
    typedef std::vector<glm::vec2> Tex2DBuffer;
    typedef std::vector<GLuint> IndexBuffer;

    struct Vertex
    {
        glm::vec3   m_Pos;
        glm::vec3   m_Normal;
        glm::vec2   m_Tex0;
        int         m_StartWeight;
        int         m_WeightCount;
    };
    typedef std::vector<Vertex> VertexList;

    struct Triangle
    {
        int             m_Indices[3];
    };
    typedef std::vector<Triangle> TriangleList;

    struct Weight
    {
        int             m_JointID;
        float           m_Bias;
        glm::vec3       m_Pos;
    };
    typedef std::vector<Weight> WeightList;

    struct Joint
    {
        std::string     m_Name;
        int             m_ParentID;
        glm::vec3       m_Pos;
        glm::quat       m_Orient;
    };
    typedef std::vector<Joint> JointList;

    struct Mesh
    {
        std::string     m_Shader;
        // This vertex list stores the vertices in the bind pose.
        VertexList      m_Verts;
        TriangleList    m_Tris;
        WeightList      m_Weights;

        // A texture ID for the material
        GLuint          m_TexID;

        // These buffers are used for rendering the animated mesh
        PositionBuffer  m_PositionBuffer;   // Vertex position stream
        NormalBuffer    m_NormalBuffer;     // Vertex normals stream
        Tex2DBuffer     m_Tex2DBuffer;      // Texture coordinate set
        IndexBuffer     m_IndexBuffer;      // Vertex index buffer
    };
    typedef std::vector<Mesh> MeshList;

    // Prepare the mesh for rendering
    // Compute vertex positions and normals
    bool PrepareMesh(Mesh& mesh);
    bool PrepareMesh(Mesh& mesh, const MD5MyAnimation::FrameSkeleton& skel);
    bool PrepareNormals(Mesh& mesh);

    bool CheckAnimation(const MD5MyAnimation& animation) const;

private:

    int                 m_iMD5Version;
    int                 m_iNumJoints;
    int                 m_iNumMeshes;

    bool                m_bHasAnimation;

    JointList           m_Joints;
    MeshList            m_Meshes;

    MD5MyAnimation      m_Animation;

    glm::mat4x4         m_LocalToWorldMatrix;

    std::vector<GLuint> m_AllIndices;

    std::vector<float>  m_AllData[6];

};
