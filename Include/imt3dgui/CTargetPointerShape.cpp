#include <imt3dgui/CTargetPointerShape.h>


namespace imt3dgui
{


// public methods

CTargetPointerShape::CTargetPointerShape()
	: m_linesCount(0)
{
}


CTargetPointerShape::~CTargetPointerShape()
{
}


void CTargetPointerShape::SetFromPoint(const QVector3D& fromPoint)
{
	m_fromPoint = fromPoint;

	UpdateShapeGeometry();
}


void CTargetPointerShape::SetToPoint(const QVector3D& toPoint)
{
	m_toPoint = toPoint;

	UpdateShapeGeometry();
}


void CTargetPointerShape::SetLinesCount(int linesCount)
{
	Q_ASSERT(linesCount > 0);

	m_linesCount = linesCount;

	UpdateShapeGeometry();
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CTargetPointerShape::UpdateShapeGeometry()
{
	if (m_linesCount <= 0){
		m_linesCount = 12;
	}
	else if ((m_linesCount % 2) == 0){
		++m_linesCount;
	}

	m_vertices.clear();
	m_indices.clear();

	QVector3D initialVector = m_toPoint - m_fromPoint;

	for (int i = 0; i < m_linesCount; ++i){
		m_vertices.push_back(Vertex(m_fromPoint + initialVector / m_linesCount * i, QVector3D(), QVector3D(0.54, 0.67, 0.69)));
		m_vertices.push_back(Vertex(m_fromPoint + initialVector / m_linesCount * (i + 1), QVector3D(), QVector3D(0.54, 0.67, 0.69)));

		m_indices.push_back(i);
		m_indices.push_back(i + 1);
		++i; // for 1 space item
	}
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CTargetPointerShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glLineWidth(3.0f);
	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


} // namespace imt3dgui


