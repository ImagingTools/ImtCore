// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;
	virtual void Draw(QPainter& painter) override;

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


