#include <imtgui3d/CAxisShape.h>


namespace imtgui3d
{


// static members
const float CAxisShape::s_zoomFontFactor = 50.0f;


// public methods

CAxisShape::CAxisShape()
	:m_axisLength(0.0)
{
}


CAxisShape::~CAxisShape()
{
}


void CAxisShape::SetAxisLength(double axisLength)
{
	Q_ASSERT(axisLength > 0.0);

	m_axisLength = axisLength;
}


// protected methods

// reimplement (imtgui3d::CShape3dBase)

void CAxisShape::UpdateShapeGeometry()
{
	m_vertices.clear();
	m_vertices.resize(6);

	// x (red)
	m_vertices[0] = Vertex(QVector3D(0.0, 0.0, 0.0), QVector3D(), QVector3D(1.0, 0.0, 0.0));
	m_vertices[1] = Vertex(QVector3D(m_axisLength, 0.0, 0.0), QVector3D(), QVector3D(1.0, 0.0, 0.0));

	// y (green)
	m_vertices[2] = Vertex(QVector3D(0.0, 0.0, 0.0), QVector3D(), QVector3D(0.0, 1.0, 0.0));
	m_vertices[3] = Vertex(QVector3D(0.0, m_axisLength, 0.0), QVector3D(), QVector3D(0.0, 1.0, 0.0));

	// z (blue)
	m_vertices[4] = Vertex(QVector3D(0.0, 0.0, 0.0), QVector3D(), QVector3D(0.0, 0.0, 1.0));
	m_vertices[5] = Vertex(QVector3D(0.0, 0.0, m_axisLength), QVector3D(), QVector3D(0.0, 0.0, 1.0));
	
	m_indices = { 0, 1, 2, 3, 4, 5 };
}


// protected methods

// reimplement (imtgui3d::CShape3dBase)

void CAxisShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glLineWidth(2.0f);
	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


// reimplement (imtgui3d::IDrawable)

void CAxisShape::Draw(QPainter& painter)
{
	if (!IsVisible()){
		return;
	}

	painter.setPen(Qt::black);
	painter.setFont(GetAxeLabelFont());

	QPoint windowCoordinate = ModelToWindow(QVector3D(m_axisLength, 0.0, 0.0), painter.viewport());
	painter.drawText(windowCoordinate, "X");

	windowCoordinate = ModelToWindow(QVector3D(0.0, m_axisLength, 0.0), painter.viewport());
	painter.drawText(windowCoordinate, "Y");

	windowCoordinate = ModelToWindow(QVector3D(0.0, 0.0, m_axisLength), painter.viewport());
	painter.drawText(windowCoordinate, "Z");
}


// private methods

QFont CAxisShape::GetAxeLabelFont() const
{
	if (!m_cameraPtr){
		return QFont();
	}

	float fontSize = qAbs(s_zoomFontFactor / m_cameraPtr->GetPosition().z());
	fontSize = qMin(fontSize, 10.0f);

	QFont font("Arial");
	font.setPointSizeF(fontSize);
	font.setBold(true);
	return font;
}


} // namespace imtgui3d


