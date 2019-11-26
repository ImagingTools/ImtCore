#include <imt3dgui/CMeshShape.h>


// Qt includes
#include <QtGui/QTextDocument.h>

// ACF includes
#include <imod/IModel.h>


namespace imt3dgui
{


// static members

const QVector3D CMeshShape::s_selectionColor(0.5, 0.0, 0.5);


// public methods

CMeshShape::CMeshShape()
	:m_color(QVector3D(0.0, 1.0, 1.0))
{
}


void CMeshShape::SetColor(const QVector3D& color)
{
	m_color = color;
}


void CMeshShape::SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection)
{
	// get 2D point-face intersection if any
	int selectedFacePointIndex = -1;
	QVector3D intersectionPoint;

	IsPointFaceIntersection(selectionPoint, selectedFacePointIndex, intersectionPoint);

	// select/deselect corresponding vertices
	for (int i = 0; i < m_vertices.size(); i += 3){
		if (i == selectedFacePointIndex){
			m_selectedVerticesIndicies.insert(i + 0);
			m_selectedVerticesIndicies.insert(i + 1);
			m_selectedVerticesIndicies.insert(i + 2);

			m_vertices[i + 0].color = s_selectionColor;
			m_vertices[i + 1].color = s_selectionColor;
			m_vertices[i + 2].color = s_selectionColor;
		}
		else if (clearPreviousSelection){
			m_selectedVerticesIndicies.erase(i + 0);
			m_selectedVerticesIndicies.erase(i + 1);
			m_selectedVerticesIndicies.erase(i + 2);

			m_vertices[i + 0].color = m_color;
			m_vertices[i + 1].color = m_color;
			m_vertices[i + 2].color = m_color;
		}
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection)
{
	SetRectSelection(selectionRect, false, clearPreviousSelection);
}


void CMeshShape::SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection)
{
	SetRectSelection(selectionRect, true, clearPreviousSelection);
}


void CMeshShape::ClearSelection()
{
	if (m_selectedVerticesIndicies.empty()){
		return;
	}

	for (int i = 0; i < m_vertices.size(); ++i){
		m_vertices[i].color = m_color;
	}

	m_selectedVerticesIndicies.clear();

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::AllSelection()
{
	for (int i = 0; i < m_vertices.size(); ++i){
		m_selectedVerticesIndicies.insert(i);

		m_vertices[i].color = s_selectionColor;
	}

	UploadGeometry(false, m_vertices, m_vertexBuffer);
}


void CMeshShape::InvertSelection()
{
	for (int i = 0; i < m_vertices.size(); ++i){
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
	functions.glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (GLuint*)0);
}


// reimplemented (imt3dview::IDrawable)

void CMeshShape::Draw(QPainter& painter)
{
	imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());
	if (!meshPtr){
		return;
	}

	QString text = QString("<b><p>Total vertices: %1</p>").arg(meshPtr->GetPointsCount());
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

void CMeshShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection)
{
	if (!selectionRect.isValid() || m_vertices.isEmpty()){
		return;
	}

	for (int i = 0; i < m_vertices.size(); ++i){
		QPoint windowPosition = ModelToWindow(m_vertices[i].position);

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
	int meshSize = mesh.GetPointsCount();
	const imt3d::IMesh3d::Indices& indices = mesh.GetIndices();

	m_vertices.clear();
	m_vertices.reserve(meshSize);

	if (meshSize <= 0 || indices.empty()){
		return;
	}

	// update vertices and normals
	for (int i = 0; i < meshSize; ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(mesh.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		float positionX = static_cast<float>(pointDataPtr->data[0]);
		float positionY = static_cast<float>(pointDataPtr->data[1]);
		float positionZ = static_cast<float>(pointDataPtr->data[2]);

		float normalX = static_cast<float>(pointDataPtr->data[4]);
		float normalY = static_cast<float>(pointDataPtr->data[5]);
		float normalZ = static_cast<float>(pointDataPtr->data[6]);

		m_vertices.push_back(Vertex(QVector3D(positionX, positionY, positionZ),
				QVector3D(normalX, normalY, normalZ),
				m_color));
	}

	// update indices
	m_indices.clear();
	m_indices.reserve(static_cast<int>(indices.size() * indices.front().size()));

	for (int i = 0; i < indices.size(); ++i){
		const std::vector<uint32_t>& index = indices[i];

		for (int j = 0; j < index.size(); ++j){
			m_indices.push_back(index[j]);
		}
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


bool CMeshShape::IsPointFaceIntersection(const QPoint& point, int& intersectedVertexIndex, QVector3D& intersectionPoint) const
{
	intersectedVertexIndex = -1;
	intersectionPoint = QVector3D();

	if (point.isNull() || m_vertices.isEmpty()){
		return false;
	}

	// Determine mesh faces intersected with the given 2D point.
	// Among all the intersected faces choose the closest one to the camera.
	// For that, calculate the distance between the projection of the click point on the near plane and each face.
	QVector3D rayOrigin = WindowToModel(point, 0.0);
	QVector3D rayDestination = WindowToModel(point, 1.0);
	QVector3D rayDirection = (rayDestination - rayOrigin).normalized();

	float minDistance = qInf();

	for (int i = 0; i < m_vertices.size(); i += 3){
		const QVector3D& position1 = m_vertices[i + 0].position;
		const QVector3D& position2 = m_vertices[i + 1].position;
		const QVector3D& position3 = m_vertices[i + 2].position;

		float distance = -1.0;
		QVector3D faceIntersectionPoint;

		if (IsRayFaceIntersection(rayOrigin, rayDirection, position1, position2, position3, distance, faceIntersectionPoint)){
			if (distance < minDistance){
				minDistance = distance;

				intersectedVertexIndex = i;
				intersectionPoint = faceIntersectionPoint;
			}
		}
	}

	return intersectedVertexIndex >= 0;
}


bool CMeshShape::IsRayFaceIntersection(
				const QVector3D& rayOrigin,
				const QVector3D& rayDirection,
				const QVector3D& trianglePoint1,
				const QVector3D& trianglePoint2,
				const QVector3D& trianglePoint3,
				float& distanceToTriangle,
				QVector3D& intersectionPoint)
{
	// Möller–Trumbore ray-triangle intersection algorithm implementation
	distanceToTriangle = -1.0;
	intersectionPoint = QVector3D();

	const float EPSILON = 0.0000001;

	QVector3D edge1 = trianglePoint2 - trianglePoint1;
	QVector3D edge2 = trianglePoint3 - trianglePoint1;
	QVector3D h = QVector3D::crossProduct(rayDirection, edge2);

	// check if ray is parallel to the triangle
	float a = QVector3D::dotProduct(edge1, h);
	if (a > -EPSILON && a < EPSILON){
		return false;
	}

	float f = 1.0 / a;

	QVector3D s = rayOrigin - trianglePoint1;

	float u = f * QVector3D::dotProduct(s, h);
	if (u < 0.0 || u > 1.0){
		return false;
	}

	QVector3D q = QVector3D::crossProduct(s, edge1);

	float v = f * QVector3D::dotProduct(rayDirection, q);
	if (v < 0.0 || u + v > 1.0){
		return false;
	}

	distanceToTriangle = f * QVector3D::dotProduct(edge2, q);

	if (distanceToTriangle > EPSILON && distanceToTriangle < 1 / EPSILON){
		intersectionPoint = rayOrigin + distanceToTriangle * rayDirection;

		return true; // there is ray intersection
	}
	else{
		return false; // line intersection but not a ray intersection
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


