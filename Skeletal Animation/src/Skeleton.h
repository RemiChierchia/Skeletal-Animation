#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
// for convenience
using json = nlohmann::json;

class Skeleton
{
private:

    std::string path;

    glm::vec3 SpherePositions[17];

    glm::vec3 CylinderPositions[16];
    glm::vec3 CylinderRotations[16];
    glm::vec3 PelvicRotation;
    std::vector<float> angles;
    std::vector<float> lenghts;

public:

    Skeleton(const std::string& _path)
        : path(_path)
    {
        setPositions();
    }


    const glm::vec3* getSpherePosition() const      { return SpherePositions; }
    const glm::vec3* getCylinderPosition() const    { return CylinderPositions; }
    glm::vec3* getCylinderPositionNC()              { return CylinderPositions; }
    const glm::vec3* getCylinderRotation() const    { return CylinderRotations; }
    glm::vec3* getCylinderRotationNC()              { return CylinderRotations; }
    const glm::vec3 getPelvicRotation() const       { return PelvicRotation; }
    const float getAngles(int i) const              { return angles[i]; }
    const float getLenghts(int i) const             { return lenghts[i]; }

    void setPositions()
    {
        std::ifstream p(path, std::ifstream::binary);
        json Person;
        p >> Person;
        for (unsigned int i = 0; i < 17; i++) {
            SpherePositions[i] = glm::vec3((float)Person[i][1], (float)Person[i][2], (float)Person[i][0]); //like this i have the skeleton in front of me
        }

        SetCylinders();
    }

    void Update(const std::string& _path)
    {
        this->path = _path;

        Clear();

        std::ifstream p(path, std::ifstream::binary);
        json Person;
        p >> Person;
        for (unsigned int i = 0; i < 17; i++) {
            SpherePositions[i] = glm::vec3((float)Person[i][1], (float)Person[i][2], (float)Person[i][0]); //like this i have the skeleton in front of me
        }

        SetCylinders();
    }

    void Clear()
    {
        std::vector<float>().swap(angles);
        std::vector<float>().swap(lenghts);
    }

    void SetCylinders()
    {
        /// CylinderPositions
        CylinderPositions[0]  = midPoint(SpherePositions[0],  SpherePositions[1]);  /// Hip ------- RHip
        CylinderPositions[1]  = midPoint(SpherePositions[0],  SpherePositions[4]);  /// Hip ------- LHip
        CylinderPositions[2]  = midPoint(SpherePositions[0],  SpherePositions[7]);  /// Hip ------- Spine
        CylinderPositions[3]  = midPoint(SpherePositions[1],  SpherePositions[2]);  /// RHip ------ RKnee
        CylinderPositions[4]  = midPoint(SpherePositions[2],  SpherePositions[3]);  /// RKnee ----- RFoot
        CylinderPositions[5]  = midPoint(SpherePositions[4],  SpherePositions[5]);  /// LHip ------ LKnee
        CylinderPositions[6]  = midPoint(SpherePositions[5],  SpherePositions[6]);  /// LKnee ----- LFoot
        CylinderPositions[7]  = midPoint(SpherePositions[7],  SpherePositions[8]);  /// Spine ----- Thorax
        CylinderPositions[8]  = midPoint(SpherePositions[8],  SpherePositions[9]);  /// Thorax ---- Neck
        CylinderPositions[9]  = midPoint(SpherePositions[9],  SpherePositions[10]); /// Neck ------ Head
        CylinderPositions[10] = midPoint(SpherePositions[8],  SpherePositions[14]); /// Thorax ---- RShoulder
        CylinderPositions[11] = midPoint(SpherePositions[14], SpherePositions[15]); /// RShoulder - RElbow
        CylinderPositions[12] = midPoint(SpherePositions[15], SpherePositions[16]); /// RElbow ---- RWrist
        CylinderPositions[13] = midPoint(SpherePositions[8],  SpherePositions[11]); /// Thorax ---- LShoulder
        CylinderPositions[14] = midPoint(SpherePositions[11], SpherePositions[12]); /// LShoulder - LElbow
        CylinderPositions[15] = midPoint(SpherePositions[12], SpherePositions[13]); /// LElbow ---- LWrist


        /// CylinderRotations
        CylinderRotations[0]  = Rotation(SpherePositions[0],  SpherePositions[1],  CylinderPositions[0]);  /// Hip ------- RHip
        CylinderRotations[1]  = Rotation(SpherePositions[0],  SpherePositions[4],  CylinderPositions[1]);  /// Hip ------- LHip
        CylinderRotations[2]  = Rotation(SpherePositions[0],  SpherePositions[7],  CylinderPositions[2]);  /// Hip ------- Spine
        CylinderRotations[3]  = Rotation(SpherePositions[1],  SpherePositions[2],  CylinderPositions[3]);  /// RHip ------ RKnee
        CylinderRotations[4]  = Rotation(SpherePositions[2],  SpherePositions[3],  CylinderPositions[4]);  /// RKnee ----- RFoot
        CylinderRotations[5]  = Rotation(SpherePositions[4],  SpherePositions[5],  CylinderPositions[5]);  /// LHip ------ LKnee
        CylinderRotations[6]  = Rotation(SpherePositions[5],  SpherePositions[6],  CylinderPositions[6]);  /// LKnee ----- LFoot
        CylinderRotations[7]  = Rotation(SpherePositions[7],  SpherePositions[8],  CylinderPositions[7]);  /// Spine ----- Thorax
        CylinderRotations[8]  = Rotation(SpherePositions[8],  SpherePositions[9],  CylinderPositions[8]);  /// Thorax ---- Neck
        CylinderRotations[9]  = Rotation(SpherePositions[9],  SpherePositions[10], CylinderPositions[9]);  /// Neck ------ Head
        CylinderRotations[10] = Rotation(SpherePositions[8],  SpherePositions[14], CylinderPositions[10]); /// Thorax ---- RShoulder
        CylinderRotations[11] = Rotation(SpherePositions[14], SpherePositions[15], CylinderPositions[11]); /// RShoulder - RElbow
        CylinderRotations[12] = Rotation(SpherePositions[15], SpherePositions[16], CylinderPositions[12]); /// RElbow ---- RWrist
        CylinderRotations[13] = Rotation(SpherePositions[8],  SpherePositions[11], CylinderPositions[13]); /// Thorax ---- LShoulder
        CylinderRotations[14] = Rotation(SpherePositions[11], SpherePositions[12], CylinderPositions[14]); /// LShoulder - LElbow
        CylinderRotations[15] = Rotation(SpherePositions[12], SpherePositions[13], CylinderPositions[15]); /// LElbow ---- LWrist
    
        
        /// PelvicRotation
        PelvicRotation = Rotation(SpherePositions[1], SpherePositions[4], SpherePositions[0]); /// RHip ----- LHip
    }

