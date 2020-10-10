#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {

	public:
		enum CameraType { LOOKAT, FPS };

		Camera();

		glm::mat4 view();
		glm::mat4 projection();
		glm::vec3 direction();

		void setType(CameraType type);
		void setPerspective(float fov, float aspect, float znear, float zfar);
		void setOrtho(float left, float right, float top, float bottom, float znear, float zfar);
		void setPosition(glm::vec3 position);
		void setRotation(glm::vec3 rotation);
		void setTranslation(glm::vec3 translation);
		void setFov(float fov);

		void updateAspectRatio(float aspect);
		void rotate(glm::vec3 delta);
		void translate(glm::vec3 delta);
		void fitInView(float xMin, float xMax, float yMin, float yMax);

		float rotationSpeed = 1.0f;
		float movementSpeed = 1.0f;

		float getFov() const;
		float getAspect() const;
		float getZnear() const;
		float getZfar() const;

	private:
		void updateView();

		CameraType type = CameraType::LOOKAT;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec4 viewPos;

		float fov;
		float aspect;
		float znear, zfar;

		bool updated = false;

		glm::mat4 projMatrix;
		glm::mat4 viewMatrix;
};

#endif // CAMERA_H
