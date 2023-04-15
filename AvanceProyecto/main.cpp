#include "libraries.h"

#include "Camera.h"
#include "Variables.h"

#include "Shader.h"
#include "Model.h"

void initGLFWVersion();
bool gladLoad();
void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Model ourModel1);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static int mini(int x, int y);
static int maxi(int x, int y);
GLFWmonitor* get_current_monitor(GLFWwindow* window);

void TransformacionObjeto(Shader ourShader, Model ourModel, vec3 poseModel);
void CameraTransform(Shader ourShader);

int main()
{
    initGLFWVersion();

    GLFWwindow* window = glfwCreateWindow(w, h, "Modelo", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoad())
    {
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertexShader.vs", "fragmenShader.fs");

    stbi_set_flip_vertically_on_load(true);
    Model ourModel("Recursos/Modelos/backpack/backpack.obj");
    
    //Model ourModel1("Recursos/Modelos/Islas/Lowpoly_tree_sample.obj");
    Model ourModel1("Recursos/Modelos/Escenario/escenario2.obj");
    updateWindow(window, ourShader, ourModel, ourModel1);

    glfwTerminate();
    return 0;
}

void initGLFWVersion()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}
bool gladLoad()
{
    bool active = true;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    return active;
}
void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Model ourModel1)
{
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);

        //Directional
        ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("dirLight.direction", lightDirect);

        //Point
        for (int i = 0; i < totalP; ++i) {
            string i_str = to_string(i);
            ourShader.setVec3("pointLights[" + i_str + "].ambient", 0.05f, 0.05f, 0.05f);
            ourShader.setVec3("pointLights[" + i_str + "].diffuse", 0.8f, 0.8f, 0.8f);
            ourShader.setVec3("pointLights[" + i_str + "].specular", 1.0f, 1.0f, 1.0f);
            ourShader.setVec3("pointLights[" + i_str + "].pose", pointLightPositions[i]);
            ourShader.setFloat("pointLights[" + i_str + "].constant", constantV);
            ourShader.setFloat("pointLights[" + i_str + "].linear", linearV);
            ourShader.setFloat("pointLights[" + i_str + "].quadratic", quadraticV);
        }

        //Spot
        if (attached) 
        {
            ourShader.setVec3("spotLight.pose", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
        }
        else 
        {
            ourShader.setVec3("spotLight.pose", droppedPos);
            ourShader.setVec3("spotLight.direction", droppedDir);
        }
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", constantV);
        ourShader.setFloat("spotLight.linear", linearV);
        ourShader.setFloat("spotLight.quadratic", quadraticV);
        ourShader.setFloat("spotLight.cutOff", cos(radians(cutOffValue)));
        ourShader.setFloat("spotLight.outerCutOff", cos(radians(outerCutOffValue)));

        CameraTransform(ourShader);

        TransformacionObjeto(ourShader, ourModel, vec3(0.0f, 0.0f, 0.0f));
        TransformacionObjeto(ourShader, ourModel1, vec3(1.0f, 1.0f, 1.0f));

        if (poly)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    w = width;
    h = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    //Camara move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) 
    {
        firstMouse = true;
        if (!glfwGetWindowMonitor(window)) 
        {
            
            glfwGetWindowPos(window, &s_Xpos, &s_Ypos);
            glfwGetWindowSize(window, &s_w, &s_h);

            GLFWmonitor* monitor = get_current_monitor(window);

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else 
        {
            glfwSetWindowMonitor(window, nullptr, s_Xpos, s_Ypos, s_w, s_h, 0);
        }
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS) 
    {
        if (attached) 
        {
            droppedPos = camera.Position;
            droppedDir = camera.Front;
            attached = false;
        }
        else 
        {
            attached = true;
        }
    }
}

static int mini(int x, int y)
{
    return x < y ? x : y;
}

static int maxi(int x, int y)
{
    return x > y ? x : y;
}

GLFWmonitor* get_current_monitor(GLFWwindow* window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor* bestmonitor;
    GLFWmonitor** monitors;
    const GLFWvidmode* mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) 
    {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) *
            maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}

void CameraTransform(Shader ourShader)
{
    mat4 projection = perspective(radians(camera.Zoom), (float)w / (float)h, 0.1f, 100.0f);
    mat4 view = camera.GetViewMatrix();

    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
}

void TransformacionObjeto(Shader ourShader, Model ourModel, vec3 poseModel)
{
    mat4 model = mat4(1.0f);
    model = translate(model, poseModel);
    model = scale(model, vec3(1.0f, 1.0f, 1.0f));
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);
}