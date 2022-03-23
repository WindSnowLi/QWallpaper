#include "QExpandOpenGLWidget.h"

unsigned int QExpandOpenGLWidget::TextureFromFile(const char* path, const std::string& directory, QExpandOpenGLWidget* parent) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	parent->glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format{};
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		parent->glBindTexture(GL_TEXTURE_2D, textureID);
		parent->glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		parent->glGenerateMipmap(GL_TEXTURE_2D);

		parent->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		parent->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		parent->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		parent->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void QExpandOpenGLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
}



std::tuple<GLuint, int, int, GLenum> QExpandOpenGLWidget::loadTexture(char const* path)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	GLenum format{};
	if (data)
	{
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return { textureID, width, height, format };
}

unsigned int QExpandOpenGLWidget::loadFramebufferTexture2D(GLuint texture, int w, int h, GLenum type)
{
	unsigned int fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//this setting let ripple bound from edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, w, h, 0, type, GL_FLOAT, NULL);
	//here we use type GL_FLOAT to save the water height and velocity

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	return fb;
}

void QExpandOpenGLWidget::Shader::readShaderFile(const char* vertexPath, const char* fragmentPath) {

	QFile file(vertexPath);
	QString vertexStr, fragmentStr;
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray array = file.readAll();//读取文本中全部文件
		vertexStr = QString(array);
	}
	file.close();
	file.setFileName(fragmentPath);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray array = file.readAll();//读取文本中全部文件
		fragmentStr = QString(array);
	}
	file.close();
	readShaderStr(vertexStr.toStdString().c_str(), fragmentStr.toStdString().c_str());
}

void QExpandOpenGLWidget::Shader::readShaderStr(const char* vertexStr, const char* fragmentStr) {
	GLuint vertexShader = m_parent->glCreateShader(GL_VERTEX_SHADER);
	m_parent->glShaderSource(vertexShader, 1, &vertexStr, NULL);
	m_parent->glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	m_parent->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		m_parent->glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	}
	GLuint fragmentShader = m_parent->glCreateShader(GL_FRAGMENT_SHADER);

	m_parent->glShaderSource(fragmentShader, 1, &fragmentStr, NULL);
	m_parent->glCompileShader(fragmentShader);

	m_parent->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		m_parent->glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
	}
	shaderProgram = m_parent->glCreateProgram();
	m_parent->glAttachShader(shaderProgram, vertexShader);
	m_parent->glAttachShader(shaderProgram, fragmentShader);
	m_parent->glLinkProgram(shaderProgram);

	m_parent->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		m_parent->glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
	}
	m_parent->glDeleteShader(vertexShader);
	m_parent->glDeleteShader(fragmentShader);

}

void QExpandOpenGLWidget::Shader::checkCompileErrors(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		m_parent->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)

		{
			m_parent->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		m_parent->glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			m_parent->glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void QExpandOpenGLWidget::Shader::use()
{
	m_parent->glUseProgram(shaderProgram);
}

void QExpandOpenGLWidget::Shader::setBool(const std::string& name, bool value)
{
	m_parent->glUniform1i(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void QExpandOpenGLWidget::Shader::setInt(const std::string& name, int value)
{
	m_parent->glUniform1i(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), value);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setFloat(const std::string& name, float value)
{
	m_parent->glUniform1f(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), value);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setVec2(const std::string& name, const glm::vec2& value)
{
	m_parent->glUniform2fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void QExpandOpenGLWidget::Shader::setVec2(const std::string& name, float x, float y)
{
	m_parent->glUniform2f(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	m_parent->glUniform3fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void QExpandOpenGLWidget::Shader::setVec3(const std::string& name, float x, float y, float z)
{
	m_parent->glUniform3f(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setVec4(const std::string& name, const glm::vec4& value)
{
	m_parent->glUniform4fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void QExpandOpenGLWidget::Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	m_parent->glUniform4f(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setMat2(const std::string& name, const glm::mat2& mat)
{
	m_parent->glUniformMatrix2fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setMat3(const std::string& name, const glm::mat3& mat)
{
	m_parent->glUniformMatrix3fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void QExpandOpenGLWidget::Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
	m_parent->glUniformMatrix4fv(m_parent->glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}