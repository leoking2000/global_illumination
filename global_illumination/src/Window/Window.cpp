#include "Window.h"
#include "Graphics/OpenGL.h"
#include "Global/Logger.h"

#include <GLFW/glfw3.h>
#include <exception>
#include <stdexcept>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void glfw_window_size_callback(GLFWwindow* glfw_window, int width, int height)
{
	GL::Window* win = reinterpret_cast<GL::Window*>(glfwGetWindowUserPointer(glfw_window));
	win->ResizeWindow(width, height);
}

void glfw_error_callback(int code, const char* msg)
{
	LOGERROR("[GLFW] ERROR %i | %s", code, msg);
}

void glfw_init()
{
	// Set GLFW error callback
	glfwSetErrorCallback(glfw_error_callback);

	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		LOGERROR("GLFW could not Initialize.");
		throw std::runtime_error("GLFW could not Initialize.");
	}
}

namespace GL
{
	Window::Window(const WindowParameters& params)
	{
		// Initialize GLFW
		static bool GLFW_Initialized = false;
		if (!GLFW_Initialized)
		{
			glfw_init();
			GLFW_Initialized = true;
		}

		// Hits
		// OpenGL version 4.6
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, params.isResizable ? GL_TRUE : GL_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		// Create Window
		GLFWwindow* glfw_window;
		glfw_window = glfwCreateWindow(params.width, params.height, params.title.c_str(), NULL, NULL);
		if (!glfw_window)
		{
			LOGERROR("Window creation failed.");
			glfwTerminate();
			throw std::runtime_error("Window creation failed.");
		}
		LOGINFO("Created Window");

		glfwSetWindowUserPointer(glfw_window, this);
		glfwSetWindowSizeCallback(glfw_window, glfw_window_size_callback);
		glfwMakeContextCurrent(glfw_window);

		// Initialize OpenGL
		static bool OpenGL_Initialized = false;
		if (!OpenGL_Initialized)
		{
			InitOpenGL();
			OpenGL_Initialized = true;
		}
		
		// vsync
		glfwSwapInterval(params.vsync ? 1 : 0);

		glCall(glEnable(GL_MULTISAMPLE));
		glCall(glViewport(0, 0, params.width, params.height));

		// Initialize ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		m_window = glfw_window;
		m_elapsedTime = m_timer.Elapsed();
	}


	Window::~Window()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		GLFWwindow* glfw_window = reinterpret_cast<GLFWwindow*>(m_window);

		glfwDestroyWindow(glfw_window);
		glfwTerminate();

		LOGINFO("Destroyed Window");
	}

	void Window::RunMainLoop(UpdateCallBackFn Update)
	{
		while (!ShouldWindowClose())
		{
			NewFrame();
			Update(ElapsedTime());
			EndFrame();
		}
	}

	bool Window::KeyIsPress(u32 key) const
	{
		return glfwGetKey(reinterpret_cast<GLFWwindow*>(m_window), (int)key) == GLFW_PRESS;
	}

	bool Window::KeyIsPressAsButton(u32 key) const
	{
		if (m_keyStates.find(key) == m_keyStates.end())
		{
			m_keyStates[key] = false;
		}

		if (KeyIsPress(key) && m_keyStates[key] == false)
		{
			m_keyStates[key] = true;
			return true;
		}
		else if (!KeyIsPress(key))
		{
			m_keyStates[key] = false;
		}

		return false;
	}

	bool Window::MouseButtonIsPress(u32 key) const
	{
		return glfwGetMouseButton(reinterpret_cast<GLFWwindow*>(m_window), key) == GLFW_PRESS;
	}

	bool Window::ShouldWindowClose()
	{
		return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_window));
	}

	void Window::CloseWindow()
	{
		glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_window), true);
	}

	void Window::ResizeWindow(u32 width, u32 height)
	{
		glfwSetWindowSize(reinterpret_cast<GLFWwindow*>(m_window), (int)width, (int)height);

		glCall(glViewport(0, 0, width, height));
	}

	glm::vec2 Window::WindowSize() const
	{
		int width, height;
		glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_window), &width, &height);

		return glm::vec2((float)width, (float)height);
	}

	glm::vec2 Window::MousePos() const
	{
		double xpos, ypos;
		glfwGetCursorPos(reinterpret_cast<GLFWwindow*>(m_window), &xpos, &ypos);

		return glm::vec2((float)xpos, (float)ypos);
	}

	void Window::SetMouseVisibility(bool visible)
	{
		glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_window), GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	void Window::SetWindowTitle(const std::string& title)
	{
		glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_window), title.c_str());
	}

	f32 Window::ElapsedTime() const
	{
		return m_elapsedTime;
	}

	void Window::NewFrame()
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Window::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_window));

		m_elapsedTime = m_timer.Elapsed();
		m_timer.Reset();
	}

}