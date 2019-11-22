#pragma once


// Qt includes
#include <QtGui/QMouseEvent>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>


namespace imt3dgui
{


class CRulerShape: public CShape3dBase
{
public:
	CRulerShape();

	void SetSlaveShape(const CShape3dBase* slaveShapePtr);
	bool OnMousePress(QMouseEvent& e);
	bool OnMouseMove(QMouseEvent& e);

protected:
	// reimplemented (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;
	void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	ColorMode GetColorMode() const;

private:
	const CShape3dBase* m_slaveShapePtr;
	int m_movingVertexIndex;
	QPoint m_mousePrevPos;
	static const float s_moveSpeed;
};


} // namespace imt3dgui


