#pragma once


// ImtCore includes
#include <imtgui3d/CShape3dBase.h>


namespace imtgui3d
{


class CTargetPointerShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CTargetPointerShape();
	~CTargetPointerShape();

	void SetFromPoint(const QVector3D& fromPoint);
	void SetToPoint(const QVector3D& toPoint);
	void SetLinesCount(int linesCount);

protected:
	// reimplement (imtgui3d::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imtgui3d::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	QVector3D m_fromPoint;
	QVector3D m_toPoint;
	int m_linesCount;
};


} // namespace imtgui3d


