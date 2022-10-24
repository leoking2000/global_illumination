#pragma once
#include <glad/glad.h>
#include <assert.h>

namespace GL
{
	/* 
	*  Initialize glad and OpenGL
	*/
	void InitOpenGL();

	void glClearError(); // if we don't have valid opengl contex it will loop forever.
	void glCheckError(const char* file, int lin); // if we don't have valid opengl contex it will loop forever.
}


#ifdef _DEBUG
#define glCall(x) GL::glClearError(); x; GL::glCheckError(__FILE__, __LINE__)
#else
#define glCall(x) x
#endif