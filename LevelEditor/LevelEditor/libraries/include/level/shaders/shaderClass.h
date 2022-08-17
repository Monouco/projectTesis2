#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#define LOGSIZE 1024

std::string get_file_contents(const char* filename);

class Shader {
	public:
		GLuint id;
		Shader(const char* vertexFile, const char* fragmentFile);
		void activate();
		void deleteShader();

	private:
		void compileErrors(unsigned int shader, const char* type);
};


#endif // !SHADER_CLASS_H
