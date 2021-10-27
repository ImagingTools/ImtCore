#pragma once


// Qt includes
#include <QtGui/QMouseEvent>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


class CRulerShape: public CShape3dBase
{
public:
	CRulerShape();

	void SetSlaveShape(const CShape3dBase* slaveShapePtr);
	bool OnMousePress(QMouseEvent& e);
	bool OnMouseMove(QMouseEvent& e);
	bool OnMouseRelease(QMouseEvent& e);

protected:
	// reimplemented (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;
	void Draw(QPainter& painter) override;

private:
	const CShape3dBase* m_slaveShapePtr;
	int m_movingVertexIndex;
	QPoint m_mousePrevPos;
	static const float s_moveSpeed;
	static const QVector3D s_color;
	static const QVector3D s_selectionColor;
	imt3d::IPointsBasedObject::PointXyzwRgba32* m_vertices;
	imt3d::CPointCloud3d m_data;
};


} // namespace imt3dgui


