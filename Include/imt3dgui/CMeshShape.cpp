#include <imt3dgui/CMeshShape.h>


// ACF includes
#include <imod/IModel.h>


namespace imt3dgui
{


// public methods

CMeshShape::CMeshShape()
{
}


CMeshShape::~CMeshShape()
{
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CMeshShape::UpdateShapeGeometry()
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return;
	}

	switch (meshPtr->GetPointFormat()){
		case imt3d::IPointsBasedObject::PF_XYZ_32:
			return TUpdateShapeGeometry<imt3d::IPointsBasedObject::PointXyz32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_64:
			return TUpdateShapeGeometry<imt3d::IPointsBasedObject::PointXyz64>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_32:
			return TUpdateShapeGeometry<imt3d::IPointsBasedObject::PointXyzw32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
			return TUpdateShapeGeometry<imt3d::IPointsBasedObject::PointXyzAbc32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return TUpdateShapeGeometry<imt3d::IPointsBasedObject::PointXyzwNormal32>(*meshPtr);
	}
}


void CMeshShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glDrawElements(GL_TRIANGLES, m_indices.count(), GL_UNSIGNED_INT, 0);
}


bool CMeshShape::HasNormals() const
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return false;
	}

	if (meshPtr->GetPointFormat() == imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32){
		return true;
	}
	else{
		return false;
	}
}


template <typename PointType>
void CMeshShape::TUpdateShapeGeometry(const imt3d::IMesh3d& mesh)
{
	m_vertices.clear();
	m_indices.clear();

	// update vertices and normals
	int pointsCount = mesh.GetPointsCount();
	const imt3d::IMesh3d::Indices& indices = mesh.GetIndices();

	if (pointsCount <= 0 || indices.empty()){
		return;
	}

	m_vertices.resize(pointsCount);

	for (int i = 0; i < pointsCount; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(mesh.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float pointX = static_cast<float>(pointDataPtr->data[0]);
		float pointY = static_cast<float>(pointDataPtr->data[1]);
		float pointZ = static_cast<float>(pointDataPtr->data[2]);
		float normalX = static_cast<float>(pointDataPtr->data[4]);
		float normalY = static_cast<float>(pointDataPtr->data[5]);
		float normalZ = static_cast<float>(pointDataPtr->data[6]);

		m_vertices[i].position = QVector3D(pointX, pointY, pointZ);
		m_vertices[i].normal = QVector3D(normalX, normalY, normalZ);

		EnsureNormalExist(i);
	}

	// update indices
	m_indices.reserve(static_cast<int>(indices.size() * indices.front().size()));

	for (int i = 0; i < indices.size(); ++i){
		const std::vector<uint32_t>& index = indices[i];

		for (int j = 0; j < index.size(); ++j){
			m_indices.push_back(index[j]);
		}
	}
}


void CMeshShape::EnsureNormalExist(int pointIndex)
{
	if ((pointIndex + 1) % 3 == 0){
		Vertex& point1 = m_vertices[pointIndex - 0];
		Vertex& point2 = m_vertices[pointIndex - 1];
		Vertex& point3 = m_vertices[pointIndex - 2];

		if (point1.normal == QVector3D(0.0, 0.0, 0.0) &&
			point2.normal == QVector3D(0.0, 0.0, 0.0) &&
			point3.normal == QVector3D(0.0, 0.0, 0.0)){
			point1.normal = point2.normal = point3.normal = QVector3D::normal(point1.position, point2.position, point3.position);
		}
	}
}


} // namespace imt3dgui


