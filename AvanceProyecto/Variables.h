
unsigned int w = 800;
unsigned int h = 600;
int s_w = w;
int s_h = h;
int s_Xpos = 0;
int s_Ypos = 0;
const bool poly = false;

//Camera
mat4 projection, view, model;

float deltaTime = 0.0f, lastFrame = 0.0f;
Camera camera(vec3(0.0f, 0.0f, 6.0f));
float lastX = w / 2.0f;
float lastY = h / 2.0f;
bool firstMouse = true;

// Luz
int totalP = 1;
vec3 droppedPos = camera.Position;
vec3 droppedDir = camera.Front;
vec3 lightPos(0.5f, 0.2f, 1.0f);
vec3 lightDirect(-0.2f, -1.0f, -0.3f);

float constantV = 1.0f;
float linearV = 0.09f;
float quadraticV = 0.032f;

float cutOffValue = 12.5f;
float outerCutOffValue = 17.5f;

bool attached = true;

vec3 pointLightPositions[] = {
        vec3(0.7f,  0.2f,  2.0f),
        vec3(2.3f, -3.3f, -4.0f),
        vec3(-4.0f,  2.0f, -12.0f),
        vec3(0.0f,  0.0f, -3.0f)
};
