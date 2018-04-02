// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//            Jeremy Hart, University of Calgary
//            John Hall, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "imageBuffer/imagebuffer.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h";
#include "rayIntersect.h"

using namespace std;
using namespace glm;

const int BUFF_SIZE = 256;

ImageBuffer imageBuffer;
int imageBufferWidth;
int imageBufferHeight;
float fieldOfView = 90.f;
float imageAspectRatio;

vector<float> vertices;
vector<float> colours;

string outputFileName;


struct MyShader
{
    // OpenGL names for vertex and fragment shaders, shader program
    GLuint  vertex;
    GLuint  fragment;
    GLuint  program;
    
    // initialize shader and program names to zero (OpenGL reserved value)
    MyShader() : vertex(0), fragment(0), program(0)
    {}
};

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data
struct MyGeometry
{
    // OpenGL names for array buffer objects, vertex array object
    GLuint  vertexBuffer;
    GLuint  colourBuffer;
    //GLuint  indiceBuffer;
    GLuint  vertexArray;
    GLsizei elementCount;
    
    // initialize object names to zero (OpenGL reserved value)
    MyGeometry() : vertexBuffer(0), colourBuffer(0), /*indiceBuffer(0),*/ vertexArray(0), elementCount(0)
    {}
};

struct TriangleSpecs
{
    vec3 trianglePointA;
    vec3 trianglePointB;
    vec3 trianglePointC;
    vec3 triangleColour;
};
struct SphereSpecs
{
    vec3 sphereCentre;
    float sphereRadius;
    vec3 sphereColour;
};
struct PlaneSpecs
{
    vec3 planeNormal;
    vec3 pointOnPlane;
    vec3 planeColour;
};

