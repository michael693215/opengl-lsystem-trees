//
// Simple GL example
//


#include "GLapp.hpp"
#include "Plane.hpp"
#include "Shader.hpp"
#include"LSystem.hpp"
#include "Tree.hpp"
#include "Mat.inl"
#include "Vec.inl"
#include <algorithm>
#include <iostream>
#include <string>
#include <cstring>
#include <limits>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

///////
// GLFW callbacks, must use extern "C"
extern "C" {
    // called for GLFW error
    void error(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    // called whenever the window size changes
    void reshape(GLFWwindow *win, int width, int height) {
        // save window dimensions
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        glfwGetFramebufferSize(win, &app->width, &app->height);

        // viewport size matches window size
        glViewport(0, 0, app->width, app->height);
    }

    // called when mouse button is pressed
    void mousePress(GLFWwindow *win, int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        app->active = true;
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(win, &app->mouseX, &app->mouseY);
    }

    // called when mouse is moved
    void mouseMove(GLFWwindow *win, double x, double y) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        if (!app->active) return;

        // rotation angle, scaled so across the window = one rotation
        app->pan += float(F_PI * float(x - app->mouseX) / app->width);
        app->tilt += float(0.5f*F_PI * float(y - app->mouseY) / app->height);

        // remember location so next update will be relative to this one
        app->mouseX = x;
        app->mouseY = y;
    }

    // called on any keypress
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);

        if (action == GLFW_PRESS) {
            switch (key) {
            case 'A':                   // rotate left
                app->panRate = -F_PI;  // half a rotation/sec
                return;

            case 'D':                   // rotate right
                app->panRate = F_PI;   // half a rotation/sec
                return;

            case 'W':                   // rotate up
                app->tiltRate = 0.5f * F_PI; // 1/4 rotation/sec
                return;

            case 'S':                   // rotate down
                app->tiltRate = -0.5f * F_PI; // 1/4 rotation/sec
                return;

            case 'R':                   // reload shaders
                for (auto object : app->objects)
                    object->updateShaders();
                return;

            case 'I':                   // cycle through light intensity
                app->scene.LightDir[3] += 0.2f;
                if (app->scene.LightDir[3] > 1.f)
                    app->scene.LightDir[3] = 0.f;
                return;

            case 'L':                   // toggle lines or solid
                app->wireframe = !app->wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, app->wireframe ? GL_LINE : GL_FILL);
                return;
            
            case 'O':
                app->zoomRate = 30.f * F_PI;
                return;

            case 'P':
                app->zoomRate = -30.f * F_PI;
                return;
            
            case 'J':
                app->generations++;
                app->regenerateTree();
                return;
            
            case 'K':
                app->generations = std::max(app->generations - 1, static_cast<size_t>(1));
                app->regenerateTree();
                return;


            case GLFW_KEY_ESCAPE:                    // Escape
                if (app->active) {                   //  1st press, release mouse
                    app->active = false;
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else                                 //  2nd press, exit                 
                    glfwSetWindowShouldClose(win, true);
                return;
            }
        }

        if (action == GLFW_RELEASE) {
            switch (key) {
            case 'A': case 'D':         // stop panning
                app->panRate = 0;
                return;
            case 'W': case 'S':         // stop tilting
                app->tiltRate = 0;
                return;
            case 'O': case 'P':         // stop zooming
                app->zoomRate = 0;
                return;
            }
        }
    }
}

// initialize GLFW - windows and interaction
GLapp::GLapp(char *_rule, size_t _generations) : generations(_generations), rule(_rule)
{
    // member data initialization
    active = false;                             // not tracking mouse input
    width = 843; height = 480;                  // window size
    distance = 600.f; pan = 0.f; tilt = -1.4f;  // view
    panRate = tiltRate = zoomRate = 0.f;                   // keyboard view control
    mouseX = mouseY = 0.f;                      // mouse view controls
    wireframe = false;                          // solid drawing

    // set error callback before init
    glfwSetErrorCallback(error);
    int ok = glfwInit();
    assert(ok);

    // OpenGL version: YOU MAY NEED TO ADJUST VERSION OR OPTIONS!
    // When figuring out the settings that will work for you, make
    // sure you can see error messages on console output.
    //
    // Some drivers needs FORWARD_COMPAT, but others will need to comment that out.
    // Likely changes for other versions:
    //   All versions: change VERSION_MAJOR and VERSION_MINOR
    //   OpenGL 3.0 (2008): does not support features we need
    //   OpenGL 3.1 (2009):
    //     comment out GLFW_OPENGL_PROFILE line
    //     Use "#version 140\n
    //   OpenGL 3.2 (2009): Use "#version 150 core\n"
    //   OpenGL 3.3 (2010): Use "#version 330 core\n"
    //   Any of 4.0-4.6:
    //     Similar to 3.3: #version uses <MAJOR><MINOR>0
    //     For example, 4.6 is "#version 460 core\n"
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    ShaderInfo::ShaderVersion = "#version 410 core\n";

    // ask for a window with dimensions 843 x 480 (HD 480p)
    win = glfwCreateWindow(width, height, "Simple OpenGL Application", 0, 0);
    assert(win);

    glfwMakeContextCurrent(win);

    // GLEW handles OpenGL shared library access
    glewExperimental = true;
    glewInit();

    // set callback functions to be called by GLFW
    glfwSetWindowUserPointer(win, this);
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);

    // tell OpenGL to enable z-buffer for overlapping surfaces
    glEnable(GL_DEPTH_TEST);

    // only draw front faces of objects (assumes counter-clockwise vertex order)
    glEnable(GL_CULL_FACE);

    // initialize buffer for scene shader data
    glGenBuffers(1, &sceneUniformsID);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), 0, GL_STREAM_DRAW);

    // initialize scene data
    scene.LightDir = fVec4{-1,-2,2,1};

    // add tree
    objects.push_back(new Plane(fVec3{500.f, 500.f, 100.f}, "rocks"));
    objects.push_back(new Tree(rule, generations));
}

