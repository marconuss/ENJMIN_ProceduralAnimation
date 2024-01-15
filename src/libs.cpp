#include "libs.h"

Particle::Particle(float inRadius, glm::vec4 inColor, glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration)
{
	radius = inRadius;
	color = inColor;
	position = inPosition;
	velocity = inVelocity;
	acceleration = inAcceleration;
}


/*Particle::~Particle()
{
	delete this;
}
*/
void Particle::updateParticle()
{
	velocity += acceleration;
	position += velocity;
}
