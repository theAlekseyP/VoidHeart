#include <hollowpch.h>

#include "GameWindow.h"

namespace Hollow {
	GameWindow::GameWindow(std::string title, int width, int height) : mTitle(title), mWidth(width), mHeight(height)
	{
		Initialize();
	}

	GameWindow::~GameWindow()
	{
		SDL_GL_DeleteContext(mContext);
		SDL_DestroyWindow(mpWindow);
		SDL_Quit();
	}

	void GameWindow::Initialize()
	{
		HW_CORE_INFO("Creating SDL window");
		static bool initSDL = true;
		// Init SDL if it needs to be
		if (initSDL)
		{
			HW_CORE_INFO("Initalizing SDL");
			SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
			initSDL = false;
		}

		//SDL Opengl settings
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Enable AntiAliasiing (Don't know what does this do)
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Create SDL window
		mpWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, SDL_WINDOW_OPENGL);

		if (mpWindow == NULL)
		{
			HW_CORE_ERROR("Could not create window: {0}", SDL_GetError());
		}

		// Create OpenGL context
		mContext = SDL_GL_CreateContext(mpWindow);
		
		// Set this flag if Vysnc is wanted
		SDL_GL_SetSwapInterval(0);
	}
}
