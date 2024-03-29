#include "viewer.h"
#include "drawbuffer.h"
#include "renderapi.h"
#include "libs.h"

#include <time.h>
#include <vector>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>


#define COUNTOF(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define BOUNCE_ARRAY_SIZE 5

constexpr char const* viewerName = "MyViewer";
constexpr glm::vec4 white = { 1.f, 1.f, 1.f, 1.f };
constexpr glm::vec4 blue = { 0.f, 0.f, 1.f, 1.f };
constexpr glm::vec4 green = { 0.f, 1.f, 0.f, 1.f };
constexpr glm::vec4 red = { 1.f, 0.f, 0.f, 1.f };


struct VertexShaderAdditionalData {
	float bouncePower;
	float bounceRadius;
	float bounceDuration;
	float bounceSpeed;

	int count;
	int padding[3];
	glm::vec4 posAndTime[BOUNCE_ARRAY_SIZE];
};

struct MyViewer : Viewer {

	//-----------
	// particles
	/*
	std::vector<Particle> particles;
	float particleRadius = 0.05f;
	float particleLifetime = 5.f;
	float particleBounciness = 0.8f;
	glm::vec4 particleColor = white;
	int spawningRate = 100;
	float gravityIntensity = -3;
	double spawningTimer = 0;
	glm::vec3 particleDirection;
	float initialVelocityFactor = 1;
	*/

	// boids
	/*
	std::vector<Boid> boids;
	float coherence;
	float separation;
	float alignment;
	float visualRange;
	*/


	double lastFrameElapsedTime = 0;

	// Forward Kinematics
	std::vector<Joint> bones;

