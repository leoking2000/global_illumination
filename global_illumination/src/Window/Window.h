#pragma once
#include "Global/Types.h"
#include "Global/Timer.h"
#include "Keys.h"

#include "imgui/imgui.h"

#include <string>
#include <functional>
#include <unordered_map>

namespace GL
{
	struct WindowParameters
	{
		u32 width = 1600u;
		u32 height = 900u;
		std::string title = "";
		bool isResizable = true;
		bool vsync = false;
	};

	class Window
	{
	public:
		Window(const WindowParameters& params);

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		~Window();
	public:
		using UpdateCallBackFn = std::function<void(f32)>;

		// it calls Update every frame
		void RunMainLoop(UpdateCallBackFn Update);
	public:
		// The key is From the Keys.h file.
		bool KeyIsPress(u32 key) const;

		// will return true only in the first frame the key was pressed
		// the user will have to stop pressing the key and then press it again to activate the action
		bool KeyIsPressAsButton(u32 key) const;

		bool MouseButtonIsPress(u32 key) const;

		// makes RunMainLoop() to return!!!
		void CloseWindow();

		void ResizeWindow(u32 width, u32 height);

		glm::vec2 WindowSize() const;

		glm::vec2 MousePos() const;

		void SetMouseVisibility(bool visible);

		void SetWindowTitle(const std::string& title);

		f32 ElapsedTime() const; // in seconds
	private:
		bool ShouldWindowClose();
		void NewFrame();
		void EndFrame();
	private:
		void* m_window;
		Timer m_timer;
		f32 m_elapsedTime;
		mutable std::unordered_map<u32, bool> m_keyStates; // use in KeyIsPressAsButton
	};
}
