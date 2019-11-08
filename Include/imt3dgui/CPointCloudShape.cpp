#include <imt3dgui/CPointCloudShape.h>


// Qt includes
#include <QtGui/QTextDocument.h>

// ACF includes
#include <imod/IModel.h>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


// static members

const QVector3D CPointCloudShape::s_selectionColor(0.0, 0.0, 0.0);
const QVector3D CPointCloudShape::s_selectionCubeColor(0.5, 0.5, 0.5);
const QVector3D CPointCloudShape::s_selectionCubeXColor(0.75, 0.0, 0.0);
const QVector3D CPointCloudShape::s_selectionCubeYColor(0.0, 0.75, 0.0);
const QVector3D CPointCloudShape::s_selectionCubeZColor(0.0, 0.0, 0.75);
const int CPointCloudShape::s_selectionCubeFacesSize = 24;
const int CPointCloudShape::s_selectionCubeEdgesSize = 24;


// public methods

CPointCloudShape::CPointCloudShape()
	:m_color(QVector3D(0.0, 0.8, 0.2)),
	m_pointSize(2.0),
	m_pointCloudSize(0)
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


void CPointCloudShape::SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection, const QRect& viewPort)
{
	if (selectionPoint.isNull() || m_pointCloudSize <= 0){
		return;
	}

	// project window 2D coordinate to near and far planes getting 3D world coordinates
	// create a ray between those points
	QVector3D rayFrom = WindowToModel(selectionPoint, 0, viewPort);
	QVector3D rayTo = WindowToModel(selectionPoint, 1, viewPort);

	QVector3D rayDirection = rayTo - rayFrom;
	rayDirection.normalize();

	// find a vertex closest to the ray
	float minDist = qInf();
	int closestVertexIndex = -1;

	for (int i = 0; i < m_pointCloudSize; ++i){
		float dist = m_vertices[i].position.distanceToLine(rayFrom, rayDirection);

		if (dist < minDist){
			minDist = dist;
			closestVertexIndex = i;
		}
	}

	// update selected vertices and vertex colors
	for (int i = 0; i < m_pointCloudSize; ++i){
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

	for (int i = 0; i < m_pointCloudSize; ++i){
		m_vertices[i].color = m_color;
	}

	m_selectedVerticesIndicies.clear();
	m_cubeSelectionSize = QVector3D();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CPointCloudShape::AllSelection()
{
	for (int i = 0; i < m_pointCloudSize; ++i){
		m_selectedVerticesIndicies.insert(i);

		m_vertices[i].color = s_selectionColor;
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CPointCloudShape::InvertSelection()
{
	for (int i = 0; i < m_pointCloudSize; ++i){
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
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*pointCloudPtr);
	}
}


void CPointCloudShape::BoxFromSelection()
{
	m_cubeSelectionSize = QVector3D();

	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	// calculate selection cube boundaries
	istd::TRange<float> xRange, yRange, zRange;
	if (!CalculateSelectionBox(xRange, yRange, zRange)){
		return;
	}

	SetSelectionCubeFaces(xRange, yRange, zRange);
	SetSelectionCubeEdges(xRange, yRange, zRange);
	SetSelectionCubeEdgeColors();

	// calculate selection cube size in pixels
	m_cubeSelectionSize.setX(xRange.GetLength());
	m_cubeSelectionSize.setY(yRange.GetLength());
	m_cubeSelectionSize.setZ(zRange.GetLength());

	UploadGeometry(false, m_vertices, m_vertexBuffer);
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

	// draw point cloud
	const GLuint* indexBufferOffset = 0;
	functions.glDrawElements(GL_POINTS, m_pointCloudSize, GL_UNSIGNED_INT, indexBufferOffset);

	// draw selection cube
	if (!m_cubeSelectionSize.isNull()){
		// draw selection cube faces
		indexBufferOffset += m_pointCloudSize;
		functions.glDrawElements(GL_QUADS, s_selectionCubeFacesSize, GL_UNSIGNED_INT, indexBufferOffset);

		// draw selection cube edges
		indexBufferOffset += s_selectionCubeFacesSize;

		functions.glLineWidth(3.0f);
		functions.glDrawElements(GL_LINES, s_selectionCubeEdgesSize, GL_UNSIGNED_INT, indexBufferOffset);
	}
}


// reimplemented (imt3dview::IDrawable)

void CPointCloudShape::Draw(QPainter& painter)
{
	imt3d::IPointCloud3d* pointCloudPtr = dynamic_cast<imt3d::IPointCloud3d*>(GetObservedModel());
	if (!pointCloudPtr || !m_cameraPtr){
		return;
	}

	QString text = QString("<b><p>Vertices: %1</p>").arg(pointCloudPtr->GetPointsCount());
	text += QString("<p>Selected vertices: %1</p></b>").arg(static_cast<int>(m_selectedVerticesIndicies.size()));

	if (!m_cubeSelectionSize.isNull()){
		text += QString("<b><p>Selection cuboid: <span style='color: rgb(%1, %2, %3)'>%4</span> x ")
				.arg(static_cast<int>(s_selectionCubeXColor.x() * 255.0))
				.arg(static_cast<int>(s_selectionCubeXColor.y() * 255.0))
				.arg(static_cast<int>(s_selectionCubeXColor.z() * 255.0))
				.arg(m_cubeSelectionSize.x(), 0, 'f', 2);

		text += QString("<span style='color: rgb(%1, %2, %3)'>%4</span> x ")
				.arg(static_cast<int>(s_selectionCubeYColor.x() * 255.0))
				.arg(static_cast<int>(s_selectionCubeYColor.y() * 255.0))
				.arg(static_cast<int>(s_selectionCubeYColor.z() * 255.0))
				.arg(m_cubeSelectionSize.y(), 0, 'f', 2);

		text += QString("<span style='color: rgb(%1, %2, %3)'>%4</span></p></b>")
				.arg(static_cast<int>(s_selectionCubeZColor.x() * 255.0))
				.arg(static_cast<int>(s_selectionCubeZColor.y() * 255.0))
				.arg(static_cast<int>(s_selectionCubeZColor.z() * 255.0))
				.arg(m_cubeSelectionSize.z(), 0, 'f', 2);
	}

	painter.save();

	painter.setBrush(QBrush(QColor(240, 240, 240)));
	painter.drawRoundedRect(10, 10, 270, 90, 3.0, 3.0);
	painter.translate(15.0, 15.0);

	QTextDocument doc;
	doc.setHtml(text);
	doc.drawContents(&painter);

	painter.restore();
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

void CPointCloudShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort)
{
	if (!selectionRect.isValid() || m_pointCloudSize <= 0){
		return;
	}

	for (int i = 0; i < m_pointCloudSize; ++i){
		Vertex& vertex = m_vertices[i];

		QPoint windowPosition = ModelToWindow(vertex.position, viewPort);

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
	const int cubeVerticesSize = s_selectionCubeFacesSize + s_selectionCubeEdgesSize;

	m_pointCloudSize = pointCloud.GetPointsCount();

	m_vertices.clear();
	m_vertices.reserve(m_pointCloudSize + cubeVerticesSize);

	m_indices.clear();
	m_indices.reserve(m_vertices.capacity());

	for (int i = 0; i < m_pointCloudSize; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float x = static_cast<float>(pointDataPtr->data[0]);
		float y = static_cast<float>(pointDataPtr->data[1]);
		float z = static_cast<float>(pointDataPtr->data[2]);

		if (qIsNaN(x) || qIsNaN(y) || qIsNaN(z)){
			continue;
		}

		m_vertices.push_back(Vertex(QVector3D(x, y, z), QVector3D(), m_color));
		m_indices.push_back(m_indices.size());
	}

	// add empty selection cube vertices so that they will be allocated in opengl buffers
	for (int i = 0; i < cubeVerticesSize; ++i){
		m_vertices.push_back(Vertex(QVector3D(), QVector3D(), s_selectionCubeColor));
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


bool CPointCloudShape::CalculateSelectionBox(
				istd::TRange<float>& xRange,
				istd::TRange<float>& yRange,
				istd::TRange<float>& zRange)
{
	xRange.SetMinValue(std::numeric_limits<float>::max());
	xRange.SetMaxValue(std::numeric_limits<float>::lowest());

	yRange = xRange;
	zRange = xRange;

	for (int i : m_selectedVerticesIndicies){
		const QVector3D& position = m_vertices[i].position;

		xRange.SetMinValue(qMin(xRange.GetMinValue(), position.x()));
		xRange.SetMaxValue(qMax(xRange.GetMaxValue(), position.x()));
		yRange.SetMinValue(qMin(yRange.GetMinValue(), position.y()));
		yRange.SetMaxValue(qMax(yRange.GetMaxValue(), position.y()));
		zRange.SetMinValue(qMin(zRange.GetMinValue(), position.z()));
		zRange.SetMaxValue(qMax(zRange.GetMaxValue(), position.z()));
	}

	return xRange.IsValidNonEmpty() && yRange.IsValidNonEmpty() && zRange.IsValidNonEmpty();
}


void CPointCloudShape::SetSelectionCubeFaces(
				const istd::TRange<float>& xRange,
				const istd::TRange<float>& yRange,
				const istd::TRange<float>& zRange)
{
	int i = m_pointCloudSize;

	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());

	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());

	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());

	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());

	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());

	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());
}


void CPointCloudShape::SetSelectionCubeEdges(
				const istd::TRange<float>& xRange,
				const istd::TRange<float>& yRange,
				const istd::TRange<float>& zRange)
{
	int i = m_pointCloudSize + s_selectionCubeFacesSize;

	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());

	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());

	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMinValue(), yRange.GetMinValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMaxValue(), zRange.GetMinValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMaxValue());
	m_vertices[i++].position = QVector3D(xRange.GetMaxValue(), yRange.GetMinValue(), zRange.GetMinValue());
}


void CPointCloudShape::SetSelectionCubeEdgeColors()
{
	for (int i = 0; i < s_selectionCubeEdgesSize; i += 2){
		int idx = i + m_pointCloudSize + s_selectionCubeFacesSize;

		Vertex& v1 = m_vertices[idx];
		Vertex& v2 = m_vertices[idx + 1];

		if (v1.position.x() != v2.position.x()){
			v1.color = v2.color = s_selectionCubeXColor;
		}
		else if (v1.position.y() != v2.position.y()){
			v1.color = v2.color = s_selectionCubeYColor;
		}
		else{
			v1.color = v2.color = s_selectionCubeZColor;
		}
	}
}


} // namespace imt3dgui
