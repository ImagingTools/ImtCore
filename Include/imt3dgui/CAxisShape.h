#pragma once


// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{


class CAxisShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CAxisShape();
	~CAxisShape();

	void SetAxisLength(double axisLength);
	void SetAxisRange(const istd::CRange& range);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplement (imt3dgui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	QFont GetAxeLabelFont() const;

private:
	double m_axisLength;
	istd::CRange m_axisRange;
	static const float s_zoomFontFactor;
};


} // namespace imt3dgui


