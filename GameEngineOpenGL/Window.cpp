#include "Window.h"
#include "ErrManager.h"


namespace ge {

	Window::Window() { /* empty */ }

	Window::~Window() { /* empty */ }

	int Window::create(std::string windowTitle, int width, int height, WindowFlags windowFlags)
	{
		m_width = width;
		m_height = height;

		// assigning all flags (if any)
		Uint32 flags = SDL_WINDOW_OPENGL; /// default window flag
		if (windowFlags & WINDOW_SHOWN) {
			flags |= SDL_WINDOW_SHOWN;
		}
		if (windowFlags & WINDOW_HIDDEN) {
			flags |= SDL_WINDOW_HIDDEN;
		}
		if (windowFlags & WINDOW_FULLSCREEN) {
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (windowFlags & WINDOW_BORDERLESS) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		/// Creating the main window
		m_sdlWindow = SDL_CreateWindow(windowTitle.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			(int)m_width,
			(int)m_height,
			flags);
		if (nullptr == m_sdlWindow) {
			fatalError("BallGameMainGame: SDL Window could not be created!");
		}

		// Creating SDL Context and sending it to the main window
		SDL_GLContext glContext = SDL_GL_CreateContext(m_sdlWindow);
		if (nullptr == glContext) {
			fatalError("BallGameMainGame: SDL_GL context could not be created!");
		}

		// Initializing Glue (does hardware check etc.)
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			fatalError("BallGameMainGame: Could not initialize Glew!");
		}

		std::printf("***	OpenGL version %s	***\n", glGetString(GL_VERSION));

		// Setting the screen background color
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// setting VSYNC, 0 = off, 1 = on
		SDL_GL_SetSwapInterval(0);

		// enable transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void Window::swapBuffer()
	{
		// Flushing current screen
		SDL_GL_SwapWindow(m_sdlWindow);
	}

}