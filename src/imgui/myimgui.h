#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace ImGui {
	template <typename F>
	bool InputTextCool(const char* label, std::string* str,
			   ImGuiInputTextFlags flags = 0,
			   F callback = nullptr, void* user_data = nullptr)
	{
		auto freeCallback = [](ImGuiTextEditCallbackData* data) {
			auto& f = *static_cast<F*>(data->UserData);
			return f(data);
		};
		return InputText(label, str, flags,
				 freeCallback, &callback);
	}

	void ProcessEvent(const SDL_Event &event);

	void Init(SDL_Window *window, const SDL_GLContext &context);

	template<typename F>
	void Init(SDL_Window *window, const SDL_GLContext &context,
		const F &initFunction)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		initFunction();

		// Setup Platform/Renderer bindings
		ImGui_ImplSDL2_InitForOpenGL(window, context);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	template<typename F>
	void RenderFrame(SDL_Window *window, const F &drawFunction)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		// custom draw commands
		drawFunction();

		ImGui::Render();

		ImGuiIO *io = &ImGui::GetIO(); (void)io;
		glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);

		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

#endif
