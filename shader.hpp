#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <stdio.h>
#include <stdlib.h>

class Shader{
	public:
		unsigned int ID;

		// Constructor for shaders
		Shader(const char* vertexPath, const char* fragmentPath){
			char* vertexCode; 
			char* fragmentCode;
			FILE* vertexFile = fopen(vertexPath, "r");
			FILE* fragmentFile = fopen(fragmentPath, "r");

			if(vertexFile == NULL && fragmentFile == NULL){
				printf("Files does not exist!");
				exit(-1);
			}
			size_t newLen = fread(vertexCode, sizeof(char), sizeof(vertexCode), vertexFile);
			if(ferror(vertexFile) != 0){
				fputs("Error reading file!", stderr);
			}
			else{
				vertexCode[newLen++] = '\0';
			}

			newLen = fread(fragmentCode, sizeof(char), sizeof(fragmentCode), fragmentFile);
			if(ferror(fragmentFile) != 0){
				fputs("Error reading file!", stderr);
			}
			else{
				fragmentCode[newLen++] = '\0';
			}
			fclose(vertexFile);
			fclose(fragmentFile);

			unsigned int vertex, fragment;
			int success;
			char infoLog[512];

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vertexCode, NULL);
			glCompileShader(vertex);

			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if(!success){
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
			}

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fragmentCode, NULL);
			glCompileShader(fragment);

			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if(!success){
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
			}

			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);

			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if(!success){
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
			}

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		void use(){
			glUseProgram(ID);
		}

		void setBool(const char* name, bool value){
			glUniform1i(glGetUniformLocation(ID, name), (int)value);
		}

		void setInt(const char* name, int value){
			glUniform1i(glGetUniformLocation(ID, name), (int)value);
		}

		void setFloat(const char* name, float value){
			glUniform1f(glGetUniformLocation(ID, name), (float)value);
		}
};
#endif