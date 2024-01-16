# pragma once
#include "renderapi.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

struct Particle
{
public:
	float radius;
	float lifetime;
	glm::vec4 color;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float bounciness;

	float elapsedLife = 0;

	Particle(float inRadius, float inLifetime, glm::vec4 inColor, glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration, float inBounciness);

	void updateParticle(double deltaTime);
};

struct Boid
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float maxSpeed;
	float maxForce;
	float visualRange;

	Boid(glm::vec3 inPosition, glm::vec3 inVelocity, glm::vec3 inAcceleration);

	glm::vec3 separation(const std::vector<Boid>& Boids, float avoidFactor);
	glm::vec3 alignment(const std::vector<Boid>& Boids, float fieldOfVision, float alignment);
	glm::vec3 cohesion(const std::vector<Boid>& Boids, float fieldOfVision, float cohesion);

	void updateBoid(double deltaTime);

};