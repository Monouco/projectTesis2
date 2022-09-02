#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &id);
}

void VAO::linkAttrib(VBO& vboBuff, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	vboBuff.bind();
	//Specify how to use the VBO, this helps to communicate the shader from the outside
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	//Enable the vertex attribute so that OpenGL knows how to use it
	glEnableVertexAttribArray(layout);
	vboBuff.unbind();
}

void VAO::bind() {
	glBindVertexArray(id);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::deleteVAO() {
	glDeleteVertexArrays(1, &id);
}