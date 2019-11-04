#include <imt3dgui/CCrossTargetShape.h>


#include <QtMath>


namespace imt3dgui
{


// public methods

CCrossTargetShape::CCrossTargetShape()
	:m_crossBranchSize(0.0)
{
}


CCrossTargetShape::~CCrossTargetShape()
{
}


void CCrossTargetShape::SetCrossBranchSize(double crossBranchSize)
{
	Q_ASSERT(crossBranchSize > 0.0);

	m_crossBranchSize = crossBranchSize;

	UpdateShapeGeometry();
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CCrossTargetShape::UpdateShapeGeometry()
{
	m_vertices.clear();
	m_indices.clear();

	m_vertices.push_back(Vertex(QVector3D(-m_crossBranchSize, 0.0, 0.0), QVector3D(), QVector3D(0.5, 0.2, 0.5)));
	m_vertices.push_back(Vertex(QVector3D(m_crossBranchSize, 0.0, 0.0), QVector3D(), QVector3D(0.5, 0.2, 0.5)));

	m_vertices.push_back(Vertex(QVector3D(0.0, -m_crossBranchSize, 0.0), QVector3D(), QVector3D(0.5, 0.2, 0.5)));
	m_vertices.push_back(Vertex(QVector3D(0.0, m_crossBranchSize, 0.0), QVector3D(), QVector3D(0.5, 0.2, 0.5)));

	m_indices = {
		0, 1, 2, 3
	};

	// inner circle
	const int circleFragmentCount = 24;
	for (int i = 0; i < circleFragmentCount; ++i){
		double currentX = m_crossBranchSize * 0.35 * cos(qDegreesToRadians(360./circleFragmentCount * i));
		double currentY = m_crossBranchSize * 0.35 * sin(qDegreesToRadians(360./circleFragmentCount * i));
		m_vertices.push_back(Vertex(QVector3D(currentX, currentY, 0.0), QVector3D(), QVector3D(0.7, 0.3, 0.7)));
	}

	for (int i = 4; i < m_vertices.count()-1; ++i){
		m_indices.push_back(i);
		m_indices.push_back(i + 1);
	}

	m_indices.push_back(m_vertices.count()-1);
	m_indices.push_back(4);

	const int lastIndex = m_vertices.count();

	// outer circle
	for (int i = 0; i < circleFragmentCount; ++i){
		double currentX = m_crossBranchSize * 0.8 * cos(qDegreesToRadians(360./circleFragmentCount * i));
		double currentY = m_crossBranchSize * 0.8 * sin(qDegreesToRadians(360./circleFragmentCount * i));
		m_vertices.push_back(Vertex(QVector3D(currentX, currentY, 0.0), QVector3D(), QVector3D(0.7, 0.3, 0.7)));
	}

	for (int i = lastIndex; i < m_vertices.count(); ++i){
		m_indices.push_back(i);
		m_indices.push_back(i + 1);
	}

	m_indices.push_back(m_vertices.count()-1);
	m_indices.push_back(lastIndex);
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CCrossTargetShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glLineWidth(5.0f);
	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


} // namespace imt3dgui


