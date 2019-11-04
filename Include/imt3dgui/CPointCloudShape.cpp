#include <imt3dgui/CPointCloudShape.h>


// ACF includes
#include <imod/IModel.h>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


// public methods

CPointCloudShape::CPointCloudShape()
	:m_color(QVector3D(0.0, 0.8, 0.2)),
	m_selectionColor(QVector3D(0.0, 0.0, 0.0)),
	m_pointSize(2.0)
{
}


void CPointCloudShape::SetColor(const QVector3D& color)
{
	m_color = color;
}


void CPointCloudShape::SetPointSize(float pointSize)
{
	m_pointSize = pointSize;
}


void CPointCloudShape::SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort)
{
	SetRectSelection(selectionRect, false, clearPreviousSelection, viewPort);
}


void CPointCloudShape::SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort)
{
	SetRectSelection(selectionRect, true, clearPreviousSelection, viewPort);
}


void CPointCloudShape::ClearSelection()
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	for (Vertex& vertex : m_vertices){
		vertex.color = m_color;
	}

	m_selectedVerticesIndicies.clear();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CPointCloudShape::InvertSelection()
{
	for (int i = 0; i < m_vertices.size(); ++i){
		Vertex& vertex = m_vertices[i];

		SelectedVerticesIndicies::iterator it = m_selectedVerticesIndicies.find(i);

		if (it != m_selectedVerticesIndicies.end()){
			m_selectedVerticesIndicies.erase(it);
			vertex.color = m_color;
		}
		else{
			m_selectedVerticesIndicies.insert(i);
			vertex.color = m_selectionColor;
		}
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CPointCloudShape::DeleteSelection()
{
	imt3d::IPointCloud3d* pointCloudPtr = dynamic_cast<imt3d::IPointCloud3d*>(GetObservedModel());
	if (!pointCloudPtr){
		return;
	}

	switch (pointCloudPtr->GetPointFormat()){
		case imt3d::IPointsBasedObject::PF_XYZ_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyz32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_64:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyz64>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzw32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzAbc32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
	}
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CPointCloudShape::UpdateShapeGeometry()
{
	imt3d::IPointCloud3d* pointCloudPtr = dynamic_cast<imt3d::IPointCloud3d*>(GetObservedModel());
	if (!pointCloudPtr){
		return;
	}

	switch (pointCloudPtr->GetPointFormat()){
		case imt3d::IPointsBasedObject::PF_XYZ_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyz32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_64:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyz64>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzw32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzAbc32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
	}
}


void CPointCloudShape::DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions)
{
	program.setUniformValue("usePointSize", true);
	program.setUniformValue("pointSize", m_pointSize);
	functions.glDrawElements(GL_POINTS, m_indices.count(), GL_UNSIGNED_INT, 0);
}


// reimplemented (imt3dgui::IShape3d)

imt3dgui::IShape3d::ColorMode CPointCloudShape::GetColorMode() const
{
	return IShape3d::CM_POINT;
}


QVector3D CPointCloudShape::GetColor() const
{
	return m_color;
}


// private methods

int CPointCloudShape::SelectVertex(const QPoint& windowPoint, const QRect& viewPort) const
{
	if (m_vertices.isEmpty()){
		return -1;
	}

	// project window 2D coordinate to near and far planes getting 3D world coordinates
	// create a ray between those points
	QVector3D rayFrom = WindowToModel(windowPoint, 0, viewPort);
	QVector3D rayTo = WindowToModel(windowPoint, 1, viewPort);

	QVector3D rayDirection = rayTo - rayFrom;
	rayDirection.normalize();

	// find point cloud vertex correponding closest to the ray
	float minDist = qInf();
	int closestVertexIndex = -1;

	for (int i = 0; i < m_vertices.size(); ++i){
		float dist = m_vertices[i].position.distanceToLine(rayFrom, rayDirection);

		if (dist < minDist)
		{
			minDist = dist;
			closestVertexIndex = i;
		}
	}

	return closestVertexIndex;
}


void CPointCloudShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort)
{
	if (!selectionRect.isValid()){
		return;
	}

	for (int i = 0; i < m_vertices.size(); ++i){
		Vertex& vertex = m_vertices[i];

		QPoint windowPosition = ModelToWindow(vertex.position, viewPort);

		if (IsPointWithin(windowPosition, selectionRect, isCircle)){
			m_selectedVerticesIndicies.insert(i);
			vertex.color = m_selectionColor;
		}
		else if (clearPreviousSelection){
			m_selectedVerticesIndicies.erase(i);
			vertex.color = m_color;
		}
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


template <typename PointType>
void CPointCloudShape::UpdateShapeGeometryHelper(const imt3d::IPointCloud3d& pointCloud)
{
	m_vertices.clear();
	m_pointCloudIndicies.clear();

	// update vertices
	int pointsCount = pointCloud.GetPointsCount();

	m_vertices.reserve(pointsCount);
	m_pointCloudIndicies.reserve(pointsCount);

	for (int i = 0; i < pointsCount; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float x = static_cast<float>(pointDataPtr->data[0]);
		float y = static_cast<float>(pointDataPtr->data[1]);
		float z = static_cast<float>(pointDataPtr->data[2]);

		if (qIsNaN(x) || qIsNaN(y) || qIsNaN(z)){
			continue;
		}

		m_pointCloudIndicies.push_back(i);
		m_vertices.push_back(Vertex(QVector3D(x, y, z), QVector3D(), m_color));
	}

	// update indices
	m_indices.clear();
	m_indices.reserve(pointsCount);

	for (int i = 0; i < pointsCount; ++i){
		m_indices.push_back(i);
	}
}


template <typename PointType>
void CPointCloudShape::DeleteSelectionHelper(imt3d::IPointCloud3d& pointCloud)
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	// save point cloud data to create the same point cloud without selected points
	imt3d::IPointsBasedObject::PointFormat pointFormat = pointCloud.GetPointFormat();

	istd::CIndex2d gridSize;

	const imt3d::IGridInfo* gridInfoPtr = dynamic_cast<const imt3d::IGridInfo*>(&pointCloud);
	if (gridInfoPtr){
		gridSize = gridInfoPtr->GetGridSize();
	}

	int pointsCount = pointCloud.GetPointsCount();
	int remainingPointsCount = pointsCount - static_cast<int>(m_selectedVerticesIndicies.size());

	PointType* pointsDataPtr = new PointType[remainingPointsCount];
	PointType* ptr = pointsDataPtr;

	// add only non-selected cloud points
	// corresponding vertices and indicies will be removed after change notification in CPointCloud3d::CreateCloud
	for (int i = 0; i < pointsCount; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(i));
		if (!pointDataPtr){
			continue;
		}

		SelectedVerticesIndicies::iterator it = m_selectedVerticesIndicies.find(i);
		if (it == m_selectedVerticesIndicies.end()){
			memcpy(ptr++, pointDataPtr, sizeof(PointType));
		}
	}

	pointCloud.CreateCloud(pointFormat, remainingPointsCount, pointsDataPtr, true, gridSize.IsZero() ? nullptr : &gridSize);

	m_selectedVerticesIndicies.clear();
}


bool CPointCloudShape::IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle)
{
	if (isCircle){
		QPoint center = rect.center();

		qreal ellipseEquation = qPow(point.x() - center.x(), 2) / qPow(rect.width() / 2, 2) +
			qPow(point.y() - center.y(), 2) / qPow(rect.height() / 2, 2);

		return ellipseEquation <= 1.0;
	}
	else{
		return rect.contains(point);
	}
}


} // namespace imt3dgui
