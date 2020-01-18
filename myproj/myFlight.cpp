#include "myFlight.h"
#include <glm/glm.hpp>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp> 

#define PI 3.14159265

void myFlight::upPitch(myObject* myPlane, myCamera* cam1) {
	
	//forwardVector = (forwardVector * cos(pitch)) + (upVector * sin(pitch))
	//upVector = forwardVector x rightVector

	cam1->camera_forward = glm::normalize(cam1->camera_forward * cosf(deltaPitch * PI / 180.0) + cam1->camera_up * sinf(deltaPitch * PI / 180.0));
	cam1->camera_up = glm::cross(cam1->camera_forward, cam1->camera_eye);

	
	
	//cam1->firstperson_rotateView(0, -1);

	/*myPlane->model_matrix = glm::mat4(1.0f);

	myPlane->model_matrix = glm::rotate(myPlane->model_matrix, 3.14f, glm::vec3(0, 1, 0));
	//scene["plane"]->model_matrix = glm::rotate(scene["plane"]->model_matrix, -3.14f / 8, glm::vec3(1, 0, 0));

	myPlane->model_matrix = glm::translate(myPlane->model_matrix, glm::vec3(0.8f, -2.4f, -7));
	myPlane->model_matrix = glm::inverse(cam1->viewMatrix()) * myPlane->model_matrix;
	*/
}

void myFlight::downPitch(myObject* myPlane, myCamera* cam1) {
	//cam1->firstperson_rotateView(0, 1);
	cam1->camera_forward = glm::normalize(cam1->camera_forward * cosf(-deltaPitch * PI / 180.0) + cam1->camera_up * sinf(-deltaPitch * PI / 180.0));
	cam1->camera_up = glm::cross(cam1->camera_forward, cam1->camera_eye);
}

void myFlight::leftRoll(myObject* myPlane, myCamera* cam1) {
	//std::cout << "Camera eye: " << glm::to_string(cam1->camera_eye) << std::endl;
	//std::cout << "Camera up: " << glm::to_string(cam1->camera_up) << std::endl;

	cam1->camera_eye = glm::normalize(cam1->camera_eye * cosf(-deltaRoll * PI / 180.0) + cam1->camera_up * sinf(-deltaRoll * PI / 180.0));
	cam1->camera_up = glm::cross(cam1->camera_forward, cam1->camera_eye);
}


void myFlight::rightRoll(myObject* myPlane, myCamera* cam1) {
	
	//std::cout << "Camera eye: " << glm::to_string(cam1->camera_eye) << std::endl;
	//std::cout << "Camera up: " << glm::to_string(cam1->camera_up) << std::endl;

	cam1->camera_eye = glm::normalize(cam1->camera_eye * cosf(deltaRoll * PI / 180.0) + cam1->camera_up * sinf(deltaRoll * PI / 180.0));
	cam1->camera_up = glm::cross(cam1->camera_forward, cam1->camera_eye);
}
void myFlight::leftYaw(myObject* myPlane, myCamera* cam1) {

	/*glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)); // Note that we convert the angle to radians first
	direction.z = sin(glm::radians(yaw));*/
}

void myFlight::rightYaw(myObject* myPlane, myCamera* cam1) {

}

void myFlight::acelerate() {
	velocity += aceleration;
	if (velocity > maxSpeed) 
		velocity = maxSpeed;
}

void myFlight::desacelerate() {
	velocity -= aceleration;
	if (velocity < minSpeed) 
		velocity = minSpeed;

}

float myFlight::getVelocity() {
	return velocity;
}