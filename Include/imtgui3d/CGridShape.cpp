#include <imtgui3d/CGridShape.h>


namespace imtgui3d
{


// public methods

CGridShape::CGridShape()
	:m_gridValue(0.0),
	m_count(0)
{
}


CGridShape::~CGridShape()
{
}


void CGridShape::SetGridValue(double gridValue)
{
	Q_ASSERT(gridValue > 0.0);

	m_gridValue = gridValue;
}


void CGridShape::SetCount(int count)
{
	Q_ASSERT(count > 0);

	m_count = count;
}


// protected methods

// reimplement (imtgui3d::CShape3dBase)

void CGridShape::UpdateShapeGeometry()
{
	m_vertices.clear();
	m_vertices.reserve(m_count * 2 + 1);

	for(int i = -m_count; i <= m_count; ++i){
		m_vertices.push_back(Vertex(QVector3D(m_gridValue * i, -m_gridValue * m_count, 0.0)));
		m_vertices.push_back(Vertex(QVector3D(m_gridValue * i, m_gridValue * m_count, 0.0)));
	}
	for(int i = -m_count; i <= m_count; ++i){
		m_vertices.push_back(Vertex(QVector3D(-m_gridValue * m_count, m_gridValue * i, 0.0)));
		m_vertices.push_back(Vertex(QVector3D(m_gridValue * m_count, m_gridValue * i, 0.0)));
	}

	m_indices.clear();
	m_indices.reserve(m_vertices.size());

	for(int i = 0; i < m_vertices.count(); ++i){
		m_indices.push_back(i);
	}
}


// protected methods

// reimplement (imtgui3d::CShape3dBase)

void CGridShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


} // namespace imtgui3d


