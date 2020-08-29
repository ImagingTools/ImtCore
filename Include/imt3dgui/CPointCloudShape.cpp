#include <imt3dgui/CPointCloudShape.h>


// Qt includes
#include <QtGui/QTextDocument>

// ACF includes
#include <imod/IModel.h>

// ImtCore includes
#include <imt3d/IGridInfo.h>


namespace imt3dgui
{


// static members

const QVector3D CPointCloudShape::s_selectionColor(0.8, 0.8, 0.0);


// public methods

CPointCloudShape::CPointCloudShape()
	:m_color(QVector3D(0.0, 0.8, 0.2)),
	m_pointSize(3.0),
	m_isInfoBoxEnabled(false)
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


void CPointCloudShape::SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection)
{
	if (selectionPoint.isNull() || m_vertices.isEmpty()){
		return;
	}

	int closestVertexIndex = FindVertex(selectionPoint, true);

	// update selected vertices and vertex colors
	for (int i = 0; i < m_vertices.size(); ++i){
		if (i == closestVertexIndex){
			m_selectedVerticesIndicies.insert(i);
			m_vertices[i].color = s_selectionColor;
		}
		else if (clearPreviousSelection){
			m_selectedVerticesIndicies.erase(i);
			m_vertices[i].color = m_color;
		}
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CPointCloudShape::SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection)
{
	SetRectSelection(selectionRect, false, clearPreviousSelection);
}


void CPointCloudShape::SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection)
{
	SetRectSelection(selectionRect, true, clearPreviousSelection);
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


void CPointCloudShape::AllSelection()
{
	for (int i = 0; i < m_vertices.size(); ++i){
		m_selectedVerticesIndicies.insert(i);

		m_vertices[i].color = s_selectionColor;
	}

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
			vertex.color = s_selectionColor;
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
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormalRgba32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwRgba32>(*pointCloudPtr);
	}
}


void CPointCloudShape::SetInfoBoxEnabled(bool isEnabled)
{
	m_isInfoBoxEnabled = isEnabled;
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
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzwRgba32>(*pointCloudPtr);
	}
}


void CPointCloudShape::DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions)
{
	program.setUniformValue("usePointSize", true);
	program.setUniformValue("pointSize", m_pointSize);

	functions.glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, (GLuint*)0);
}


// reimplemented (imt3dview::IDrawable)

void CPointCloudShape::Draw(QPainter& painter)
{
	imt3d::IPointCloud3d* pointCloudPtr = dynamic_cast<imt3d::IPointCloud3d*>(GetObservedModel());
	if (!pointCloudPtr){
		return;
	}

	if (m_isInfoBoxEnabled){
		QString text = QString("<b><p>Total vertices: %1</p>").arg(pointCloudPtr->GetPointsCount());
		text += QString("<p>Selected vertices: %1</p></b>").arg(static_cast<int>(m_selectedVerticesIndicies.size()));

		painter.save();

		painter.setBrush(QBrush(QColor(240, 240, 240)));
		painter.drawRoundedRect(10, 10, 300, 60, 3.0, 3.0);
		painter.translate(15.0, 15.0);

		QTextDocument doc;
		doc.setHtml(text);
		doc.drawContents(&painter);

		painter.restore();
	}
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

void CPointCloudShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection)
{
	if (!selectionRect.isValid() || m_vertices.isEmpty()){
		return;
	}

	for (int i = 0; i < m_vertices.size(); ++i){
		Vertex& vertex = m_vertices[i];

		QPoint windowPosition = ModelToWindow(vertex.position);

		if (IsPointWithin(windowPosition, selectionRect, isCircle)){
			m_selectedVerticesIndicies.insert(i);
			vertex.color = s_selectionColor;
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
	m_indices.clear();

	int pointCloudSize = pointCloud.GetPointsCount();

	if (pointCloudSize <= 0){
		return;
	}

	m_vertices.reserve(pointCloudSize);
	m_indices.reserve(pointCloudSize);

	imt3d::IPointsBasedObject::PointFormat format = pointCloud.GetPointFormat();

	// update vertices
	for (int i = 0; i < pointCloudSize; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float x = static_cast<float>(pointDataPtr->data[0]);
		float y = static_cast<float>(pointDataPtr->data[1]);
		float z = static_cast<float>(pointDataPtr->data[2]);

		if (qIsNaN(x) || qIsNaN(y) || qIsNaN(z)){
			continue;
		}

		QVector3D color = m_color;

		if (format == imt3d::IPointsBasedObject::PF_XYZW_RGBA_32){
			float r = static_cast<float>(pointDataPtr->data[4]);
			float g = static_cast<float>(pointDataPtr->data[5]);
			float b = static_cast<float>(pointDataPtr->data[6]);

			color = QVector3D(r, g, b);
		}

		m_vertices.push_back(Vertex(QVector3D(x, y, z), QVector3D(), color));
		m_indices.push_back(m_indices.size());
	}
}


template <typename PointType>
void CPointCloudShape::DeleteSelectionHelper(imt3d::IPointCloud3d& pointCloud)
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	int pointsCount = pointCloud.GetPointsCount();
	int remainingPointsCount = pointsCount - static_cast<int>(m_selectedVerticesIndicies.size());

	if (remainingPointsCount <= 0){
		return; // we can't create empty point cloud
	}

	// save point cloud data to create the same point cloud without selected points
	imt3d::IPointsBasedObject::PointFormat pointFormat = pointCloud.GetPointFormat();

	istd::CIndex2d gridSize;

	const imt3d::IGridInfo* gridInfoPtr = dynamic_cast<const imt3d::IGridInfo*>(&pointCloud);
	if (gridInfoPtr){
		gridSize = gridInfoPtr->GetGridSize();
	}

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