    ////////////////////////////////////////////////////////////
    /// MidPoints
    ////////////////////////////////////////////////////////////
    glm::vec3 midPoint(const glm::vec3 p1, const glm::vec3 p2)
    {
        float mpx, mpy, mpz;


        /// X ///
        if (p1.x >= 0 && p2.x >= 0) {
            if (p1.x > p2.x)
                mpx = (p1.x - p2.x) / 2 + p2.x;
            else
                if (p1.x == p2.x)
                    mpx = p1.x;
                else
                    mpx = (p2.x - p1.x) / 2 + p1.x;
        }
        else if (p1.x < 0 && p2.x < 0) {
            if (p1.x < p2.x)
                mpx = (p1.x - p2.x) / 2 + p2.x;
            else
                if (p1.x == p2.x)
                    mpx = p1.x;
                else
                    mpx = (p2.x - p1.x) / 2 + p1.x;
        }
        else if (p1.x < 0 && p2.x >= 0)
            mpx = (p2.x + p1.x) / 2;
        else if (p2.x < 0 && p1.x >= 0)
            mpx = (p1.x + p2.x) / 2;


        /// Y ///
        if (p1.y >= 0 && p2.y >= 0) {
            if (p1.y > p2.y)
                mpy = (p1.y - p2.y) / 2 + p2.y;                
            else
                if (p1.y == p2.y)
                    mpy = p1.y;
                else
                    mpy = (p2.y - p1.y) / 2 + p1.y;
        }
        else if (p1.y < 0 && p2.y < 0) {
            if (p1.y < p2.y)
                mpy = (p1.y - p2.y) / 2 + p2.y;
            else
                if (p1.y == p2.y)
                    mpy = p1.y;
                else
                    mpy = (p2.y - p1.y) / 2 + p1.y;
        }
        else if (p1.y < 0 && p2.y >= 0)
            mpy = (p2.y + p1.y) / 2;
        else if (p2.y < 0 && p1.y >= 0)
            mpy = (p1.y + p2.y) / 2;


        /// Z ///
        if (p1.z >= 0 && p2.z >= 0) {
            if (p1.z > p2.z)
                mpz = (p1.z - p2.z) / 2 + p2.z;
            else
                if (p1.z == p2.z)
                    mpz = p1.z;
                else
                    mpz = (p2.z - p1.z) / 2 + p1.z;
        }
        else if (p1.z < 0 && p2.z < 0) {
            if (p1.z < p2.z)
                mpz = (p1.z - p2.z) / 2 + p2.z;
            else
                if (p1.z == p2.z)
                    mpz = p1.z;
                else
                    mpz = (p2.z - p1.z) / 2 + p1.z;
        }
        else if (p1.z < 0 && p2.z >= 0)
            mpz = (p2.z + p1.z) / 2;
        else if (p2.z < 0 && p1.z >= 0)
            mpz = (p1.z + p2.z) / 2;


        return glm::vec3(mpx, mpy, mpz);
    }