///////
// Clean up any context data
GLapp::~GLapp() 
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

// call before drawing each frame to update per-frame scene state
void GLapp::sceneUpdate(double dTime)
{
    pan += float(panRate * dTime);
    tilt += float(tiltRate * dTime);
    distance += float(zoomRate * dTime);

    scene.ProjFromWorld = 
        perspective<float>(F_PI/4.f, (float)width/height, 1.f, 10000.f)
        * translate<float>(fVec3{0,0,-distance})
        * xrotate<float>(tilt)
        * zrotate<float>(pan);
    scene.WorldFromProj = inverse(scene.ProjFromWorld);

    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData), &scene);
}

// render a frame
void GLapp::render()
{
    // consistent time for drawing this frame
    double currTime = glfwGetTime();
    double dTime = currTime - prevTime;

    // clear old screen contents to a sky blue
    glClearColor(0.5, 0.7, 0.9, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all objects
    sceneUpdate(dTime);
    for (auto object : objects)
        object->draw(this, currTime);

    // show what we drew
    glfwSwapBuffers(win);
    prevTime = currTime;
}

void GLapp::regenerateTree()
{
    objects.clear();
    objects.push_back(new Plane(fVec3{500.f, 500.f, 100.f}, "rocks"));
    objects.push_back(new Tree(rule, generations));
    render();
}

void usage(char*);

int main(int argc, char *argv[])
{
	char *program = argv[0];
    char *rule = nullptr;
    int generations = 5;
	for (++argv, --argc; argc > 0; ++argv, --argc) 
	{
		// help command skips to usage guide
		if (strncmp(argv[0], "-h", 2) == 0 || strncmp(argv[0], "--h", 3) == 0)
		{
			usage(program);
			return 1;
		}

		// parse options
		if (argc > 1 && argv[0][0] == '-')
		{
			int len = strlen(++argv[0]);
			for ( ; len > 0; ++argv[0], --len)
			{
				if (argv[0][0] == 'g') 
				{
					size_t digits;
					try 
					{
						generations = stoi(std::string(argv[0] + 1), &digits);
					}
					catch (const std::invalid_argument&)
					{
						std::cerr << "Invalid number of generations.\n";
						usage(program);
						return 1;
					}
					if (generations < 0)
					{
						std::cerr << "Number of generations cannot be negative.\n";
						return 1;
					}
					len -= digits;
					argv[0] += digits;
				}

				else  
				{
					std::cerr << "Invalid option: " << argv[0][0] << '\n';
					usage(program);
					return 1;
				}
			}
			continue;
		}

		// last argument is the rule 
		if (argc == 1)
		{
			rule = argv[0];
			continue;
		}
        usage(program);
        return 1;
	}
	if (!rule || argc != 0)
	{
		usage(program);
		return 1;
	}

    // initialize windows and OpenGL
    GLapp app(rule, generations);

    // add some objects to draw

    // set up initial viewport
    reshape(app.win, app.width, app.height);

    // each frame: render then check for events
    while (!glfwWindowShouldClose(app.win)) {
        app.render();
        glfwPollEvents();
    }

    return 0;
}

void usage(char *program) 
{
	std::cout << "Expected usage: " << program << " -[OPTIONS] rule.txt\n"
		<< "\tOPTIONS:\n"
		<< "\t\tg(number)\n"
		<< "\t\t\tSets the number of generations equal to (number). If not specified, the program will use g = 5.\n"
        << "\t\trule.txt" 
        << "\t\t\tThis file contains the rule that the lystem will use to create the tree.\n";
}