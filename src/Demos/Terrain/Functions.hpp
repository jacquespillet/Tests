#include "Common.h"

#include "GL_Helpers/GL_Shader.hpp"
#include <GL/glew.h>
#include <fstream>
#include <sstream>


GLuint CreateTexture3D(std::vector<float> data, int size) {
    GLuint glTex;
    glGenTextures(1, &glTex);  
    glBindTexture(GL_TEXTURE_3D, glTex);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLint  texType = GL_RED;

    glTexImage3D(GL_TEXTURE_3D, 0, texType, size, size, size, 0, texType,  GL_FLOAT, (unsigned char *)data.data());
    // glGenerateMipmap(GL_TEXTURE_3D);
    
    glBindTexture(GL_TEXTURE_3D, 0);
    
    return glTex;
}

GLuint CreateIntTexture(uint8_t* data, int width, int height) {
    GLuint glTex;
    glGenTextures(1, &glTex);  
    glBindTexture(GL_TEXTURE_2D, glTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTexImage3D(GL_TEXTURE_2D, 0, GL_RED, widt, size, size, 0, texType,  GL_FLOAT, (unsigned char *)data.data());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    return glTex;
}

GL_Shader* CreateShader(std::string vert, std::string geom, std::string frag) {
    GL_Shader * shader= new GL_Shader;

    std::ifstream t(vert);
    std::stringstream vertBuffer;
    vertBuffer << t.rdbuf();
    shader->vertSrc= vertBuffer.str();

    t = std::ifstream (geom);
    std::stringstream geomBuffer;
    geomBuffer << t.rdbuf();
    shader->geometrySrc= geomBuffer.str();

    t = std::ifstream (frag);
    std::stringstream fragBuffer;
    fragBuffer << t.rdbuf();
    shader->fragSrc= fragBuffer.str();

    t.close();

    bool hasGeometry=shader->geometrySrc.size()>0;

	//make array to pointer for source code (needed for opengl )
	const char* vsrc[1];
	const char* fsrc[1];
	const char* gsrc[1];
	vsrc[0] = shader->vertSrc.c_str();
	fsrc[0] = shader->fragSrc.c_str();
	if(hasGeometry)gsrc[0] = shader->geometrySrc.c_str();
	
	//compile vertex and fragment shaders from source
	shader->vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader->vertexShaderObject, 1, vsrc, NULL);
	glCompileShader(shader->vertexShaderObject);
	shader->fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader->fragmentShaderObject, 1, fsrc, NULL);
	glCompileShader(shader->fragmentShaderObject);
	if(hasGeometry) {
		shader->geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shader->geometryShaderObject, 1, gsrc, NULL);
		glCompileShader(shader->geometryShaderObject); 
	}
	std::cout << "Shader:Compile: Linking shader" << std::endl;
	
	//link vertex and fragment shader to create shader program object
	shader->programShaderObject = glCreateProgram();
	glAttachShader(shader->programShaderObject, shader->vertexShaderObject);
	glAttachShader(shader->programShaderObject, shader->fragmentShaderObject);
    

	if(hasGeometry) glAttachShader(shader->programShaderObject, shader->geometryShaderObject);
    
	const GLchar* feedbackVaryings[] = { "outPosition", "outNormal" };
    glTransformFeedbackVaryings(shader->programShaderObject, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	
	glLinkProgram(shader->programShaderObject);
	std::cout << "Shader:Compile: checking shader status" << std::endl;
	
	//Check status of shader and log any compile time errors
	int linkStatus;
	glGetProgramiv(shader->programShaderObject, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) 
	{
		char log[5000];
		int lglen; 
		glGetProgramInfoLog(shader->programShaderObject, 5000, &lglen, log);
		std::cerr << "Shader:Compile: Could not link program: " << std::endl;
		std::cerr << log << std::endl;
		glGetShaderInfoLog(shader->vertexShaderObject, 5000, &lglen, log);
		std::cerr << "vertex shader log:\n" << log << std::endl;
		glGetShaderInfoLog(shader->fragmentShaderObject, 5000, &lglen, log);
		std::cerr << "fragment shader log:\n" << log << std::endl;
		glDeleteProgram(shader->programShaderObject);
		shader->programShaderObject = 0;
	}
	else
	{
		std::cout << "Shader:Compile: compile success " << std::endl;
		shader->compiled = true; 
	}


    return shader;
}

GLuint CreateFramebuffer3D(int size, GLuint* texHandle){
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
	
	// create a color attachment texture
	glGenTextures(1, texHandle);
	glBindTexture(GL_TEXTURE_3D, *texHandle);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, size, size,size, 0, GL_RED, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT ); 
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT ); 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *texHandle, 0);

	
	// now that we actually created the fbo and added all attachments we want to check if it is actually complete now
	GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if ( e != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "Framebuffer:Constructor: ERROR:: Framebuffer is not complete!" << std::endl;
	} else {
		std::cout << "Framebuffer:Constructor:  Framebuffer OK!"<<std::endl;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER,0);
	glBindTexture(GL_TEXTURE_3D, 0);

	return fbo;
}