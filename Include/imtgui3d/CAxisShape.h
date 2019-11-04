#pragma once


// ImtCore includes
#include <imtgui3d/CShape3dBase.h>


namespace imtgui3d
{


class CAxisShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CAxisShape();
	~CAxisShape();

	void SetAxisLength(double axisLength);

protected:
	// reimplement (imtgui3d::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imtgui3d::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplement (imtgui3d::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	QFont GetAxeLabelFont() const;

private:
	double m_axisLength;
	static const float s_zoomFontFactor;
};


} // namespace imtgui3d


