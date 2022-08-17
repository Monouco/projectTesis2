#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaders/shaderClass.h"

//Lets define the structure that will represent each pixel that we'll be changing
typedef struct pixelT{
	GLuint r;
	GLuint g;
	GLuint b;
	GLuint a;
} pixelT;

//Defining another structure that will help at the time of getting an object

typedef struct textureT {
	GLuint height;
	GLuint width;
	GLuint numChannels;
	void* pixels;
} textureT;

class Texture
{
	public:
		GLuint id;
		GLenum type;
		GLenum format;
		GLenum pixelType;
		GLenum slot;
		pixelT* arrTexture;
		int* arrBuffer;
		unsigned char* bytesTexture;

		Texture(const char * image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
		Texture(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

		void texUnit(Shader& shader, const char* uniform, GLuint unit);
		void bind();
		void unbind();
		void deleteTexture();

		void refreshTexture(GLuint height, GLuint width);
		void cleanArrTexture(GLuint height, GLuint width);

	private:
		void serializeArrTexture(GLuint height, GLuint width);
};

#endif // !TEXTURE_CLASS_H