    ////////////////////////////////////////////////////////////
    /// Rotations
    ////////////////////////////////////////////////////////////
    /// 
    /// DO NOT TOUCH IT IT WORKS BUT NO IDEA ON HOW NOR WHY
    /// 
    glm::vec3 Rotation(const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 pos)
    {
        float dx, dy, dz;
        float vx, vy, vz;


        /// X ///
        if (p1.x >= 0 && p2.x >= 0) {
            if (p1.x > p2.x)
                dx = p1.x - p2.x;
            else
                if (p1.x == p2.x) 
                    dx = 0;
                else 
                    dx = p2.x - p1.x;
        }
        else if (p1.x < 0 && p2.x < 0) {
            if (p1.x < p2.x)
                dx = abs(p1.x - p2.x);
            else
                if (p1.x == p2.x)
                    dx = 0;
                    //dx = p1.x;
                else
                    dx = abs(p2.x - p1.x);
        }
        else if (p1.x < 0 && p2.x >= 0)
            dx = p2.x - p1.x;           //ocioo, I need distance
        else if (p2.x < 0 && p1.x >= 0)
            dx = p1.x - p2.x;           //ocioo, I need distance


        /// Y ///
        if (p1.y >= 0 && p2.y >= 0) {
            if (p1.y > p2.y)
                dy = p1.y - p2.y;
            else
                if (p1.y == p2.y)
                    dy = 0;
                    //dy = p1.y;
                else
                    dy = p2.y - p1.y;
        }
        else if (p1.y < 0 && p2.y < 0) {
            if (p1.y < p2.y)
                dy = abs(p1.y - p2.y);
            else
                if (p1.y == p2.y)
                    dy = 0;
                    //dy = p1.y;
                else
                    dy = abs(p2.y - p1.y);
        }
        else if (p1.y < 0 && p2.y >= 0)
            dy = p2.y - p1.y;           //ocioo, I need distance
        else if (p2.y < 0 && p1.y >= 0)
            dy = p1.y - p2.y;           //ocioo, I need distance


        /// Z ///
        if (p1.z >= 0 && p2.z >= 0) {
            if (p1.z > p2.z)
                dz = p1.z - p2.z;
            else
                if (p1.z == p2.z)
                    //dz = p1.z;
                    dz = 0;
                else
                    dz = p2.z - p1.z;
        }
        else if (p1.z < 0 && p2.z < 0) {
            if (p1.z < p2.z)
                dz = abs(p1.z - p2.z);
            else
                if (p1.z == p2.z)
                    //dz = p1.z;
                    dz = 0;
                else
                    dz = abs(p2.z - p1.z);
        }
        else if (p1.z < 0 && p2.z >= 0)
            dz = p2.z - p1.z;           //ocioo, I need distance
        else if (p2.z < 0 && p1.z >= 0)
            dz = p1.z - p2.z;           //ocioo, I need distance


        if (p1.y > p2.y) {
            if (p1.x < p2.x)
                vy = -dy;       //pendenza negativa
            else
                vy = -dy;        //pendenza positiva
        }else
            if (p1.x < p2.x)
                vy = -dy;        //pendenza positiva
            else
                vy = -dy;       //pendenza negativa

        if (p1.x > p2.x) {
            if (p1.y < p2.y)
                vx = -dx;       //pendenza negativa
            else
                vx = dx;        //pendenza positiva
        }
        else
            if (p1.y < p2.y)
                vx = dx;        //pendenza positiva
            else
                vx = -dx;       //pendenza negativa

        if (p1.z > p2.z) {
            if (p1.y < p2.y)
                vz = -dz;       //pendenza negativa
            else
                vz = dz;        //pendenza positiva
        }
        else
            if (p1.y < p2.y)
                vz = dz;        //pendenza positiva
            else
                vz = -dz;       //pendenza negativa


        lenghts.push_back( sqrt( pow( sqrt(( dx*dx + dy*dy )), 2 ) + dz*dz ) );

    
        /// CHECK FOR LENGHTS MISTAKEN
        /// Problems with signs and orientation
        /// Bug, some Cylinder has the x axe inverted that i cannot catch, in this case the output of vy
        /// 
        /// DO NOT TOUCH IT IT WORKS BUT NO IDEA ON HOW NOR WHY
        /// 
        /// 

        glm::vec3 defaultOrientation = glm::vec3(0.0f, 0.0f, 1.0f);

        glm::vec3 vectorNorm = glm::normalize(glm::vec3(vy, vx, vz));
        float dotFunction = glm::dot(defaultOrientation, vectorNorm);
        float trasposingAngle = glm::acos(dotFunction);

        angles.push_back(trasposingAngle);

        vectorNorm = glm::vec3(vectorNorm.x, vectorNorm.y, 0);

        return vectorNorm;
    }

};


