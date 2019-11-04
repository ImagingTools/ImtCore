#pragma once


// ImtCore includes
#include <imtgui3d/CShape3dBase.h>


namespace imtgui3d
{


class CCrossTargetShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CCrossTargetShape();
	~CCrossTargetShape();

	void SetCrossBranchSize(double crossBranchSize);

protected:
	// reimplement (imtgui3d::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imtgui3d::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	double m_crossBranchSize;
};


} // namespace imtgui3d