vector<TriangleSpecs> triangleFromFile;
vector<SphereSpecs> sphereFromFile;
vector<PlaneSpecs> planeFromFiles;
vec3 lightSource = vec3(1.0f);
float defuseCoefficiant;
float ambientCoefficiant;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ErrorCallback(int error, const char* description);
bool InitializeShaders(MyShader *shader);
void DestroyShaders(MyShader *shader);
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
bool CheckGLErrors();
bool InitializeGeometry(MyGeometry *geometry, vector<GLfloat> vertices, vector<GLfloat> colours);
void DestroyGeometry(MyGeometry *geometry);
int randNum(int min, int max);
vec3 generateRay(int x, int y);
float degToRad(const float &deg);
void printLines(const char* filename);
void findShapes(const char* filename);
vec3 shading(RayIntersectDetails rayIntersect);
bool isInShadow(Sphere sphere, Triangle triangle);

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("Needs filename\n");
        return -1;
    }
    
    findShapes(argv[1]);
    
    if (string(argv[1]).find("scene2") != std::string::npos) {
        defuseCoefficiant = 0.9f;
        ambientCoefficiant = 0.4f;
        outputFileName = "../imageOutputs/scene2.png";
    } else if (string(argv[1]).find("customScene") != std::string::npos) {
        defuseCoefficiant = 0.7f;
        ambientCoefficiant = 0.1f;
        outputFileName = "../imageOutputs/customScene.png";
    } else {
        defuseCoefficiant = 0.9f;
        ambientCoefficiant = 0.1f;
        outputFileName = "../imageOutputs/scene1.png";
    }
    
    // initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
        return -1;
    }
    glfwSetErrorCallback(ErrorCallback);
    
    // attempt to create a window with an OpenGL 4.1 core profile context
    GLFWwindow *window = 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    int width = 512, height = 512;
    window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }
    
    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);
    
    //Intialize GLAD
    if (!gladLoadGL())
    {
        cout << "GLAD init failed" << endl;
        return -1;
    }
    
    // call function to load and compile shader programs
    MyShader shader;
    if (!InitializeShaders(&shader)) {
        cout << "Program could not initialize shaders, TERMINATING" << endl;
        return -1;
    }

    if (!imageBuffer.Initialize()) {
        cout << "Prgram could not initialize the image buffer, TERMINATING" << endl;
        return -1;
    }
    
    MyGeometry geometry;
    if (!InitializeGeometry(&geometry, vertices, colours)) {
        cout << "Program failed to intialize geometry!" << endl;
    }
    
    imageBufferWidth = imageBuffer.Width();
    imageBufferHeight = imageBuffer.Height();
    imageAspectRatio = imageBufferWidth/imageBufferHeight;
    vec3 colour = vec3(1.f, 0, 0);
    for (int x = 0; x < imageBufferWidth; x++) {
        for (int y = 0; y < imageBufferHeight; y++) {
            float closest = 200;

            // draw planes
            for (int i = 0; i < planeFromFiles.size(); i++) {
                Plane plane(vec3(0,0,0), generateRay(x, y), planeFromFiles[i].planeNormal, planeFromFiles[i].pointOnPlane, planeFromFiles[i].planeColour);
                
                if (plane.planeIntersection()) {
                    imageBuffer.SetPixel(x, y, shading(plane.getIntersection()));
                }
            }
            
            // draw triangles
            for (int i = 0; i < triangleFromFile.size(); i++) {
                Triangle triangle(triangleFromFile[i].trianglePointA, triangleFromFile[i].trianglePointB, triangleFromFile[i].trianglePointC, vec3(0,0,0), generateRay(x, y), triangleFromFile[i].triangleColour);
                
                if (triangle.triangleIntersection()) {
                    if (triangle.getIntersection().closest < closest) {
                        closest = triangle.getIntersection().closest;
                        imageBuffer.SetPixel(x, y, shading(triangle.getIntersection()));
                    }
                }
            }

            // draw spheres
            for (int i = 0; i < sphereFromFile.size(); i++) {
                Sphere sphere(sphereFromFile[i].sphereCentre, vec3(0,0,0), generateRay(x, y), sphereFromFile[i].sphereRadius, 1.f, sphereFromFile[i].sphereColour);
                
                if (sphere.sphereIntersection()) {
                    imageBuffer.SetPixel(x, y, shading(sphere.getIntersection()));
                }
            }
        }
    }
    
    // run an event-triggered main loop
    while (!glfwWindowShouldClose(window))
    {
        imageBuffer.Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    imageBuffer.SaveToFile(outputFileName);
    
    // clean up allocated resources before exit
    DestroyGeometry(&geometry);
    DestroyShaders(&shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    cout << "Goodbye!" << endl;
    return 0;
}

void printLines(const char* filename) {
    ifstream f (filename);
    char buffer [BUFF_SIZE];
    
    while (f) {
        f.getline(buffer, BUFF_SIZE);
        
        cout << buffer << endl;
    }
    
    f.close();
}

void findShapes(const char* filename)
{
    ifstream f (filename);
    char buffer[BUFF_SIZE];
    
    TriangleSpecs triangleSpecs;
    SphereSpecs sphereSpecs;
    PlaneSpecs planeSpecs;
    float ax, ay, az;
    float bx, by, bz;
    float cx, cy, cz;
    float r, g, b;
    float radius;
    bool lightRetrieved = false;
    
    while (f) {
        f.getline(buffer, BUFF_SIZE);
        // get triangles
        if (sscanf(buffer, "triangle {%f %f %f %f %f %f %f %f %f %f %f %f}", &ax, &ay, &az, &bx, &by, &bz, &cx, &cy, &cz, &r, &g, &b) == 12) {
            triangleSpecs.trianglePointA = vec3(ax, ay, az);
            triangleSpecs.trianglePointB = vec3(bx, by, bz);
            triangleSpecs.trianglePointC = vec3(cx, cy, cz);
            triangleSpecs.triangleColour = vec3(r, g, b);
            
            triangleFromFile.push_back(triangleSpecs);
            
        // get spheres
        } else if (sscanf(buffer, "sphere {%f %f %f %f %f %f %f}", &ax, &ay, &az, &radius, &r, &g, &b) == 7) {
            sphereSpecs.sphereCentre = vec3(ax, ay, az);
            sphereSpecs.sphereRadius = radius;
            sphereSpecs.sphereColour = vec3(r, g, b);
            
            sphereFromFile.push_back(sphereSpecs);
            
        // get planes
        } else if(sscanf(buffer, "plane {%f %f %f %f %f %f %f %f %f}", &ax, &ay, &az, &bx, &by, &bz, &r, &g, &b) == 9) {
            planeSpecs.planeNormal = vec3(ax, ay, az);
            planeSpecs.pointOnPlane = vec3(bx, by, bz);
            planeSpecs.planeColour = vec3(r, g, b);
            
            planeFromFiles.push_back(planeSpecs);
            
        // get light source, since there's only one, only run this part once
        } else if ((sscanf(buffer, "light {%f %f %f}", &ax, &ay, &az) == 3) && lightRetrieved == false) {
            lightSource = vec3(ax, ay, az);
            lightRetrieved = true;
        }
    }
}

vec3 generateRay(int x, int y)
{
    vec3 ray = vec3(1.f);
    
    ray = vec3(((-1*imageBufferWidth/2.f - 0.5f) + x), ((-1*imageBufferHeight/2.f - 0.5f) + y), -950);
    
    return ray;
}

vec3 shading(RayIntersectDetails rayIntersect)
{
    vec3 pointToLight;
    float factor;
    vec3 toReturn;
    
    pointToLight = normalize(
        vec3(
             (lightSource.x - rayIntersect.position.x),
             (lightSource.y - rayIntersect.position.y),
             (lightSource.z - rayIntersect.position.z)
        )
    );
    
    factor = dot(rayIntersect.normal, pointToLight);
    
    // setting the defuse coefficiant
    toReturn = vec3(
        (defuseCoefficiant * factor * rayIntersect.colour.x),
        (defuseCoefficiant * factor * rayIntersect.colour.y),
        (defuseCoefficiant * factor * rayIntersect.colour.z)
    );
    
    // setting the ambient lighting
    toReturn += vec3(
         (ambientCoefficiant*rayIntersect.colour.x),
         (ambientCoefficiant*rayIntersect.colour.y),
         (ambientCoefficiant*rayIntersect.colour.z)
    );
    
    return toReturn;
}
/*
bool isInShadow(Sphere sphere, Triangle triangle)
{
    
}
 */

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
    // load shader source from files
    string vertexSource = LoadSource("shaders/vertex.glsl");
    string fragmentSource = LoadSource("shaders/fragment.glsl");
    if (vertexSource.empty() || fragmentSource.empty()) return false;
    
    // compile shader source into shader objects
    shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    // link shader program
    shader->program = LinkProgram(shader->vertex, shader->fragment);
    
    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);
    
    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);
    
    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }
    
    return shaderObject;
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
    // unbind any shader programs and destroy shader objects
    glUseProgram(0);
    glDeleteProgram(shader->program);
    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
}

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry, vector<GLfloat> vertices, vector<GLfloat> colours)
{
    geometry->elementCount = vertices.size()/2;
    
    // these vertex attribute indices correspond to those specified for the
    // input variables in the vertex shader
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;
    
    // create an array buffer object for storing our vertices
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    
    // create another one for storing our colours
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(float), &colours[0], GL_DYNAMIC_DRAW);
    
    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);
    
    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);
    
    // assocaite the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);
    
    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
    // unbind and destroy our vertex array object and associated buffers
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &geometry->vertexArray);
    glDeleteBuffers(1, &geometry->vertexBuffer);
    glDeleteBuffers(1, &geometry->colourBuffer);
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;
    
    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
        << filename << endl;
    }
    
    return source;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();
    
    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);
    
    // try linking the program with given attachments
    glLinkProgram(programObject);
    
    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }
    
    return programObject;
}

bool CheckGLErrors()
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
            case GL_INVALID_ENUM:
                cout << "GL_INVALID_ENUM" << endl; break;
            case GL_INVALID_VALUE:
                cout << "GL_INVALID_VALUE" << endl; break;
            case GL_INVALID_OPERATION:
                cout << "GL_INVALID_OPERATION" << endl; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
            case GL_OUT_OF_MEMORY:
                cout << "GL_OUT_OF_MEMORY" << endl; break;
            default:
                cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

int randNum(int min, int max)
{
    int x = rand() % min +  max;
    return x;
}

float degToRad(const float &deg)
{
    return deg * M_PI / 180;
}
