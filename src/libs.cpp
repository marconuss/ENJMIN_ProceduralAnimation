#include "libs.h"

Particle::Particle(float inSize, glm::vec4 inColor, glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration)
{
	size = inSize;
	color = inColor;
	position = inPosition;
	velocity = inVelocity;
	acceleration = inAcceleration;


}

void Particle::updateParticle()
{
	//HELP! 
}
