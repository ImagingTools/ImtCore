#include <imt3dgui/CRulerShape.h>


#include <imt3dgui/CPointCloudShape.h>
#include <imod/IModel.h>


namespace imt3dgui
{


// static members

const float CRulerShape::s_moveSpeed = 0.03;


// public methods

CRulerShape::CRulerShape()
	:m_slaveShapePtr(nullptr),
	m_movingVertexIndex(-1)
{
	m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
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
			int rulerVertexIndex = FindVertex(e.pos(), false);

			if (slaveShapeVertexIndex >= 0 && rulerVertexIndex >= 0){
				m_vertices[rulerVertexIndex].position = slaveShapeVertexPos * m_slaveShapePtr->GetScale();

				UploadGeometry(false, m_vertices, m_vertexBuffer);
			}
		}

		return true;
	}

	// on single click save clicked ruler vertex for further moving
	m_movingVertexIndex = FindVertex(e.pos(), true);
	return m_movingVertexIndex >= 0;
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
		QVector3D posDiff = newPos - oldPos;

		m_vertices[m_movingVertexIndex].position += (posDiff * s_moveSpeed);

		UploadGeometry(false, m_vertices, m_vertexBuffer);

		retVal = true;
	}

	m_mousePrevPos = e.pos();

	return retVal;
}


// protected methods

// reimplemented (imt3dgui::CShape3dBase)

void CRulerShape::UpdateShapeGeometry()
{
	m_vertices.clear();
	m_vertices.push_back(Vertex(QVector3D(0.0, 0.0, 1.0), QVector3D(), QVector3D(1.5, 0.4, 0.0)));
	m_vertices.push_back(Vertex(QVector3D(1.0, 0.0, 1.0), QVector3D(), QVector3D(1.5, 0.4, 0.0)));

	m_indices = {0, 1};
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CRulerShape::DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions)
{
	program.setUniformValue("usePointSize", true);
	program.setUniformValue("pointSize", 5.0f);

	functions.glLineWidth(3.0f);
	functions.glDrawElements(GL_LINES, m_vertices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
	functions.glDrawElements(GL_POINTS, m_vertices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
}


void CRulerShape::Draw(QPainter& painter)
{
	if (!IsVisible() || !m_slaveShapePtr){
		return;
	}

	QVector3D pos3d1 = m_vertices[0].position / m_slaveShapePtr->GetScale();
	QVector3D pos3d2 = m_vertices[1].position / m_slaveShapePtr->GetScale();
	float distance = qAbs(pos3d1.distanceToPoint(pos3d2));

	QPoint pos2d1 = ModelToWindow(m_vertices[0].position);
	QPoint pos2d2 = ModelToWindow(m_vertices[1].position);
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


IShape3d::ColorMode CRulerShape::GetColorMode() const
{
	return ColorMode::CM_POINT;
}


} // namespace imt3dgui


