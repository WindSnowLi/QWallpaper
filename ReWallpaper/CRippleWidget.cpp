#include "CRippleWidget.h"
#include "tool.h"

void CRippleWidget::initializeGL()
{
	this->initializeOpenGLFunctions();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	imgShader = std::make_shared<Shader>(this);
	imgShader->readShaderFile(":/glsl/Resources/glsl/draw.vert", ":/glsl/Resources/glsl/draw.frag");

	glGenVertexArrays(1, &imgVAO);
	glGenBuffers(1, &imgVBO);
	glGenBuffers(1, &imgEBO);

	glBindVertexArray(imgVAO);

	glBindBuffer(GL_ARRAY_BUFFER, imgVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	auto imgPath = tool::GetSystemWallpaperPath().toStdString();
	imgShader->use();
	imgTexture = loadTexture(imgPath.c_str());

	imgShader->setInt("img", 0);
	glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::scale(transform, glm::vec3(2.0, 2.0, 2.0));
	imgShader->setMat4("transform", transform);
}

void CRippleWidget::resizeGL(int w, int h)
{
	QExpandOpenGLWidget::resizeGL(w, h);
}

void CRippleWidget::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	// bind Texture
	glBindTexture(GL_TEXTURE_2D, imgTexture);
	// render container
	imgShader->use();
	glBindVertexArray(imgVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

CRippleWidget::CRippleWidget(QWidget* parent) :QExpandOpenGLWidget(parent)
{
	vertices = std::array<float, 20>(
		{
			//---- 位置 ----   - 纹理坐标 -
			0.5f,  0.5f, 0.0f,	1.0f, 1.0f,   // 右上
			0.5f, -0.5f, 0.0f,	1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,	0.0f, 1.0f    // 左上
		}
	);
	indices = std::array<GLuint, 6>(
		{
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
		}
	);
}

CRippleWidget::~CRippleWidget()
{
}
