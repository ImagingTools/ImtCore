#include <imt3dgui/CGridShape.h>


namespace imt3dgui
{


// public methods

CGridShape::CGridShape()
	:m_gridValue(0.0),
	m_count(0),
	m_doUpdate(true)
{
	m_pointsDataPtr = &m_data;
}


void CGridShape::SetGridValue(double gridValue)
{
	Q_ASSERT(gridValue > 0.0);

	m_gridValue = gridValue;
	m_doUpdate = true;
}


void CGridShape::SetCount(int count)
{
	Q_ASSERT(count > 0);

	m_count = count;
	m_doUpdate = true;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CGridShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_doUpdate) {
		return;
	}

	std::vector<imt3d::CPointCloud3d::PointXyz32> vertices;
	vertices.reserve(m_count * 2 + 1);

	for(int i = -m_count; i <= m_count; ++i){
		imt3d::CPointCloud3d::PointXyz32 a;
		a.data[0] = m_gridValue * i;
		a.data[1] = -m_gridValue * m_count;
		a.data[2] = 0.0;
		vertices.emplace_back(a);

		imt3d::CPointCloud3d::PointXyz32 b;
		b.data[0] = m_gridValue * i;
		b.data[1] = m_gridValue * m_count;
		b.data[2] = 0.0;
		vertices.emplace_back(b);
	}

	for(int i = -m_count; i <= m_count; ++i){
		imt3d::CPointCloud3d::PointXyz32 a;
		a.data[0] = -m_gridValue * m_count;
		a.data[1] = m_gridValue * i;
		a.data[2] = 0.0;
		vertices.emplace_back(a);

		imt3d::CPointCloud3d::PointXyz32 b;
		b.data[0] = m_gridValue * m_count;
		b.data[1] = m_gridValue * i;
		b.data[2] = 0.0;
		vertices.emplace_back(b);
	}

	m_data.CreateCloud(imt3d::CPointCloud3d::PF_XYZ_32, vertices.size(), vertices.data());

	m_indices.clear();
	m_indices.reserve(vertices.size());

	for(int i = 0; i < vertices.size(); ++i){
		m_indices.push_back(i);
	}

	m_doUpdate = false;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CGridShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glLineWidth(1.0f);
	functions.glDrawElements(GL_LINES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


} // namespace imt3dgui


