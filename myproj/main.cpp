#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main


#include "helperFunctions.h" // not used

#include "myShader.h"
#include "myCamera.h"
#include "mySubObject.h"

#include <glm/glm.hpp>
#include <iostream>
#include "myObject.h"
#include "myLights.h"
#include "myFBO.h"
#include "default_constants.h"
#include "myScene.h"
#include "myPhysics.h"
#include "myShaders.h"
#include "myFlight.h"

using namespace std;

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;

int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;
bool windowsize_changed = true;
bool crystalballorfirstperson_view = false;
float movement_stepsize = DEFAULT_KEY_MOVEMENT_STEPSIZE;

// Camera parameters.
myCamera *cam1;

// All the meshes 
myScene scene;
myPhysics physics;

myFlight flight;

//Triangle to draw to illustrate picking
size_t picked_triangle_index = 0;
myObject *picked_object = nullptr;
float airplane_pos_z = 0;
// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
		// window close button is pressed
	case SDL_QUIT:
	{
		quit = true;
		break;
	}
	case SDL_KEYDOWN:
	{
		if (current_event.key.keysym.sym == SDLK_ESCAPE)
			quit = true;
		if (current_event.key.keysym.sym == SDLK_r)
			cam1->reset();
		
		if (current_event.key.keysym.sym == SDLK_u)
		{
			//initializing the position of the ball to where the camera is.
			scene["ball"]->model_matrix = glm::mat4(1.0f);
			scene["ball"]->translate(glm::vec3(0, 0, -10));
			scene["ball"]->model_matrix = glm::inverse(cam1->viewMatrix()) * scene["ball"]->model_matrix;
			physics.setModelMatrix(scene["ball"]);

			//applying force to the ball along the direction of camera_forward.
			physics[scene["ball"]]->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			btTransform t = physics[scene["ball"]]->getWorldTransform();
			((btRigidBody *) physics[scene["ball"]])->setMotionState(new btDefaultMotionState(t));
			((btRigidBody *)physics[scene["ball"]])->setLinearVelocity(btVector3(cam1->camera_forward.x*60, cam1->camera_forward.y * 60, cam1->camera_forward.z * 60));

			//applying force to mario along the direction of camera_forward.
			/*t = physics[scene["mario"]]->getWorldTransform();
			((btRigidBody *)physics[scene["mario"]])->setMotionState(new btDefaultMotionState(t));
			((btRigidBody *)physics[scene["mario"]])->setLinearVelocity(btVector3(cam1->camera_forward.x * 60, cam1->camera_forward.y * 60, cam1->camera_forward.z * 60));*/
		}
		if (current_event.key.keysym.sym == SDLK_UP) {
			
			flight.upPitch(scene["airplane"], cam1);
			physics.setModelMatrix(scene["airplane"]);

			airplane_pos_z += 0.1f;
			/*
			//applying force to the ball along the direction of camera_forward.
			physics[scene["plane"]]->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			btTransform t = physics[scene["plane"]]->getWorldTransform();
			((btRigidBody*)physics[scene["plane"]])->setMotionState(new btDefaultMotionState(t));
			((btRigidBody*)physics[scene["plane"]])->setLinearVelocity(btVector3(cam1->camera_forward.x * 60, cam1->camera_forward.y * 60, cam1->camera_forward.z * 60));
			*/
						
		}
		// plane forward
		if (current_event.key.keysym.sym == SDLK_w) {

			flight.acelerate();
			cam1->moveForward(flight.getVelocity());
		}
		
		if (current_event.key.keysym.sym == SDLK_s) {
			flight.desacelerate();
			cam1->moveForward(flight.getVelocity());
			//cam1->moveBack(movement_stepsize * 10);
		}


		if (current_event.key.keysym.sym == SDLK_DOWN) {
			
			flight.downPitch(scene["airplane"], cam1);
			physics.setModelMatrix(scene["airplane"]);

			airplane_pos_z += 0.1f;
		}
			

		if (current_event.key.keysym.sym == SDLK_a) {
			cam1->turnLeft(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		}
			
		if (current_event.key.keysym.sym == SDLK_LEFT) {
			flight.leftRoll(scene["airplane"], cam1);
			physics.setModelMatrix(scene["airplane"]);
		}
		
		
		if (current_event.key.keysym.sym == SDLK_d)
			cam1->turnRight(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		if (current_event.key.keysym.sym == SDLK_RIGHT) {
			flight.rightRoll(scene["airplane"], cam1);
			physics.setModelMatrix(scene["airplane"]);
		}
		if (current_event.key.keysym.sym == SDLK_v)
			crystalballorfirstperson_view = !crystalballorfirstperson_view;
		else if (current_event.key.keysym.sym == SDLK_o)
		{
			//nfdchar_t *outPath = NULL;
			//nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);
			//if (result != NFD_OKAY) return;

			//myObject *obj_tmp = new myObject();
			//if (!obj_tmp->readObjects(outPath))
			//{
			//	delete obj_tmp;
			//	return;
			//}
			//delete obj1;
			//obj1 = obj_tmp;
		}
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		mouse_position[0] = current_event.button.x;
		mouse_position[1] = current_event.button.y;
		mouse_button_pressed = true;

		const Uint8 *state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_LCTRL])
		{
			glm::vec3 ray = cam1->constructRay(mouse_position[0], mouse_position[1]);
			scene.closestObject(ray, cam1->camera_eye, picked_object, picked_triangle_index);
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		mouse_button_pressed = false;
		break;
	}
	case SDL_MOUSEMOTION:
	{
		int x = current_event.motion.x;
		int y = current_event.motion.y;

		int dx = x - mouse_position[0];
		int dy = y - mouse_position[1];

		mouse_position[0] = x;
		mouse_position[1] = y;

		if ((dx == 0 && dy == 0) || !mouse_button_pressed) return;

		if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && crystalballorfirstperson_view)
			cam1->crystalball_rotateView(dx, dy);
		else if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalballorfirstperson_view)
			cam1->firstperson_rotateView(dx, dy);
		else if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT))
			cam1->panView(dx, dy);

		break;
	}
	case SDL_WINDOWEVENT:
	{
		if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
			windowsize_changed = true;
		break;
	}
	case SDL_MOUSEWHEEL:
	{
		if (current_event.wheel.y < 0)
			cam1->moveBack(DEFAULT_MOUSEWHEEL_MOVEMENT_STEPSIZE*100);
		else if (current_event.wheel.y > 0)
			cam1->moveForward(DEFAULT_MOUSEWHEEL_MOVEMENT_STEPSIZE*100);
		break;
	}
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	// Using OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create window
	window = SDL_CreateWindow("IN4I24", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	cam1 = new myCamera();
	SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);


	checkOpenGLInfo(true);




	/**************************INITIALIZING LIGHTS ***************************/
	scene.lights = new myLights();
	scene.lights->lights.push_back(new myLight(glm::vec3(1, 0, 0), glm::vec3(0.5, 0.5, 0.5), myLight::POINTLIGHT));
	scene.lights->lights.push_back(new myLight(glm::vec3(0, -1, 0), glm::vec3(0.6, 0.6, 0.6), myLight::POINTLIGHT));


	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/
	myObject *obj;

	//the big christmas scene.
	/*obj = new myObject();
	obj->readObjects("models/Road_Sign/Road_Sign/RoadSign/RoadSign.obj", true, false);
	obj->createmyVAO();
	scene.addObject(obj, "road");
	physics.addObject(obj, myPhysics::CONCAVE, btCollisionObject::CF_STATIC_OBJECT, 0.0f, 0.7f);
	*/
	
	// airplane
	obj = new myObject();
	obj->readObjects("models/FA38/FA38_landed.obj", true, false);
	obj->scaleObject(0.0005f, 0.0005f, 0.0005f);
	obj->createmyVAO();
	scene.addObject(obj, "airplane");
	physics.addObject(obj, myPhysics::CONVEX, btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK, 10.0f, 1.0f);
	physics.setModelMatrix(scene["airplane	"]);
	
	// scene

	obj = new myObject();
	obj->readObjects("models/land/Stena.obj", true, false);
	obj->scaleObject(20, 20, 20);
	obj->createmyVAO();
	scene.addObject(obj, "Stena");
	physics.addObject(obj, myPhysics::CONVEX, btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);


	/**************************SETTING UP OPENGL SHADERS ***************************/
	myShaders shaders;
	shaders.addShader(new myShader("shaders/basic-vertexshader.glsl", "shaders/basic-fragmentshader.glsl"), "shader_basic");
	shaders.addShader(new myShader("shaders/phong-vertexshader.glsl", "shaders/phong-fragmentshader.glsl"), "shader_phong");
	shaders.addShader(new myShader("shaders/texture-vertexshader.glsl", "shaders/texture-fragmentshader.glsl"), "shader_texture");
	shaders.addShader(new myShader("shaders/texture+phong-vertexshader.glsl", "shaders/texture+phong-fragmentshader.glsl"), "shader_texturephong");
	shaders.addShader(new myShader("shaders/bump-vertexshader.glsl", "shaders/bump-fragmentshader.glsl"), "shader_bump");
	shaders.addShader(new myShader("shaders/imageprocessing-vertexshader.glsl", "shaders/imageprocessing-fragmentshader.glsl"), "shader_imageprocessing");
	shaders.addShader(new myShader("shaders/environmentmap-vertexshader.glsl", "shaders/environmentmap-fragmentshader.glsl"), "shader_environmentmap");

	myShader *curr_shader;
	physics.setTime(SDL_GetPerformanceCounter() / static_cast<double>(SDL_GetPerformanceFrequency()));
	
	// display loop
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);
			windowsize_changed = false;
		}

		//Computing transformation matrices. Note that model_matrix will be computed and set in the displayScene function for each object separately
		glViewport(0, 0, cam1->window_width, cam1->window_height);
		glm::mat4 projection_matrix = cam1->projectionMatrix();
		glm::mat4 view_matrix = cam1->viewMatrix();
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix)));

		//Setting uniform variables for each shader
		for (unsigned int i=0;i<shaders.size();i++)
		{
			curr_shader = shaders[i];
			curr_shader->start();
			curr_shader->setUniform("myprojection_matrix", projection_matrix);
			curr_shader->setUniform("myview_matrix", view_matrix);
			scene.lights->setUniform(curr_shader, "Lights");
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		physics.stepSimulation( SDL_GetPerformanceCounter() / static_cast<double>(SDL_GetPerformanceFrequency()) );


		
		// everything that has a texture put it after here
		curr_shader = shaders["shader_texturephong"];
		curr_shader->start();

		// cockpit vision
		scene["airplane"]->model_matrix = glm::mat4(1.0f);
		scene["airplane"]->model_matrix = glm::rotate(scene["airplane"]->model_matrix, 3.14f, glm::vec3(0, 1, 0));
		scene["airplane"]->model_matrix = glm::translate(scene["airplane"]->model_matrix, glm::vec3(0.8f, -2.4f, -7));
		scene["airplane"]->model_matrix = glm::inverse(cam1->viewMatrix()) * scene["airplane"]->model_matrix;

		scene["airplane"]->displayObjects(curr_shader, view_matrix);

		//scene
		scene["Stena"]->model_matrix = glm::mat4(1.0f);
		scene["Stena"]->model_matrix = glm::translate(scene["Stena"]->model_matrix, glm::vec3(0, 20, -1000.1f));
		scene["Stena"]->displayObjects(curr_shader, view_matrix);
		/*-----------------------*/
		//cam1->moveForward(movement_stepsize);

		SDL_GL_SwapWindow(window);

		SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0)
			processEvents(current_event);
	}

	// Freeing resources before exiting.

	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}