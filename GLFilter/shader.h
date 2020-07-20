#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
public:
	// 程序ID
	unsigned int ID;
	Shader() {};
	// 构造器读取并构建着色器
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	// 使用/激活程序
	void use();
	// uniform工具函数
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, glm::mat4 value);
	void setVec3(const std::string &name, float x, float y, float z);
	void setVec3(const std::string &name, const glm::vec3 &value);
	void setVec2(const std::string &name, const glm::vec2 &value);
	void setVec4(const std::string &name, const glm::vec4 &value);
};

#endif

