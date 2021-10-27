#include <imt3dgui/CRulerShape.h>


// ImtCore includes
#include <imt3dgui/CPointCloudShape.h>


namespace imt3dgui
{


// static members

const float CRulerShape::s_moveSpeed = 0.03;
const QVector3D CRulerShape::s_color(1.5, 0.4, 0.0);
const QVector3D CRulerShape::s_selectionColor(0.0, 0.0, 1.0);


// public methods

CRulerShape::CRulerShape()
	:m_slaveShapePtr(nullptr),
	m_movingVertexIndex(-1)
{
	m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);

	std::vector<imt3d::IPointsBasedObject::PointXyzwRgba32> vertices;
	// line vertices
	imt3d::IPointsBasedObject::PointXyzwRgba32 a;
	a.data[2] = 1;
	a.data[4] = s_color.x();
	a.data[5] = s_color.y();
	a.data[6] = s_color.z();
	vertices.push_back(a);

	imt3d::IPointsBasedObject::PointXyzwRgba32 b = a;
	b.data[0] = 1;
	vertices.push_back(b);

	// point vertices
	vertices.push_back(vertices[0]);
	vertices.push_back(vertices[1]);

	m_data.CreateCloud(imt3d::CPointCloud3d::PF_XYZW_RGBA_32, vertices.size(), vertices.data());
	m_vertices = static_cast<imt3d::IPointsBasedObject::PointXyzwRgba32*>(m_data.GetData());

	m_indices = { 0, 1, 2, 3 };
	m_pointsDataPtr = &m_data;
}


void CRulerShape::SetSlaveShape(const CShape3dBase* slaveShapePtr)
{
	m_slaveShapePtr = slaveShapePtr;
}


bool CRulerShape::OnMousePress(QMouseEvent& e)
{
	if (!IsVisible()){
		return false;
	}

	if (e.type() == QEvent::MouseButtonDblClick){
		// on double click stick closest ruler vertex to the pointed slave shape vertex
		if (m_slaveShapePtr){
			// determine the pointed slave shape vertex
			QVector3D slaveShapeVertexPos;
			int slaveShapeVertexIndex = m_slaveShapePtr->FindVertex(e.pos(), false, &slaveShapeVertexPos);

			// determine the ruler vertex closest to the click point
			int rulerVertexIndex = FindVertex(e.pos(), false) % 2;

			if (slaveShapeVertexIndex >= 0 && rulerVertexIndex >= 0){
				QVector3D newPos = slaveShapeVertexPos * m_slaveShapePtr->GetScale();

				m_vertices[rulerVertexIndex + 0].data[0] = newPos.x();
				m_vertices[rulerVertexIndex + 0].data[1] = newPos.y();
				m_vertices[rulerVertexIndex + 0].data[2] = newPos.z();

				m_vertices[rulerVertexIndex + 2].data[0] = newPos.x();
				m_vertices[rulerVertexIndex + 2].data[1] = newPos.y();
				m_vertices[rulerVertexIndex + 2].data[2] = newPos.z();

				RefreshGeometry();
			}
		}

		return true;
	}

	// on single click, save clicked ruler vertex for further moving
	m_movingVertexIndex = FindVertex(e.pos(), true) % 2;

	if (m_movingVertexIndex >= 0){
		m_vertices[m_movingVertexIndex + 2].data[4] = s_selectionColor.x();
		m_vertices[m_movingVertexIndex + 2].data[5] = s_selectionColor.y();
		m_vertices[m_movingVertexIndex + 2].data[6] = s_selectionColor.z();

		RefreshGeometry();

		return true;
	}

	return false;
}


bool CRulerShape::OnMouseMove(QMouseEvent& e)
{
	if (!IsVisible()){
		return false;
	}

	bool retVal = false;

	// calculate and update new vertex position
	if (m_movingVertexIndex >= 0 && e.buttons() == Qt::LeftButton){
		QVector3D oldPos = WindowToModel(m_mousePrevPos, 1.0);
		QVector3D newPos = WindowToModel(e.pos(), 1.0);
		QVector3D posDiff = (newPos - oldPos) * s_moveSpeed;

		m_vertices[m_movingVertexIndex + 0].data[0] += posDiff.x();
		m_vertices[m_movingVertexIndex + 0].data[1] += posDiff.y();
		m_vertices[m_movingVertexIndex + 0].data[2] += posDiff.z();

		m_vertices[m_movingVertexIndex + 2].data[0] += posDiff.x();
		m_vertices[m_movingVertexIndex + 2].data[1] += posDiff.y();
		m_vertices[m_movingVertexIndex + 2].data[2] += posDiff.z();

		RefreshGeometry();

		retVal = true;
	}

	m_mousePrevPos = e.pos();

	return retVal;
}


bool CRulerShape::OnMouseRelease(QMouseEvent& /*e*/)
{
	if (m_movingVertexIndex >= 0){
		m_vertices[m_movingVertexIndex + 2].data[4] = s_color.x();
		m_vertices[m_movingVertexIndex + 2].data[5] = s_color.y();
		m_vertices[m_movingVertexIndex + 2].data[6] = s_color.z();

		m_movingVertexIndex = -1;

		RefreshGeometry();

		return true;
	}

	return false;

}


// protected methods

// reimplemented (imt3dgui::CShape3dBase)

void CRulerShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CRulerShape::DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions)
{
	// draw line
	GLuint* offsetPtr = (GLuint*)0;

	functions.glLineWidth(4.0f);
	functions.glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, offsetPtr);

	// draw points
	program.setUniformValue("usePointSize", true);
	program.setUniformValue("pointSize", 12.0f);

	functions.glDrawElements(GL_POINTS, 2, GL_UNSIGNED_INT, offsetPtr + 2);
}


void CRulerShape::Draw(QPainter& painter)
{
	if (!IsVisible() || !m_slaveShapePtr){
		return;
	}

	QVector3D posA(m_vertices[0].data[0], m_vertices[0].data[1], m_vertices[0].data[2]);
	QVector3D posB(m_vertices[1].data[0], m_vertices[1].data[1], m_vertices[1].data[2]);

	QVector3D pos3d1 = posA / m_slaveShapePtr->GetScale();
	QVector3D pos3d2 = posB / m_slaveShapePtr->GetScale();

	float distance = qAbs(pos3d1.distanceToPoint(pos3d2));

	QPoint pos2d1 = ModelToWindow(posA);
	QPoint pos2d2 = ModelToWindow(posB);
	QPoint pos2dDistance = (pos2d1 + pos2d2) / 2;

	QString point1Text = QString("(%1,%2,%3)")
				.arg(pos3d1.x(), 0, 'f', 3)
				.arg(pos3d1.y(), 0, 'f', 3)
				.arg(pos3d1.z(), 0, 'f', 3);
	QString point2Text = QString("(%1,%2,%3)")
				.arg(pos3d2.x(), 0, 'f', 3)
				.arg(pos3d2.y(), 0, 'f', 3)
				.arg(pos3d2.z(), 0, 'f', 3);
	QString distanceText = QString("%1").arg(distance, 0, 'f', 3);

	static QFont font("Arial", 8);
	font.setBold(true);

	painter.save();
	painter.setPen(QColor(0, 0, 0, 128));
	painter.setFont(font);
	painter.drawText(pos2d1, point1Text);
	painter.drawText(pos2d2, point2Text);
	painter.drawText(pos2dDistance, distanceText);
	painter.restore();
}


} // namespace imt3dgui


