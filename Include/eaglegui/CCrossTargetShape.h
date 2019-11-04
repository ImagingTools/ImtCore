#pragma once


// eagle includes
#include <eaglegui/CShape3dBase.h>


namespace eaglegui
{


class CCrossTargetShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CCrossTargetShape();
	~CCrossTargetShape();

	void SetCrossBranchSize(double crossBranchSize);

protected:
	// reimplement (eaglegui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (eaglegui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_POINT; }

private:
	double m_crossBranchSize;
};


} // namespace eaglegui


