//#include "MD5MyModelLoaderPCH.h"
#include "MD5MyModel.h"


MD5MyModel::MD5MyModel()
: m_iMD5Version(-1)
, m_iNumJoints(0)
, m_iNumMeshes(0)
, m_bHasAnimation(false)
, m_LocalToWorldMatrix(1)
{
}

MD5MyModel::~MD5MyModel()
{
}

bool MD5MyModel::LoadModel( const std::string &filename )
{
    //std::ifstream file(filename);

    if ( !fs::exists(filename) )
    {
        std::cerr << "MD5MyModel::LoadModel: Failed to find file: " << filename << std::endl;
        return false;
    }
    
    fs::path filePath = filename;
    // store the parent path used for loading images relative to this file.
    fs::path parent_path = filePath.parent_path();

    std::string param;
    std::string junk;   // Read junk from the file

    fs::ifstream file(filename);
    int fileLength = GetFileLength(file);
    assert( fileLength > 0 );

    m_Joints.clear();
    m_Meshes.clear();
    
    file >> param;

    while ( !file.eof() )
    {
        if ( param == "MD5Version" )
        {
            file >> m_iMD5Version;
            assert( m_iMD5Version == 10 );
        }
        else if ( param == "commandline" )
        {
            //IgnoreLine(file, fileLength ); // Ignore the contents of the line
        }
        else if ( param == "numJoints" )
        {
            file >> m_iNumJoints;
            m_Joints.reserve(m_iNumJoints);
        }
        else if ( param == "numMeshes" )
        {
            file >> m_iNumMeshes;
            m_Meshes.reserve(m_iNumMeshes);
        }
        else if ( param == "joints" )
        {
            Joint joint;
            file >> junk; // Read the '{' character
            for ( int i = 0; i < m_iNumJoints; ++i )
            {
                file >> joint.m_Name        >> joint.m_ParentID >> junk
                     >> joint.m_Pos.x       >> joint.m_Pos.y    >> joint.m_Pos.z    >> junk >> junk
                     >> joint.m_Orient.x    >> joint.m_Orient.y >> joint.m_Orient.z >> junk;
                
                RemoveQuotes( joint.m_Name );
                ComputeQuatW( joint.m_Orient );

                m_Joints.push_back(joint);
                // Ignore everything else on the line up to the end-of-line character.
                IgnoreLine( file, fileLength );
            }
            file >> junk; // Read the '}' character
        }
        else if ( param == "mesh" )
        {
            Mesh mesh;
            int numVerts, numTris, numWeights;

            file >> junk; // Read the '{' character
            file >> param;
            while ( param != "}" )  // Read until we get to the '}' character
            {
                if ( param == "shader" )
                {
                    file >> mesh.m_Shader;
                    RemoveQuotes( mesh.m_Shader );

                    fs::path shaderPath( mesh.m_Shader );
                    fs::path texturePath;
                    if ( shaderPath.has_parent_path() )
                    {
                        texturePath = shaderPath;
                    }
                    else
                    {
                        texturePath = parent_path / shaderPath;
                    }

                    if ( !texturePath.has_extension() )
                    {
                        texturePath.replace_extension( ".tga" );
                    }

                    mesh.m_TexID = SOIL_load_OGL_texture( texturePath.string().c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
                    // for now i comment it

                    file.ignore(fileLength, '\n' ); // Ignore everything else on the line
                }
                else if ( param == "numverts")
                {
                    file >> numVerts;               // Read in the vertices
                    IgnoreLine(file, fileLength);
                    for ( int i = 0; i < numVerts; ++i )
                    {
                        Vertex vert;

                        file >> junk >> junk >> junk                    // vert vertIndex (
                            >> vert.m_Tex0.x >> vert.m_Tex0.y >> junk  //  s t )
                            >> vert.m_StartWeight >> vert.m_WeightCount;

                        IgnoreLine(file, fileLength);

                        mesh.m_Verts.push_back(vert);
                        mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
                    }  
                }
                else if ( param == "numtris" )
                {
                    file >> numTris;
                    IgnoreLine(file, fileLength);
                    for ( int i = 0; i < numTris; ++i )
                    {
                        Triangle tri;
                        file >> junk >> junk >> tri.m_Indices[0] >> tri.m_Indices[1] >> tri.m_Indices[2];

                        IgnoreLine( file, fileLength );

                        mesh.m_Tris.push_back(tri);
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[0] );
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[1] );
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[2] );
                    }              
                }
                else if ( param == "numweights" )
                {
                    file >> numWeights;
                    IgnoreLine( file, fileLength );
                    for ( int i = 0; i < numWeights; ++i )
                    {
                        Weight weight;
                        file >> junk >> junk >> weight.m_JointID >> weight.m_Bias >> junk
                            >> weight.m_Pos.x >> weight.m_Pos.y >> weight.m_Pos.z >> junk;

                        IgnoreLine( file, fileLength );
                        mesh.m_Weights.push_back(weight);
                    }
                }
                else
                {
                    IgnoreLine(file, fileLength);
                }
            
                file >> param;
            }

            PrepareMesh(mesh);
            PrepareNormals(mesh);

            m_Meshes.push_back(mesh);

        }

        file >> param;
    }

    //assert( m_Joints.size() == m_iNumJoints );
    assert( m_Meshes.size() == m_iNumMeshes );

    return true;
}


bool MD5MyModel::LoadAnim(const std::string& filename)
{
    if (m_Animation.LoadAnimation(filename))
    {
        // Check to make sure the animation is appropriate for this model
        m_bHasAnimation = CheckAnimation(m_Animation);
    }

    return m_bHasAnimation;
}


