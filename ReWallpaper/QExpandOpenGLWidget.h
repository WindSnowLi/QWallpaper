#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <QFile>

#define MAX_BONE_INFLUENCE 4

class QExpandOpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
public:
	class Shader;
	struct Vertex;
	struct Texture;
public:
	QExpandOpenGLWidget(QWidget* parent = nullptr) :QOpenGLWidget(parent) {}

	void resizeGL(int w, int h);

	static unsigned int TextureFromFile(const char* path, const std::string& directory, QExpandOpenGLWidget* m_parent);

	unsigned int loadTexture(const char* path);
};

class QExpandOpenGLWidget::Shader {
public:
	unsigned int shaderProgram{};
	QExpandOpenGLWidget* m_parent = nullptr;
	Shader(QExpandOpenGLWidget* parent) : m_parent(parent) {}

	// 读取shader配置文件
	void readShaderFile(const char* vertexPath, const char* fragmentPath);

	// 读取shader配置字符串
	void readShaderStr(const char* vertexStr, const char* fragmentStr);

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type);

	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value);

	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value);

	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value);

	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value);

	void setVec2(const std::string& name, float x, float y);

	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value);

	void setVec3(const std::string& name, float x, float y, float z);

	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value);

	void setVec4(const std::string& name, float x, float y, float z, float w);

	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat);

	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat);

	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat);
};

struct QExpandOpenGLWidget::Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct QExpandOpenGLWidget::Texture {
	unsigned int id;
	std::string type;
	std::string path;
};