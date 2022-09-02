#include "Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
    // texture
    //assigns the type of texture
    type = texType;
    this->format = format;
    this->pixelType = pixelType;
    this->slot = slot;
    arrTexture = NULL;
    bytesTexture = NULL;

    //image reading
    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    //generating an OpenGL texture object
    glGenTextures(1, &id);
    //Assigning the texture to a Texture Unit
    glActiveTexture(slot);
    glBindTexture(type, id);

    //algorithm that makes the image smaller or bigger
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //config of the way the texture will reapeat
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //This is in case GL_CLAMP_TO_BORDER will be used
    //float flatColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //glTexParameterfv(type, GL_TEXTURE_BORDER_COLOR, flatColor);

    //assigns the read image to the object
    glTexImage2D(type, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
    //generates a mipmap
    glGenerateMipmap(type);

    //freeing memory and unbinding the texture
    stbi_image_free(bytes);
    glBindTexture(type, 0);
    
}

Texture::Texture(GLuint height, GLuint width, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
    //assigns the type of texture
    type = texType;
    this->format = format;
    this->pixelType = pixelType;
    this->slot = slot;

    //setting the pixelT array
    arrTexture = (pixelT *) malloc(sizeof(pixelT) * height * width);
    cleanArrTexture(height, width);

    bytesTexture = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 4);

    if (bytesTexture == NULL) {
        std::cout << "Error trying to allocate memory to bytes for Texture: " << id << std::endl;
        return;
    }
    else {
        serializeArrTexture(height, width);
    }

    //generating an OpenGL texture object
    glGenTextures(1, &id);
    //Assigning the texture to a Texture Unit
    glActiveTexture(slot);
    glBindTexture(type, id);

    //algorithm that makes the image smaller or bigger
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //config of the way the texture will reapeat
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //assigns the read image to the object
    glTexImage2D(type, 0, GL_RGBA, width, height, 0, format, pixelType, bytesTexture);
    //generates a mipmap
    glGenerateMipmap(type);

    //freeing memory and unbinding the texture
    glBindTexture(type, 0);

}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint texUni = glGetUniformLocation(shader.id, uniform);
    //shader needs to be active to change the value of the uniform
    shader.activate();
    glUniform1i(texUni, unit);
}

void Texture::bind() {
    glBindTexture(type, id);
}

void Texture::unbind() {
    glBindTexture(type, 0);
}

void Texture::deleteTexture() {
    glDeleteTextures(1, &id);
}

void Texture::refreshTexture(GLuint height, GLuint width) {

    serializeArrTexture(height, width);

    //Assigning the texture to a Texture Unit
    glActiveTexture(slot);
    glBindTexture(type, id);

    //algorithm that makes the image smaller or bigger
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //config of the way the texture will reapeat
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //assigns the read image to the object
    glTexImage2D(type, 0, GL_RGBA, width, height, 0, format, pixelType, bytesTexture);
    //generates a mipmap
    glGenerateMipmap(type);

    //freeing memory and unbinding the texture
    glBindTexture(type, 0);
}




void Texture::cleanArrTexture(GLuint height, GLuint width) {
    unsigned int i;
    if (arrTexture == NULL) {
        std::cout << "Array Texture must be initialize for Texture: " << id << std::endl;
        return;
    }
    for (i = 0; i < height * width; i++) {
        arrTexture[i].a = 255;
        arrTexture[i].r = 255;
        arrTexture[i].g = 255;
        arrTexture[i].b = 255;
    }
}

void Texture::serializeArrTexture(GLuint height, GLuint width) {
    unsigned int i;
    if (arrTexture == NULL) {
        std::cout << "Array Texture must be initialize for Texture: " << id << std::endl;
        return;
    }

    if (bytesTexture == NULL) {
        std::cout << "BytesTexture must be initialize for Texture: " << id << std::endl;
        return ;
    }

    for (i = 0; i < height * width * 4; i+=4) {
        bytesTexture[i] = (unsigned char) arrTexture[i / 4].r;
        bytesTexture[i+1] = (unsigned char) arrTexture[i / 4].g;
        bytesTexture[i+2] = (unsigned char) arrTexture[i / 4].b;
        bytesTexture[i+3] = (unsigned char) arrTexture[i / 4].a;
    }

    return ;
}