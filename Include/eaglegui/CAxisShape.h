#pragma once


// eagle includes
#include <eaglegui/CShape3dBase.h>


namespace eaglegui
{


class CAxisShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CAxisShape();
	~CAxisShape();

	void SetAxisLength(double axisLength);

protected:
	// reimplement (eaglegui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (eaglegui::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplement (eaglegui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	QFont GetAxeLabelFont() const;

private:
	double m_axisLength;
	static const float s_zoomFontFactor;
};


} // namespace eaglegui


