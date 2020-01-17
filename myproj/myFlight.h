#pragma once

#include "myObject.h"
#include "myCamera.h"
#include <iostream>
#include "glm/ext.hpp"

class myFlight
{
	float velocity = 1.0;
	float aceleration = 1.0;
	float desaceleration = 0.4;
	float maxSpeed = 10.0;
	float minSpeed = 1.0;
public:

		void upPitch(myObject*, myCamera*);
		void downPitch(myObject*, myCamera*);
		void leftRoll(myObject*, myCamera*);
		void rightRoll(myObject*, myCamera*);
		void leftYaw(myObject*, myCamera*);
		void rightYaw(myObject*, myCamera*);
		float getVelocity();
		void acelerate();
		void desacelerate();

};