	// IK
	glm::vec3 targetPosition;
	Joint hip = Joint(glm::vec3(0.f, 0.f, 0.f), glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));
	Joint knee = Joint(glm::vec3(0.f, 0.f, 0.f), glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));
	Joint heel = Joint(glm::vec3(0.f, 0.f, 0.f), glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));

	float hipAngle_0 = 0;
	float hipAngle_1 = 0;
	float kneeAngle_0 = 0;
	float kneeAngle_1 = 0;

	//-----------

	glm::vec3 jointPosition;
	glm::vec3 cubePosition;
	glm::vec3 ballPosition;
	float boneAngle;


	glm::vec2 mousePos;

	bool leftMouseButtonPressed;
	bool altKeyPressed;

	glm::vec3 mouseRayDir;
	glm::vec3 mouseRayPos;

	VertexShaderAdditionalData additionalShaderData;

	MyViewer() : Viewer(viewerName, 1280, 720) {}

	void init() override {

		/*
		// Boids
		coherence = 0.5f;
		separation = 1.f;
		alignment = 1.f;
		visualRange = 0.1f;
		*/
		cubePosition = glm::vec3(1.f, 0.25f, -1.f);
		jointPosition = glm::vec3(-1.f, 2.f, -1.f);
		ballPosition = glm::vec3(-1.f, 0.5f, 1.f);
		boneAngle = 0.f;
		mousePos = { 0.f, 0.f };
		leftMouseButtonPressed = false;

		altKeyPressed = false;

		// Boids
		/*
		for (int i = 0; i < 200; i++)
		{
			float xrand = (4*rand() / (float)RAND_MAX) -2;
			float yrand = (4*rand() / (float)RAND_MAX);
			float zrand = (4*rand() / (float)RAND_MAX) -2;
			glm::vec3 randomPos = glm::vec3(xrand, yrand, zrand);
			glm::vec3 initialRandomAcceleration = glm::vec3(-1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1 - (-1.0f)))), -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0f - (-1.0f)))), -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0f - (-1.0f)))));

			boids.push_back(Boid(randomPos, glm::vec3(0.f, 0.f, 0.f), initialRandomAcceleration));
		}
		*/

		additionalShaderData.bouncePower = 0.5f;
		additionalShaderData.bounceRadius = 1.0f;
		additionalShaderData.bounceDuration = 1.f;
		additionalShaderData.bounceSpeed = 30.f;
		additionalShaderData.count = 0;

		// Forward Kinematic
		/*
		for (int i = 0; i < 3; i++)
		{
			glm::quat q = glm::angleAxis(boneAngle, glm::vec3{ 0.f, 1.f, 0.f });
			bones.push_back(Joint(glm::vec3(0.f, 0.f, 0.f), q, glm::vec3{ 0.f, 0.f, 0.f }));
		}
		*/

		// IK
		targetPosition = glm::vec3(0.f, 0.f, 0.f);

		// root
		hip.RelativePosition = glm::vec3(0.f, 2.f, 0.f);
		hip.RelativeRotation = glm::identity<glm::quat>();
		hip.AbsolutePosition = hip.RelativePosition;
		hip.AbsoluteRotation = hip.RelativeRotation;

		knee.RelativePosition = glm::vec3(0.f, -1.f, 0.f);
		heel.RelativePosition = glm::vec3(0.f, -1.f, 0.f);

	}


	void update(double elapsedTime) override {

		float deltaTime = (float)elapsedTime - (float)lastFrameElapsedTime;

		//boneAngle = (float)elapsedTime;

		leftMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		altKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		mousePos = { float(mouseX), viewportHeight - float(mouseY) };

		if (leftMouseButtonPressed) {
			float xrand = (4 * rand() / (float)RAND_MAX) - 2;
			float zrand = (4 * rand() / (float)RAND_MAX) - 2;
			glm::vec4 posAndTime = glm::vec4(xrand, 2.0, zrand, elapsedTime);

			if (additionalShaderData.count < BOUNCE_ARRAY_SIZE) {
				additionalShaderData.posAndTime[additionalShaderData.count] = posAndTime;
				additionalShaderData.count++;
			}
			else {

				int oldest = 0;
				for (size_t i = 0; i < additionalShaderData.count; i++) {
					if (additionalShaderData.posAndTime[i].w < additionalShaderData.posAndTime[oldest].w) {
						oldest = i;
					}
				}
				additionalShaderData.posAndTime[oldest] = posAndTime;

			}
		}


		pCustomShaderData = &additionalShaderData;
		CustomShaderDataSize = sizeof(VertexShaderAdditionalData);

		// Particles
		/*
		spawningTimer += (elapsedTime - lastFrameElapsedTime);
		if(spawningTimer > 1/spawningRate)
		{
			particles.push_back(Particle(particleRadius, particleLifetime, particleColor, glm::vec3(0.1f, 0.1f, 0.f), glm::vec3(float(rand()) / float((RAND_MAX)), initialVelocityFactor, float(rand()) / float((RAND_MAX))), glm::vec3(0.f, -3.f, 0.f), particleBounciness));
			spawningTimer = 0;
		}


		std::vector<struct Particle>::iterator it;
		for (it = particles.begin(); it < particles.end();) {

			(*it).updateParticle(elapsedTime - lastFrameElapsedTime);
			if ((*it).elapsedLife >= (*it).lifetime) {
				it = particles.erase(it);
			} else {
				++it;
			}
		}
		*/

		// Boids
		/*
		std::vector<struct Boid>::iterator it;
		for (it = boids.begin(); it < boids.end();) {
			glm::vec3 sep = it->separation(boids, separation);
			glm::vec3 ali = it->alignment(boids, visualRange, alignment);
			glm::vec3 coe = it->cohesion(boids, visualRange, coherence);

			it->acceleration += sep;
			it->acceleration += ali;
			it->acceleration += coe;

			it->updateBoid(deltaTime);
			++it;
		}
		*/

		// Forward Kinematic
		// calculate bones0
		/*
		for (int i = 0; i < bones.size(); i++)
		{
			glm::vec3 childRelPos = { 0.f, 1.f, 0.f };
			glm::quat q = glm::eulerAngleXYZ(glm::radians(bones[i].angles.x), glm::radians(bones[i].angles.y), glm::radians(bones[i].angles.z));
			glm::vec3 childAbsPos = q * childRelPos;

			bones[i].RelativePosition = childRelPos;
			bones[i].RelativeRotation = q;


			if (i > 0)
			{
				bones[i].updateJoint(bones[i - 1].AbsoluteRotation, bones[i - 1].AbsolutePosition);
			}
			else
			{
				glm::vec3 initialPosition = glm::vec3(0.f, 0.f, 0.f);
				glm::quat initialRotation = glm::angleAxis(boneAngle, glm::vec3{ 0.f, 1.f, 0.f });
				bones[i].RelativePosition = initialPosition;
				bones[i].AbsoluteRotation = bones[i].RelativeRotation;
				bones[i].AbsolutePosition = bones[i].RelativePosition;
			}
		}
		*/

		// IK
		float targetDistance = glm::clamp(glm::distance(targetPosition, hip.AbsolutePosition), 0.1f, 1.9f); 

		hipAngle_0 = glm::clamp(glm::acos(glm::dot(glm::normalize(heel.AbsolutePosition - hip.AbsolutePosition), glm::normalize(knee.AbsolutePosition - hip.AbsolutePosition))), -1.f, 1.f);
		kneeAngle_0 = glm::clamp(glm::acos(glm::dot(glm::normalize(hip.AbsolutePosition - knee.AbsolutePosition), glm::normalize(heel.AbsolutePosition - knee.AbsolutePosition))), -1.f, 1.f);

		hipAngle_1 = glm::acos(glm::clamp((1.f - 1.f - targetDistance * targetDistance)/(-2.f * 1.f * targetDistance), -1.f, 1.f));
		kneeAngle_1= glm::acos(glm::clamp((targetDistance * targetDistance -1.f -1.f)/(-2.f * 1.f * 1.f), -1.f, 1.f));

		//vec3 axis0 = normalize(cross(c - a, b - a));
		//quat r0 = quat_angle_axis(ac_ab_1 - ac_ab_0, quat_mul(quat_inv(a_gr), axis0)));
		//quat r1 = quat_angle_axis(ba_bc_1 - ba_bc_0, quat_mul(quat_inv(b_gr), axis0)));

		glm::vec3 axis0 = glm::normalize(glm::cross(heel.AbsolutePosition - hip.AbsolutePosition, knee.AbsolutePosition - hip.AbsolutePosition));
		//glm::quat r0 = glm::angleAxis(hipAngle_1 - hipAngle_0, glm::cross(glm::inverse(hip.AbsoluteRotation)));

		// hip		
		hip.updateJoint(glm::identity<glm::quat>(), glm::vec3(0.f, 0.f,0.f));
		// knee
		knee.updateJoint(hip.AbsoluteRotation, hip.AbsolutePosition);
		// heel
		heel.updateJoint(knee.AbsoluteRotation, knee.AbsolutePosition);


		lastFrameElapsedTime = elapsedTime;


	}

	void render3D_custom(const RenderApi3D& api) const override {
		//Here goes your drawcalls affected by the custom vertex shader
		//api.horizontalPlane(glm::vec3( 0., 2.0, 0. ), { 4, 4 }, 200, glm::vec4(0.0f, 0.2f, 1.f, 1.f));
	}

	void render3D(const RenderApi3D& api) const override {
		api.horizontalPlane({ 0, 0, 0 }, { 10, 10 }, 1, glm::vec4(0.9f, 0.9f, 0.9f, 1.f));

		api.grid(10.f, 10, glm::vec4(0.5f, 0.5f, 0.5f, 1.f), nullptr);

		api.axisXYZ(nullptr);


		/*
		constexpr float cubeSize = 0.5f;
		glm::mat4 cubeModelMatrix = glm::translate(glm::identity<glm::mat4>(), cubePosition);
		api.solidCube(cubeSize, white, &cubeModelMatrix);

		{
			glm::vec3 vertices[] = {
				{0.5f * cubeSize, 0.5f * cubeSize, 0.5f * cubeSize},
				{0.f, cubeSize, 0.f},
				{0.5f * cubeSize, 0.5f * cubeSize, -0.5f * cubeSize},
				{0.f, cubeSize, 0.f},
				{-0.5f * cubeSize, 0.5f * cubeSize, 0.5f * cubeSize},
				{0.f, cubeSize, 0.f},
				{-0.5f * cubeSize, 0.5f * cubeSize, -0.5f * cubeSize},
				{0.f, cubeSize, 0.f},
			};
			api.lines(vertices, COUNTOF(vertices), white, &cubeModelMatrix);
		}

		{
			glm::quat q = glm::angleAxis(boneAngle, glm::vec3(0.f, 1.f, 0.f));
			glm::vec3 childRelPos = { 1.f, 1.f, 0.f };
			api.bone(childRelPos, white, q, glm::vec3(0.f, 0.f, 0.f));
			glm::vec3 childAbsPos = q * childRelPos;
			api.solidSphere(childAbsPos, 0.05f, 10, 10, white);
		}

		api.solidSphere(ballPosition, 0.5f, 100, 100, white);

		{
			glm::vec3 vertices[] = {
				{ mouseRayPos.x, mouseRayPos.y, mouseRayPos.z},
				{ mouseRayPos.x + mouseRayDir.x * 10.f, mouseRayPos.y + mouseRayDir.y * 10.f, mouseRayPos.z + mouseRayDir.z * 10.f},
			};
			api.lines(vertices, COUNTOF(vertices), white, nullptr);
		}*/

		// boids
		/*
		for each(Boid b in boids)
		{
			api.solidSphere(b.position, 0.05f, 5, 5, white);
		}
		*/

		// Forward Kinematic
		/*
		for (int i = 0; i < bones.size(); i++)
		{
			if (i > 0)
			{
				api.bone(bones[i].RelativePosition, white, bones[i - 1].AbsoluteRotation, bones[i - 1].AbsolutePosition);
				api.solidSphere(bones[i].AbsolutePosition, 0.05f, 5, 5, white);
			}
		}
		*/

		// IK

		// knee bone
		api.bone(knee.RelativePosition, white, hip.AbsoluteRotation, hip.AbsolutePosition);
		api.solidSphere(knee.AbsolutePosition, 0.05f, 5, 5, white);

		// heel bone
		api.bone(heel.RelativePosition, white, knee.AbsoluteRotation, knee.AbsolutePosition);
		api.solidSphere(heel.AbsolutePosition, 0.05f, 5, 5, white);

		// target
		api.solidSphere(targetPosition, 0.1f, 5, 5, red);

	}

	void render2D(const RenderApi2D& api) const override {

		constexpr float padding = 50.f;

		if (altKeyPressed) {
			if (leftMouseButtonPressed) {
				api.circleFill(mousePos, padding, 10, white);
			}
			else {
				api.circleContour(mousePos, padding, 10, white);
			}

		}
		else {
			const glm::vec2 min = mousePos + glm::vec2(padding, padding);
			const glm::vec2 max = mousePos + glm::vec2(-padding, -padding);
			if (leftMouseButtonPressed) {
				api.quadFill(min, max, white);
			}
			else {
				api.quadContour(min, max, white);
			}
		}
		/*
		{
			const glm::vec2 from = { viewportWidth * 0.5f, padding };
			const glm::vec2 to = { viewportWidth * 0.5f, 2.f * padding };
			constexpr float thickness = padding * 0.25f;
			constexpr float hatRatio = 0.3f;
			api.arrow(from, to, thickness, hatRatio, white);
		}

		{
			glm::vec2 vertices[] = {
				{ padding, viewportHeight - padding },
				{ viewportWidth * 0.5f, viewportHeight - 2.f * padding },
				{ viewportWidth * 0.5f, viewportHeight - 2.f * padding },
				{ viewportWidth - padding, viewportHeight - padding },
			};
			api.lines(vertices, COUNTOF(vertices), white);
		}
		*/
	}

	void drawGUI() override {
		static bool showDemoWindow = false;

		ImGui::Begin("3D Sandbox");

		ImGui::Checkbox("Show demo window", &showDemoWindow);

		ImGui::ColorEdit4("Background color", (float*)&backgroundColor, ImGuiColorEditFlags_NoInputs);

		ImGui::Separator();


		//ImGui::SliderFloat("Point size", &pointSize, 0.1f, 10.f);
		//ImGui::SliderFloat("Line Width", &lineWidth, 0.1f, 10.f);
		//ImGui::Separator();
		ImGui::SliderFloat3("Light Position", (float(&)[3])lightPosition, -10.f, 10.f);
		ImGui::SliderFloat("Ligh Ambient", &lightAmbient, 0.f, 0.5f);
		ImGui::SliderFloat("Ligh Specular", &lightSpecular, 0.f, 1.f);
		ImGui::SliderFloat("Ligh Specular Pow", &lightSpecularPow, 1.f, 200.f);
		//ImGui::Separator();
		//ImGui::SliderFloat3("CustomShader_Pos", &additionalShaderData.Pos.x, -10.f, 10.f);
		ImGui::Separator();
		float fovDegrees = glm::degrees(camera.fov);
		if (ImGui::SliderFloat("Camera field of fiew (degrees)", &fovDegrees, 15, 180)) {
			camera.fov = glm::radians(fovDegrees);
		}

		//ImGui::SliderFloat3("Cube Position", (float(&)[3])cubePosition, -1.f, 1.f);

		// particles
		/*
		ImGui::SliderInt("Particle SpawningRate", &spawningRate, 1, 1000);
		ImGui::SliderFloat("Particle Radius", &particleRadius, 0.01f, 0.1f);
		ImGui::SliderFloat("Gravity Intensity", &gravityIntensity, -0.1f, -10.f);
		ImGui::SliderFloat("Particle Lifetime", &particleLifetime, 0.1f, 10.f);
		ImGui::SliderFloat("Particle Bounciness", &particleBounciness, 0.1f, 1.f);
		ImGui::SliderFloat("Initial Velocity Factor", &initialVelocityFactor, 1.f, 10.f);
		ImGui::ColorEdit4("Particle color", (float*)&particleColor, ImGuiColorEditFlags_NoInputs);*/

		// boids
		/*
		 *ImGui::SliderFloat("Coherence", &coherence, 0.f, 3.f);
		ImGui::SliderFloat("Separation", &separation, 0.f, 3.f);
		ImGui::SliderFloat("Alignment", &alignment, 0.f, 3.f);
		ImGui::SliderFloat("Visual Range", &visualRange, 0.f, 3.f);
		*/

		// forward kinematic

		ImGui::SliderFloat("Hip Angle X", &hip.angles.x, 0.f, 360.f);
		ImGui::SliderFloat("Hip Angle Y", &hip.angles.y, 0.f, 360.f);
		ImGui::SliderFloat("Hip Angle Z", &hip.angles.z, 0.f, 360.f);

		ImGui::SliderFloat("Knee Angle X", &knee.angles.x, 0.f, 360.f);
		ImGui::SliderFloat("Knee Angle Y", &knee.angles.y, 0.f, 360.f);
		ImGui::SliderFloat("Knee Angle Z", &knee.angles.z, 0.f, 360.f);

		ImGui::SliderFloat3("Cube Position", (float(&)[3])targetPosition, -1.f, 1.f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		if (showDemoWindow) {
			// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			ImGui::ShowDemoWindow(&showDemoWindow);
		}
	}
};

int main(int argc, char** argv) {
	MyViewer v;
	return v.run();
}
