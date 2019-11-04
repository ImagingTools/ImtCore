#pragma once


// ImtCore includes
#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{


class CCrossTargetShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CCrossTargetShape();
	~CCrossTargetShape();

	void SetCrossBranchSize(double crossBranchSize);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	double m_crossBranchSize;
};


} // namespace imt3dgui


