#include "myFlight.h"
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp> 

void myFlight::upPitch(myObject* myPlane, myCamera* cam1) {
	
	cam1->firstperson_rotateView(0, -1);

	/*myPlane->model_matrix = glm::mat4(1.0f);

	myPlane->model_matrix = glm::rotate(myPlane->model_matrix, 3.14f, glm::vec3(0, 1, 0));
	//scene["plane"]->model_matrix = glm::rotate(scene["plane"]->model_matrix, -3.14f / 8, glm::vec3(1, 0, 0));

	myPlane->model_matrix = glm::translate(myPlane->model_matrix, glm::vec3(0.8f, -2.4f, -7));
	myPlane->model_matrix = glm::inverse(cam1->viewMatrix()) * myPlane->model_matrix;
	*/
}

void myFlight::downPitch(myObject* myPlane, myCamera* cam1) {
	cam1->firstperson_rotateView(0, 1);
}

void myFlight::leftRoll(myObject* myPlane, myCamera* cam1) {
	//cam1->crystalball_rotateView(-1, 1);
	cam1->camera_up -= glm::vec3(0.05f, 0, 0);
	//std::cout << glm::to_string(cam1->camera_up) << std::endl;
}

void myFlight::rightRoll(myObject* myPlane, myCamera* cam1) {
	//cam1->crystalball_rotateView(1, 1);
	cam1->camera_up += glm::vec3(0.05f, 0, 0);
	//std::cout << glm::to_string(cam1->camera_up) << std::endl;
}
void myFlight::leftYaw(myObject* myPlane, myCamera* cam1) {

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