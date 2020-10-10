#include "application.h"
#include "imgui/myimgui.h"

using namespace gl;
using namespace glm;

Application::Application()
{
	setTitle("Application");

    ImGui::Init(sdlWindow(), context());
}

Application::~Application()
{

}

void Application::update(float dt)
{
	ImGui::RenderFrame(sdlWindow(), [&](){
		ImGui::ShowDemoWindow();
	});
}

void Application::processEvent(const SDL_Event &event)
{
	ImGui::ProcessEvent(event);
}

void Application::sizeChanged(int w, int h)
{
    glViewport(0, 0, w, h);
}
