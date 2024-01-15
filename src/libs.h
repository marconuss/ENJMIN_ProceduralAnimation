# pragma once
#include "renderapi.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

struct Particle
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float radius;

	Particle(glm::vec3 inPosition, glm::vec4 inColor, glm::vec3 inVelocity, glm::vec3 inAcceleration, float inRadius);
	~Particle();
};
