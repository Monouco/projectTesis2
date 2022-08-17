#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "level/VBO.h"

class VAO {
public:
	GLuint id;
	VAO();

	void linkAttrib(VBO& vboBuff, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void bind();
	void unbind();
	void deleteVAO();
};

#endif // !VAO_CLASS_H

