#pragma once


// ImtCore includes
#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{


class CGridShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CGridShape();
	~CGridShape();

	void SetGridValue(double gridValue);
	void SetCount(int count);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_ITEM; }
	QVector3D GetColor() const { return QVector3D(0.5, 0.5, 0.5); }

private:
	double m_gridValue;
	int m_count;
};


} // namespace imt3dgui


