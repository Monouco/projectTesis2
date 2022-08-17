#include "level/shaders/shaderClass.h"

std::string get_file_contents(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
	if (file) {
        std::string content;
		file.seekg(0, std::ios::end);
		content.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&content[0], content.size());
		file.close();
		return(content);
	}
    else {
        std::cout << "Error con el archivo " << filename << std::endl;
    }
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {

    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Buffers definition, vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // Buffers definition, fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    //Creating shader programs
    id = glCreateProgram();

    //Attaching shaders to the program
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    //Linking all the shaders into the program
    glLinkProgram(id);
    compileErrors(id, "PROGRAM");

    //Deleting shaders since they are already in the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::activate() {
    glUseProgram(id);
}

void Shader::deleteShader() {
    glDeleteProgram(id);
}

void Shader::compileErrors(unsigned int shader, const char* type) {
    GLint compiled;
    char infoLog[LOGSIZE];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE) {
            glGetShaderInfoLog(shader, LOGSIZE, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for: " << type << std::endl << std::endl;
        }
    }
    else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE) {
            glGetProgramInfoLog(shader, LOGSIZE, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for: " << type << std::endl << std::endl;
        }
    }
}