bool MD5MyModel::CheckAnimation(const MD5MyAnimation& animation) const
{
    if (m_iNumJoints != animation.GetNumJoints())
    {
        return false;
    }

    // Check to make sure the joints match up
    for (unsigned int i = 0; i < m_Joints.size(); ++i)
    {
        const Joint& meshJoint = m_Joints[i];
        const MD5MyAnimation::JointInfo& animJoint = animation.GetJointInfo(i);

        if (meshJoint.m_Name != animJoint.m_Name ||
            meshJoint.m_ParentID != animJoint.m_ParentID)
        {
            return false;
        }
    }

    return true;
}


// Compute the position of the vertices in object local space
// in the skeleton's bind pose
bool MD5MyModel::PrepareMesh( Mesh& mesh )
{
    mesh.m_PositionBuffer.clear();
    mesh.m_Tex2DBuffer.clear();

    // Compute vertex positions
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        glm::vec3 finalPos(0);
        Vertex& vert = mesh.m_Verts[i];

        vert.m_Pos = glm::vec3(0);
        vert.m_Normal = glm::vec3(0);

        // Sum the position of the weights
        for ( int j = 0; j < vert.m_WeightCount; ++j )
        {
            Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
            Joint& joint = m_Joints[weight.m_JointID];
            
            // Convert the weight position from Joint local space to object space
            glm::vec3 rotPos = joint.m_Orient * weight.m_Pos;

            vert.m_Pos += ( joint.m_Pos + rotPos ) * weight.m_Bias;
        }

        mesh.m_PositionBuffer.push_back(vert.m_Pos);
        mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
    }

    return true;
}


bool MD5MyModel::PrepareMesh(Mesh& mesh, const MD5MyAnimation::FrameSkeleton& skel)
{
    for (unsigned int i = 0; i < mesh.m_Verts.size(); ++i)
    {
        const Vertex& vert = mesh.m_Verts[i];
        glm::vec3& pos = mesh.m_PositionBuffer[i];
        glm::vec3& normal = mesh.m_NormalBuffer[i];

        pos = glm::vec3(0);
        normal = glm::vec3(0);

        for (int j = 0; j < vert.m_WeightCount; ++j)
        {
            const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
            const MD5MyAnimation::SkeletonJoint& joint = skel.m_Joints[weight.m_JointID];

            glm::vec3 rotPos = joint.m_Orient * weight.m_Pos;
            pos += (joint.m_Pos + rotPos) * weight.m_Bias;

            normal += (joint.m_Orient * vert.m_Normal) * weight.m_Bias;
        }
    }
    return true;
}


// Compute the vertex normals in the Mesh's bind pose
bool MD5MyModel::PrepareNormals( Mesh& mesh )
{
    mesh.m_NormalBuffer.clear();

    // Loop through all triangles and calculate the normal of each triangle
    for ( unsigned int i = 0; i < mesh.m_Tris.size(); ++i )
    {
        glm::vec3 v0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Pos;
        glm::vec3 v1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Pos;
        glm::vec3 v2 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Pos;

        glm::vec3 normal = glm::cross( v2 - v0, v1 - v0 );

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Normal += normal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Normal += normal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Normal += normal;
    }

    // Now normalize all the normals
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        Vertex& vert = mesh.m_Verts[i];

        glm::vec3 normal = glm::normalize( vert.m_Normal );
        mesh.m_NormalBuffer.push_back( normal );

        // Reset the normal to calculate the bind-pose normal in joint space
        vert.m_Normal = glm::vec3(0);

        // Put the bind-pose normal into joint-local space
        // so the animated normal can be computed faster later
        for ( int j = 0; j < vert.m_WeightCount; ++j )
        {
            const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
            const Joint& joint = m_Joints[weight.m_JointID];
            vert.m_Normal += ( normal * joint.m_Orient ) * weight.m_Bias;
        }
    }

    return true;
}

void MD5MyModel::Update( float fDeltaTime )
{
    if ( m_bHasAnimation )
    {
        m_Animation.Update(fDeltaTime);
        const MD5MyAnimation::FrameSkeleton& skeleton = m_Animation.GetSkeleton();

        for (unsigned int i = 0; i < m_Meshes.size(); ++i)
        {
            PrepareMesh(m_Meshes[i], skeleton);
        }
    }
}

void MD5MyModel::ComputeAllIndices()
{
    for (unsigned int i = 0; i < m_Meshes.size(); ++i)
    {
        for (unsigned int j = 0; j < m_Meshes[i].m_IndexBuffer.size(); j++)
        {
            //m_AllData.push_back(m_Meshes[i].m_IndexBuffer[j]);
        }
    }
}

void MD5MyModel::ComputeAllData() 
{
    m_AllData[0].clear();
    m_AllData[1].clear();
    m_AllData[2].clear();
    m_AllData[3].clear();
    m_AllData[4].clear();
    m_AllData[5].clear();

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        for (unsigned int j = 0; j < m_Meshes[i].m_PositionBuffer.size(); j++)
        {
            m_AllData[i].push_back(m_Meshes[i].m_PositionBuffer[j].x);
            m_AllData[i].push_back(m_Meshes[i].m_PositionBuffer[j].y);
            m_AllData[i].push_back(m_Meshes[i].m_PositionBuffer[j].z);

            m_AllData[i].push_back(m_Meshes[i].m_NormalBuffer[j].x);
            m_AllData[i].push_back(m_Meshes[i].m_NormalBuffer[j].y);
            m_AllData[i].push_back(m_Meshes[i].m_NormalBuffer[j].z);

            m_AllData[i].push_back(m_Meshes[i].m_Tex2DBuffer[j].x);
            m_AllData[i].push_back(m_Meshes[i].m_Tex2DBuffer[j].y);
        }
    }
}
