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
	if (position.y - radius < 0.02f && velocity.y < 0) {
		velocity.y = -velocity.y * bounciness;
	}
	velocity += acceleration * (float)deltaTime;
	position += velocity * (float)deltaTime;

	elapsedLife += deltaTime;
}

Boid::Boid(glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration)
{
	position = inPosition;
	velocity = inVelocity;
	acceleration = inAcceleration;
}

glm::vec3 Boid::separation(const std::vector<Boid>& boids, float avoidFactor)
{
	glm::vec3 steer = glm::vec3(0.f, 0.f, 0.f);
	//int count = 0;

	for (int i = 0; i < boids.size(); i++)
	{
		float d = distance(position, boids[i].position);
		if ((d > 0) && d < visualRange)
		{
			steer += position - boids[i].position;
		}
	}
	if (glm::length(steer) > 0)
	{
		steer = glm::normalize(steer);
	}
	return steer * avoidFactor;
}

glm::vec3 Boid::alignment(const std::vector<Boid>& boids, float fieldOfVision, float alignment)
{
	glm::vec3 sumVelocity = glm::vec3(0.f, 0.f, 0.f);

	int count = 0;

	for (int i = 0; i < boids.size(); i++)
	{
		float d = distance(position, boids[i].position);
		if ((d > 0) && d < fieldOfVision)
		{
			glm::vec3 boidVelocity = boids[i].velocity;
			sumVelocity += boidVelocity;
			count++;
		}
	}
	if (count > 0)
	{
		sumVelocity /= (float)count;
		glm::vec3 steer = glm::vec3(0.f, 0.f, 0.f);
		steer = sumVelocity - velocity;
		return steer * alignment;
	}
	else
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}
}

glm::vec3 Boid::cohesion(const std::vector<Boid>& boids, float fieldOfVision, float cohesion)
{
	glm::vec3 sumPosition = glm::vec3(0.f, 0.f, 0.f);

	int count = 0;

	for (int i = 0; i < boids.size(); i++)
	{
		float d = distance(position, boids[i].position);
		//if ((d > 0) && d < fieldOfVision)
		//{
		glm::vec3 boidPosition = boids[i].position;
		sumPosition += boidPosition;
		count++;
		//}
	}
	if (count > 0)
	{
		sumPosition /= float(count);
		glm::vec3 steer = glm::vec3(0.f, 0.f, 0.f);
		steer = glm::normalize(sumPosition - position);
		return steer * cohesion;
	}
	else
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}
}

void Boid::updateBoid(double deltaTime)
{
	velocity += acceleration * (float)deltaTime;
	float maxSpeed = 1.f;
	if (glm::length(velocity) > maxSpeed)
	{
		velocity = glm::normalize(velocity) * maxSpeed;

	}
	if ((position.y < 0.f || position.y > 4.1f) && (velocity.y * position.y) > 0) {
		velocity.y = -velocity.y;
	}
	if ((position.x < -2.f || position.x > 2.f) && (velocity.x * position.x) > 0) {
		velocity.x = -velocity.x;
	}
	if ((position.z < -2.f || position.z > 2.f) && (velocity.z * position.z) > 0) {
		velocity.z = -velocity.z;
	}

	position += velocity * (float)deltaTime;
}
