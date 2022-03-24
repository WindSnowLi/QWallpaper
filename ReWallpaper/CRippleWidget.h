#pragma once

#include <array>
#include <memory>
#include "QExpandOpenGLWidget.h"
class CRippleWidget : public QExpandOpenGLWidget
{
	Q_OBJECT
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	std::array<float, 20> vertices;
	std::array<GLuint, 6> indices;
	unsigned int imgVBO = 0, imgVAO = 0, imgEBO = 0;
	std::shared_ptr<Shader> imgShader = nullptr, renderShader = nullptr, dropShader = nullptr;
	std::tuple<GLuint, GLuint> fb1, fb2;
	unsigned int img;
	void render();
	void drop();
public:
	explicit CRippleWidget(QWidget* parent = nullptr);
	~CRippleWidget();

};

