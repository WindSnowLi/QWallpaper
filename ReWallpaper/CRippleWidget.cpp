#include "CRippleWidget.h"
#include "tool.h"

void CRippleWidget::initializeGL()
{
	this->initializeOpenGLFunctions();
	imgShader = std::make_shared<Shader>(this);
	renderShader = std::make_shared<Shader>(this);
	dropShader = std::make_shared<Shader>(this);
	imgShader->readShaderFile(":/glsl/Resources/glsl/draw.vert", ":/glsl/Resources/glsl/draw.frag");
	renderShader->readShaderFile(":/glsl/Resources/glsl/render.vert", ":/glsl/Resources/glsl/render.frag");
	dropShader->readShaderFile(":/glsl/Resources/glsl/drop.vert", ":/glsl/Resources/glsl/drop.frag");
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
	auto imgTex = loadTexture(imgPath.c_str());
	img = std::get<0>(imgTex);
	fb1 = loadFramebufferTexture2D(this->width(), this->height(), std::get<3>(imgTex));
	fb2 = loadFramebufferTexture2D(this->width(), this->height(), std::get<3>(imgTex));

	glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::scale(transform, glm::vec3(2.0, 2.0, 2.0));

	imgShader->setInt("img", 0);
	imgShader->setInt("ripple", 1);
	imgShader->setMat4("transform", transform);

	renderShader->use();
	renderShader->setInt("currentRipple", 0);
	renderShader->setInt("ripple", 1);

	dropShader->use();
	dropShader->setInt("ripple", 0);
	dropShader->setMat4("transform", transform);
}

void CRippleWidget::resizeGL(int w, int h)
{
	QExpandOpenGLWidget::resizeGL(w, h);
}

void CRippleWidget::paintGL()
{
	drop();

	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	imgShader->use();
	glBindVertexArray(imgVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, std::get<1>(fb1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, std::get<1>(fb1));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->update();
}

void CRippleWidget::render()
{
}

void CRippleWidget::drop()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, std::get<0>(fb1));
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	dropShader->use();
	glBindVertexArray(imgVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, img);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//auto ft = fb1;
	//fb1 = fb2;
	//fb2 = ft;
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
