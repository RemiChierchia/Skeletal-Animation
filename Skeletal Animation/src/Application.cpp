#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Skeleton.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "SkinnyIcosphere.h"
#include "SkinnyCylinder.h"
#include "SkyboxGenerator.h"
#include "Framebuffer.h"
#include "MD5MyModel.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const bool antiAliasing = true; // if on, 4 frames are taken

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int initial_time = time(NULL), final_time, frame_count = 0;

int th = 0;
int ph = 0;

// lighting
glm::vec3 sunPos(1000.0f, 500.0f, -500.0f);



int main(void)
{
    GLFWwindow* window;
    // glfw window creation
    // --------------------
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4); // MSAA by default in framebuffer for anti-aliasing

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSwapInterval(0); //syncronize with gsync 0 = off, 1 = on


    //glewExperimental = GL_TRUE;
    // glew: load all OpenGL function pointers
    // ----------------------------------------
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    
    std::cout << glGetString(GL_VERSION) << std::endl;



    /// ////////////////////////////////
    /// Skybox
    /// ////////////////////////////////

    Shader skybox("res/shaders/Skybox.shader");

    SkyboxGenerator skyGen;

    VertexArray vaSky;
    vaSky.Bind();

    VertexBuffer vbSky(skyGen.getVertices(), skyGen.getVerticesSize());

    VertexBufferLayout skyLayout;
    skyLayout.Push<float>(3);
    vaSky.AddBuffer(vbSky, skyLayout);

    skybox.Bind();
    skybox.SetUniform1i("skybox", 0);



    /// //////////////////////////////////
    /// Icospheres and Cylinders
    /// //////////////////////////////////

    Shader sphereshader("res/shaders/Basic.shader");

    SkinnyIcosphere skinnyico(50, 3); //50, 3
    SkinnyCylinder skinnycyl(30, 30, 1, 30);

    VertexArray vaIcosphere, vaCylinder, lightSphere, lightCylinder;
    vaIcosphere.Bind();
    vaCylinder.Bind();
    lightSphere.Bind();
    lightCylinder.Bind();

    VertexBuffer vbIcosphere(skinnyico.getInterleavedVertices(), skinnyico.getInterleavedVertexSize());
    VertexBuffer vbCylinder(skinnycyl.getInterleavedVertices(), skinnycyl.getInterleavedVertexSize());

    VertexBufferLayout layout;
    layout.Push<float>(3); // Positions
    layout.Push<float>(3); // Normals
    vaIcosphere.AddBuffer(vbIcosphere,layout);
    vaCylinder.AddBuffer(vbCylinder,layout);


    IndexBuffer ibIcosphere(skinnyico.getIndices(), skinnyico.getIndexCount());
    IndexBuffer ibCylinder(skinnycyl.getIndices(), skinnycyl.getIndexCount());



    /// ////////////////////////////////
    /// Sun and lighting
    /// ////////////////////////////////

    Shader sun("res/shaders/Sun.shader");
    Shader lighting("res/shaders/Lighting.shader");

    SkinnyIcosphere sunSphere(1000, 3);
    VertexArray vaSun;
    vaSun.Bind();

    VertexBuffer vbSun(sunSphere.getInterleavedVertices(), sunSphere.getInterleavedVertexSize());

    VertexBufferLayout sunLayout;
    sunLayout.Push<float>(3); // Positions
    sunLayout.Push<float>(3); // Normals
    vaSun.AddBuffer(vbSun, sunLayout);

    IndexBuffer ibSun(sunSphere.getIndices(), sunSphere.getIndexCount());



    /// //////////////////////////////////
    /// Framebuffer
    /// //////////////////////////////////

    Shader screenShader("res/shaders/ScreenShader.shader");

    Framebuffer frame(antiAliasing, SCR_WIDTH, SCR_HEIGHT);

    VertexArray vaQuad;
    vaQuad.Bind();
    VertexBuffer vbQuad(frame.getQuadVertices(), frame.getQuadVerticesSize());
    VertexBufferLayout layoutQuad;
    layoutQuad.Push<float>(2);
    layoutQuad.Push<float>(2);
    vaQuad.AddBuffer(vbQuad, layoutQuad);

    screenShader.Bind();
    screenShader.SetUniform1i("texFramebuffer", 0);

    GLCall(glEnable(GL_BLEND)); //if these are active quads without texture dont show
    GLCall(glEnable(GL_DEPTH_TEST)); //for rotating stuffs
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));



    /// //////////////////////////////////
    /// Load Models
    /// //////////////////////////////////

    Model ThoraxCentered("res/objects/skeleton/ThoraxCentered.obj");
    Model Head("res/objects/skeleton/Head.obj");
    Model BoneCentered("res/objects/skeleton/BoneCentered.obj");
    Model PelvisTest("res/objects/skeleton/PelvisTest.obj");

    Shader skeletonShader("res/shaders/Basic.shader");
    Shader shaderModel("res/shaders/Model.shader");

    MD5MyModel Model;
    GLCall(Model.LoadModel("res/objects/boblamp/boblampclean.md5mesh"));
    GLCall(Model.LoadAnim("res/objects/boblamp/boblampclean.md5anim"));

    VertexArray vaModelBobBody; // Not efficient but for now I dont know how to concatenate, since for indexbuffers is not possible
    vaModelBobBody.Bind();
    VertexArray vaModelBobHead;
    vaModelBobHead.Bind();
    VertexArray vaModelBobHelmet;
    vaModelBobHelmet.Bind();
    VertexArray vaModelLantern;
    vaModelLantern.Bind();
    VertexArray vaModelLanternTop;
    vaModelLanternTop.Bind();
    VertexArray vaModelBobBody2;
    vaModelBobBody2.Bind();

    Model.Update(0.1); // It is needed to load the first animation
    GLCall(Model.ComputeAllData());

    VertexBuffer vbModelBobBody     (Model.getAllData(0), Model.getAllDataDimension(0)*sizeof(float)); //controllaaa
    VertexBuffer vbModelBobHead     (Model.getAllData(1), Model.getAllDataDimension(1)*sizeof(float));
    VertexBuffer vbModelBobHelmet   (Model.getAllData(2), Model.getAllDataDimension(2)*sizeof(float));
    VertexBuffer vbModelLantern     (Model.getAllData(3), Model.getAllDataDimension(3)*sizeof(float));
    VertexBuffer vbModelLanternTop  (Model.getAllData(4), Model.getAllDataDimension(4)*sizeof(float));
    VertexBuffer vbModelBobBody2    (Model.getAllData(5), Model.getAllDataDimension(5)*sizeof(float));

    VertexBufferLayout layoutModel;
    layoutModel.Push<float>(3); // Positions
    layoutModel.Push<float>(3); // Normals
    layoutModel.Push<float>(2); // Texture
    vaModelBobBody.AddBuffer    (vbModelBobBody,    layoutModel);
    vaModelBobHead.AddBuffer    (vbModelBobHead,    layoutModel);
    vaModelBobHelmet.AddBuffer  (vbModelBobHelmet,  layoutModel);
    vaModelLantern.AddBuffer    (vbModelLantern,    layoutModel);
    vaModelLanternTop.AddBuffer (vbModelLanternTop, layoutModel);
    vaModelBobBody2.AddBuffer   (vbModelBobBody2,   layoutModel);

    IndexBuffer ibModelBobBody      (Model.getIndexBuffer(0), Model.getIndexNumber(0)); //funzioneaaaaaaaa
    IndexBuffer ibModelBobHead      (Model.getIndexBuffer(1), Model.getIndexNumber(1));
    IndexBuffer ibModelBobHelmet    (Model.getIndexBuffer(2), Model.getIndexNumber(2)); 
    IndexBuffer ibModelLantern      (Model.getIndexBuffer(3), Model.getIndexNumber(3));
    IndexBuffer ibModelLanternTop   (Model.getIndexBuffer(4), Model.getIndexNumber(4));
    IndexBuffer ibModelBobBody2     (Model.getIndexBuffer(5), Model.getIndexNumber(5));
    std::cout << Model.getIndexNumber(0) << std::endl;
    ///
    /// Still have to see how to render multiple vertex buffers with each one a different index buffer on the same vertex array
    /// Multiple index buffering is not supported from OpenGL cause it is not supported from hardware
    ///
    
    shaderModel.Bind();



    /// //////////////////////////////////
    /// Json Reader
    /// //////////////////////////////////

    Skeleton ourSkeleton("res/jsonSkeletons/0.json");

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    int counter = 0;
    int index = 0;
    int shifting = 0;

    /// Time
    double lastReading = glfwGetTime();

    Renderer renderer;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        if (currentFrame - lastReading >= 0.1) { // print each second
            if (index <= 370) { //whole json files
                ourSkeleton.Update("res/jsonSkeletons/" + to_string(index++) + ".json");

            }

            sunPos = glm::vec3(1000.0f - shifting, 500.0f, -500.0f);
            
            Model.Update(0.1);
            GLCall(Model.ComputeAllData());
            /// You stupid moron, this is not an efficient method at all
            vbModelBobBody.Update   (Model.getAllData(0), Model.getAllDataDimension(0) * sizeof(float));
            vbModelBobHead.Update   (Model.getAllData(1), Model.getAllDataDimension(1) * sizeof(float));
            vbModelBobHelmet.Update (Model.getAllData(2), Model.getAllDataDimension(2) * sizeof(float));
            vbModelLantern.Update   (Model.getAllData(3), Model.getAllDataDimension(3) * sizeof(float));
            vbModelLanternTop.Update(Model.getAllData(4), Model.getAllDataDimension(4) * sizeof(float));
            vbModelBobBody2.Update  (Model.getAllData(5), Model.getAllDataDimension(5) * sizeof(float));


            shifting += 5;
            lastReading += 0.1;
        }

        nbFrames++;
        if (currentFrame - lastTime >= 1.0) { // print each second
            // printf and reset timer
            float FPS = 1 / deltaTime;
            //float FPS = 1000.0 / double(nbFrames);
    
            std::cout << "FPS : " << FPS << std::endl;
            nbFrames = 0;
            lastTime += 1.0; 
        }
        
        // input
        // -----
        processInput(window);



        /// /////////////////////////////////////////////////
        /// Framebuffer init and draw scene
        /// /////////////////////////////////////////////////

        renderer.Clear();

        if(antiAliasing == true)
            frame.SetAntiAliasing();
        else
            frame.Bind();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);


        shaderModel.Bind();
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 10000.0f);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, -1000.0f));
        glm::mat4 modelStart = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, -1000.0f));

        shaderModel.SetUniformMat4f("projection", projection);
        shaderModel.SetUniformMat4f("view", view);

        model = glm::scale(modelStart, glm::vec3(10.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        shaderModel.SetUniformMat4f("model", model);

        shaderModel.SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        shaderModel.SetUniform3f("lightPos", sunPos);

        glBindTexture(GL_TEXTURE_2D, Model.getTexture(0));
        renderer.Draw(vaModelBobBody, ibModelBobBody, shaderModel);
        glBindTexture(GL_TEXTURE_2D, Model.getTexture(1));
        renderer.Draw(vaModelBobHead, ibModelBobHead, shaderModel);
        glBindTexture(GL_TEXTURE_2D, Model.getTexture(2));
        renderer.Draw(vaModelBobHelmet, ibModelBobHelmet, shaderModel);
        glBindTexture(GL_TEXTURE_2D, Model.getTexture(3));
        renderer.Draw(vaModelLantern, ibModelLantern, shaderModel);
        glBindTexture(GL_TEXTURE_2D, Model.getTexture(4));
        renderer.Draw(vaModelLanternTop, ibModelLanternTop, shaderModel);
        glBindTexture(GL_TEXTURE_2D, Model.getTexture(5));
        renderer.Draw(vaModelBobBody2, ibModelBobBody2, shaderModel);


        /// /////////////////////////////////////////////////
        /// With lightings
        /// /////////////////////////////////////////////////
        lighting.Bind();
        lighting.SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        lighting.SetUniform3f("lightPos", sunPos);

        // pass projection matrix to shader (note that in this case it could change every frame)
        projection = glm::perspective(glm::radians(camera.Zoom), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 10000.0f);

        // camera/view transformation
        view = camera.GetViewMatrix();
        model = glm::mat4(1.0f);

        lighting.SetUniformMat4f("view", view);
        lighting.SetUniformMat4f("projection", projection);

        for (int i = 0; i < 17; i++)
        {
            model = glm::translate(glm::mat4(1.0f), ourSkeleton.getSpherePosition()[i]);
            lighting.SetUniformMat4f("model", model);
            renderer.Draw(vaIcosphere, ibIcosphere, lighting);           
        }

        for (int i = 0; i < 16; i++)
        {
            model = glm::translate(glm::mat4(1.0f), ourSkeleton.getCylinderPosition()[i]);
            model = glm::rotate(model, ourSkeleton.getAngles(i), ourSkeleton.getCylinderRotation()[i]);   
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, ourSkeleton.getLenghts(i)));
            lighting.SetUniformMat4f("model", model);
            renderer.Draw(vaCylinder, ibCylinder, lighting);
        }

        skeletonShader.Bind();
        skeletonShader.SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        skeletonShader.SetUniform3f("lightPos", sunPos);
        skeletonShader.SetUniformMat4f("view", view);
        skeletonShader.SetUniformMat4f("projection", projection);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 0.0f));

        /// Thorax
        model = glm::translate(model, ourSkeleton.getCylinderPosition()[7]);
        model = glm::rotate(model, ourSkeleton.getAngles(7), ourSkeleton.getCylinderRotation()[7]);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f));
        skeletonShader.SetUniformMat4f("model", model);
        ThoraxCentered.Draw(skeletonShader);

        /// Head
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 0.0f));
        model = glm::translate(model, ourSkeleton.getCylinderPosition()[9]);
        model = glm::rotate(model, ourSkeleton.getAngles(9), ourSkeleton.getCylinderRotation()[9]);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1000.0f));
        skeletonShader.SetUniformMat4f("model", model);
        Head.Draw(skeletonShader);

        /// Bones
        for (int i = 0; i < 16; i++)
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 0.0f));
            if (i != 9 && i != 7 && i != 0 && i != 1) {
                model = glm::translate(model, ourSkeleton.getCylinderPosition()[i]);
                model = glm::rotate(model, ourSkeleton.getAngles(i), ourSkeleton.getCylinderRotation()[i]);
                model = glm::scale(model, glm::vec3(ourSkeleton.getLenghts(i)/300.0f)); //so much better
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                skeletonShader.SetUniformMat4f("model", model);
                BoneCentered.Draw(skeletonShader);
            }

            /// Pelvic
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 0.0f));
            if (i == 0) {
                model = glm::translate(model, ourSkeleton.getSpherePosition()[i]);
                model = glm::rotate(model, ourSkeleton.getAngles(16), ourSkeleton.getPelvicRotation());
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(50000.0f));
                skeletonShader.SetUniformMat4f("model", model);
                PelvisTest.Draw(skeletonShader);
            }
        }

        /// Articulations
        for (int i = 0; i < 17; i++)
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-800.0f, 0.0f, 0.0f));
            model = glm::translate(model, ourSkeleton.getSpherePosition()[i]);
            model = glm::scale(model, glm::vec3(0.5f));
            skeletonShader.SetUniformMat4f("model", model);
            renderer.Draw(vaIcosphere, ibIcosphere, skeletonShader);
        }


        /// /////////////////////////////////////////////////
        /// Sun draw
        /// /////////////////////////////////////////////////

        sun.Bind();
        sun.SetUniformMat4f("projection", projection);
        sun.SetUniformMat4f("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, sunPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        sun.SetUniformMat4f("model", model);
        renderer.Draw(vaSun, ibSun, sun);


        /// /////////////////////////////////////////////////
        /// Skybox draw
        /// /////////////////////////////////////////////////
        
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        
        skybox.Bind();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skybox.SetUniformMat4f("view", view);
        skybox.SetUniformMat4f("projection", projection);
   
        vaSky.Bind();
        skyGen.Draw();

        glDepthFunc(GL_LESS); // set depth function back to default


        /// /////////////////////////////////////////////////
        /// Draw 2D screen scene with Anti-aliasing 4x if on
        /// /////////////////////////////////////////////////        

        if (antiAliasing == true)
            frame.BindAntiAliasing();
        else
            frame.Unbind();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vaQuad.Bind();
        screenShader.Bind();

        glDrawArrays(GL_TRIANGLES, 0, 6);

    
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
     
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &vaIcosphere);
    //glDeleteBuffers(1, &vbIcosphere);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
 

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
