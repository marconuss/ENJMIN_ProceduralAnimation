#include "libs.h"

Particle::Particle(float inRadius, float inLifetime, glm::vec4 inColor, glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration, float inBounciness)
{
	radius = inRadius;
	lifetime = inLifetime;
	color = inColor;
	position = inPosition;
	velocity = inVelocity;
	acceleration = inAcceleration;
	bounciness = inBounciness;
}

void Particle::updateParticle(double deltaTime)
{
	if (position.y-radius < 0.02f && velocity.y < 0) {
		velocity.y = -velocity.y*bounciness;
	}
	velocity += acceleration* (float)deltaTime;
	position += velocity* (float)deltaTime;

	elapsedLife += deltaTime;
}
