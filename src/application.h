#ifndef APPLICATION_H
#define APPLICATION_H

#include "opengl/opengl.h"

class Application : public gl::OpenGLWindow {
	public:
		Application();
		~Application();

	protected:
		void update(float dt);
		void processEvent(const SDL_Event &event);
};

#endif
