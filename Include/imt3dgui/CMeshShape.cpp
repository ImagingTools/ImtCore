#include <imt3dgui/CMeshShape.h>


// Qt includes
#include <QtGui/QTextDocument.h>

// ACF includes
#include <imod/IModel.h>


namespace imt3dgui
{


// static members

const QVector3D CMeshShape::s_selectionColor(0.5, 0.0, 0.5);
const QVector3D CMeshShape::s_selectionCubeColor(0.5, 0.5, 0.5);
const QVector3D CMeshShape::s_selectionCubeXColor(0.75, 0.0, 0.0);
const QVector3D CMeshShape::s_selectionCubeYColor(0.0, 0.75, 0.0);
const QVector3D CMeshShape::s_selectionCubeZColor(0.0, 0.0, 0.75);
const int CMeshShape::s_selectionCubeFacesSize = 24;
const int CMeshShape::s_selectionCubeEdgesSize = 24;


// public methods

CMeshShape::CMeshShape()
	:m_color(QVector3D(0.0, 1.0, 1.0)),
	m_meshSize(0)
{
}


void CMeshShape::SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection, const QRect& viewPort)
{
	if (selectionPoint.isNull() || m_meshSize <= 0){
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
	VertexIndicies closestVertexIndicies;

	for (int i = 0; i < m_meshSize; ++i){
		float dist = m_vertices[i].position.distanceToLine(rayFrom, rayDirection);

		if (dist < minDist){
			minDist = dist;

			closestVertexIndicies.clear();
			closestVertexIndicies.insert(i);
		}
		else if (qFuzzyCompare(dist, minDist)){
			closestVertexIndicies.insert(i);
		}
	}

	// update selected vertices and vertex colors
	for (int i = 0; i < m_meshSize; ++i){
		if (closestVertexIndicies.find(i) != closestVertexIndicies.cend()){
			m_selectedVerticesIndicies.insert(i);
			m_vertices[i].color = s_selectionColor;
		}
		else if (clearPreviousSelection){
			m_selectedVerticesIndicies.erase(i);
			m_vertices[i].color = m_color;
		}
	}

	// select not just selected vertex but entire face
	SelectFaceVertices();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort)
{
	SetRectSelection(selectionRect, false, clearPreviousSelection, viewPort);
}


void CMeshShape::SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort)
{
	SetRectSelection(selectionRect, true, clearPreviousSelection, viewPort);
}


void CMeshShape::ClearSelection()
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	for (int i = 0; i < m_meshSize; ++i){
		m_vertices[i].color = m_color;
	}

	m_selectedVerticesIndicies.clear();
	m_cubeSelectionSize = QVector3D();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::AllSelection()
{
	for (int i = 0; i < m_meshSize; ++i){
		m_selectedVerticesIndicies.insert(i);

		m_vertices[i].color = s_selectionColor;
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::InvertSelection()
{
	for (int i = 0; i < m_meshSize; ++i){
		Vertex& vertex = m_vertices[i];

		VertexIndicies::iterator it = m_selectedVerticesIndicies.find(i);

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


void CMeshShape::DeleteSelection()
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return;
	}

	switch (meshPtr->GetPointFormat()){
		case imt3d::IPointsBasedObject::PF_XYZ_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyz32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_64:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyz64>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzw32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzAbc32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*meshPtr);
	}
}


void CMeshShape::BoxFromSelection()
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

void CMeshShape::UpdateShapeGeometry()
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return;
	}

	switch (meshPtr->GetPointFormat()){
		case imt3d::IPointsBasedObject::PF_XYZ_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyz32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_64:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyz64>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzw32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzAbc32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32:
			return UpdateShapeGeometryHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*meshPtr);
	}
}


void CMeshShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	// draw mesh
	const GLuint* indexBufferOffset = 0;

	functions.glDrawElements(GL_TRIANGLES, m_meshSize, GL_UNSIGNED_INT, indexBufferOffset);

	// draw selection cube
	if (!m_cubeSelectionSize.isNull()){
		// draw selection cube faces
		indexBufferOffset += m_meshSize;

		functions.glDrawElements(GL_QUADS, s_selectionCubeFacesSize, GL_UNSIGNED_INT, indexBufferOffset);

		// draw selection cube edges
		indexBufferOffset += s_selectionCubeFacesSize;

		functions.glLineWidth(3.0f);
		functions.glDrawElements(GL_LINES, s_selectionCubeEdgesSize, GL_UNSIGNED_INT, indexBufferOffset);
	}
}


// reimplemented (imt3dview::IDrawable)

void CMeshShape::Draw(QPainter& painter)
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr || !m_cameraPtr){
		return;
	}

	QString text = QString("<b><p>Total vertices: %1</p>").arg(meshPtr->GetPointsCount());
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
	painter.drawRoundedRect(10, 10, 300, 90, 3.0, 3.0);
	painter.translate(15.0, 15.0);

	QTextDocument doc;
	doc.setHtml(text);
	doc.drawContents(&painter);

	painter.restore();
}


IShape3d::ColorMode CMeshShape::GetColorMode() const
{
	return IShape3d::CM_POINT;
}


QVector3D CMeshShape::GetColor() const
{
	return m_color;
}


bool CMeshShape::HasNormals() const
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return false;
	}

	return meshPtr->GetPointFormat() == imt3d::IPointsBasedObject::PF_XYZW_NORMAL_32;
}


// private methods

void CMeshShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort)
{
	if (!selectionRect.isValid() || m_meshSize <= 0){
		return;
	}

	for (int i = 0; i < m_meshSize; ++i){
		QPoint windowPosition = ModelToWindow(m_vertices[i].position, viewPort);

		if (IsPointWithin(windowPosition, selectionRect, isCircle)){
			m_selectedVerticesIndicies.insert(i);
			m_vertices[i].color = s_selectionColor;
		}
		else if (clearPreviousSelection){
			m_selectedVerticesIndicies.erase(i);
			m_vertices[i].color = m_color;
		}
	}

	// select not just selected vertex but entire face
	SelectFaceVertices();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


template <typename PointType>
void CMeshShape::UpdateShapeGeometryHelper(const imt3d::IMesh3d& mesh)
{
	const int cubeVerticesSize = s_selectionCubeFacesSize + s_selectionCubeEdgesSize;

	m_meshSize = mesh.GetPointsCount();
	const imt3d::IMesh3d::Indices& indices = mesh.GetIndices();

	m_vertices.clear();
	m_vertices.reserve(m_meshSize + cubeVerticesSize);

	if (m_meshSize <= 0 || indices.empty()){
		return;
	}

	// update vertices and normals
	for (int i = 0; i < m_meshSize; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(mesh.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float pointX = static_cast<float>(pointDataPtr->data[0]);
		float pointY = static_cast<float>(pointDataPtr->data[1]);
		float pointZ = static_cast<float>(pointDataPtr->data[2]);
		float normalX = static_cast<float>(pointDataPtr->data[4]);
		float normalY = static_cast<float>(pointDataPtr->data[5]);
		float normalZ = static_cast<float>(pointDataPtr->data[6]);

		m_vertices.push_back(Vertex(QVector3D(pointX, pointY, pointZ),
				QVector3D(normalX, normalY, normalZ),
				m_color));
	}

	// update indices
	m_indices.clear();
	m_indices.reserve(static_cast<int>(indices.size() * indices.front().size()) + cubeVerticesSize);

	for (int i = 0; i < indices.size(); ++i){
		const std::vector<uint32_t>& index = indices[i];

		for (int j = 0; j < index.size(); ++j){
			m_indices.push_back(index[j]);
		}
	}

	// add empty selection cube vertices so that they will be allocated in opengl buffers
	for (int i = 0; i < cubeVerticesSize; ++i){
		m_vertices.push_back(Vertex(QVector3D(), QVector3D(), s_selectionCubeColor));
		m_indices.push_back(m_indices.size());
	}
}


template <typename PointType>
void CMeshShape::DeleteSelectionHelper(imt3d::IMesh3d& mesh)
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	int pointsCount = mesh.GetPointsCount();
	int remainingPointsCount = pointsCount - static_cast<int>(m_selectedVerticesIndicies.size());

	if (remainingPointsCount <= 0){
		return; // we can't create empty mesh
	}

	// save mesh data to create the same mesh without selected points
	imt3d::IPointsBasedObject::PointFormat pointFormat = mesh.GetPointFormat();

	PointType* pointsDataPtr = new PointType[remainingPointsCount];
	PointType* ptr = pointsDataPtr;

	// add only non-selected mesh points
	// corresponding vertices and indicies will be removed after change notification in IMesh3d::CreateMesh
	for (int i = 0; i < pointsCount; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(mesh.GetPointData(i));
		if (!pointDataPtr){
			continue;
		}

		if (m_selectedVerticesIndicies.find(i) == m_selectedVerticesIndicies.end()){
			memcpy(ptr++, pointDataPtr, sizeof(PointType));
		}
	}

	// rebuild vertex indices
	imt3d::IMesh3d::Indices indices;
	indices.resize(remainingPointsCount / 3);

	for (int i = 0; i < indices.size(); ++i){
		indices[i].push_back(i * 3 + 0);
		indices[i].push_back(i * 3 + 1);
		indices[i].push_back(i * 3 + 2);
	}

	// create new mesh
	mesh.CreateMesh(pointFormat, remainingPointsCount, pointsDataPtr, true, indices);

	m_selectedVerticesIndicies.clear();
}


bool CMeshShape::IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle)
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


bool CMeshShape::CalculateSelectionBox(
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


void CMeshShape::SetSelectionCubeFaces(
				const istd::TRange<float>& xRange,
				const istd::TRange<float>& yRange,
				const istd::TRange<float>& zRange)
{
	int i = m_meshSize;

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


void CMeshShape::SetSelectionCubeEdges(
				const istd::TRange<float>& xRange,
				const istd::TRange<float>& yRange,
				const istd::TRange<float>& zRange)
{
	int i = m_meshSize + s_selectionCubeFacesSize;

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


void CMeshShape::SetSelectionCubeEdgeColors()
{
	for (int i = 0; i < s_selectionCubeEdgesSize; i += 2){
		int idx = i + m_meshSize + s_selectionCubeFacesSize;

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


void CMeshShape::SelectFaceVertices()
{
	VertexIndicies selectedVerticesIndicies;

	for (int i : m_selectedVerticesIndicies){
		int faceVertexIdx = (i / 3) * 3;

		selectedVerticesIndicies.insert(faceVertexIdx + 0);
		selectedVerticesIndicies.insert(faceVertexIdx + 1);
		selectedVerticesIndicies.insert(faceVertexIdx + 2);

		m_vertices[faceVertexIdx + 0].color = s_selectionColor;
		m_vertices[faceVertexIdx + 1].color = s_selectionColor;
		m_vertices[faceVertexIdx + 2].color = s_selectionColor;
	}

	m_selectedVerticesIndicies.swap(selectedVerticesIndicies);
}


} // namespace imt3dgui


