/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#include <imt3dgui/CMeshShape.h>


// Qt includes
#include <QtGui/QTextDocument>

// ACF includes
#include <imod/IModel.h>


namespace imt3dgui
{


// static members

const QVector3D CMeshShape::s_selectionColor(0.5, 0.0, 0.5);


// public methods

CMeshShape::CMeshShape()
	:m_color(QVector3D(0.0, 1.0, 1.0)),
	m_isInfoBoxEnabled(false)
{
}


void CMeshShape::SetColor(const QVector3D& color)
{
	m_color = color;
}


void CMeshShape::SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection)
{
	// get point-face intersection if any
	Indices intersectedIndicies;
	GetFacePointIntersection(selectionPoint, intersectedIndicies);

	// select/deselect vertices
	SelectVertices(intersectedIndicies, clearPreviousSelection);
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
	/*
	if (m_selectedIndicies.empty()){
		return;
	}

	m_selectedIndicies.clear();

	for (Vertex& vertex : m_vertices){
		vertex.color = m_color;
	}

	UpdateGeometry(m_vertices, m_vertexBuffer);
	*/
}


void CMeshShape::AllSelection()
{
	/*
	m_selectedIndicies.clear();

	for (int i = 0; i < m_indices.size(); ++i){
		m_selectedIndicies.insert(i);
	}

	for (Vertex& vertex : m_vertices){
		vertex.color = s_selectionColor;
	}

	UpdateGeometry(m_vertices, m_vertexBuffer);
	*/
}


void CMeshShape::InvertSelection()
{
	/*
	// select all vertices
	for (Vertex& vertex : m_vertices){
		vertex.color = s_selectionColor;
	}

	// invert selection
	for (int selectedIndex : m_selectedIndicies){
		int vertexIndex = m_indices[selectedIndex];

		m_vertices[vertexIndex].color = m_color;
	}

	for (int i = 0; i < m_indices.size(); ++i){
		if (m_selectedIndicies.find(i) == m_selectedIndicies.end()){
			m_selectedIndicies.insert(i);
		}
		else{
			m_selectedIndicies.erase(i);
		}
	}

	UpdateGeometry(m_vertices, m_vertexBuffer);
	*/
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
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormal32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwNormalRgba32>(*meshPtr);
		case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
			return DeleteSelectionHelper<imt3d::IPointsBasedObject::PointXyzwRgba32>(*meshPtr);
	}
}


void CMeshShape::SetInfoBoxEnabled(bool isEnabled)
{
	m_isInfoBoxEnabled = isEnabled;
}


// protected methods

// reimplement (imt3dgui::CShape3dBase)

void CMeshShape::UpdateShapeGeometry(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imt3d::IMesh3d* meshPtr = dynamic_cast<imt3d::IMesh3d*>(GetObservedModel());

	if (meshPtr == nullptr || meshPtr->IsEmpty()) {
		m_indices.clear();
		return;
	}

	m_pointsDataPtr = meshPtr;

	const imt3d::IMesh3d::Indices& indices = meshPtr->GetIndices();

	m_indices.clear();

	// update indices
	for (size_t i = 0; i < indices.size(); ++i) {
		const std::vector<uint32_t>& index = indices[i];

		for (size_t j = 0; j < index.size(); ++j) {
			m_indices.push_back(index[j]);
		}
	}
}


void CMeshShape::DrawShapeGl(QOpenGLShaderProgram& /*program*/, QOpenGLFunctions& functions)
{
	functions.glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (GLuint*)0);
}


// reimplemented (imt3dview::IDrawable)

void CMeshShape::Draw(QPainter& painter)
{
	if (m_isInfoBoxEnabled && m_pointsDataPtr != nullptr){
		QString text = QString("<b><p>Total vertices: %1</p>").arg(m_pointsDataPtr->GetPointsCount());
		text += QString("<p>Total faces: %1</p>").arg(m_indices.size() / 3);
		text += QString("<p>Selected faces: %1</p></b>").arg(static_cast<int>(m_selectedIndicies.size() / 3));

		painter.save();

		painter.setBrush(QBrush(QColor(240, 240, 240)));
		painter.drawRoundedRect(10, 10, 300, 90, 3.0, 3.0);
		painter.translate(15.0, 15.0);

		QTextDocument doc;
		doc.setHtml(text);
		doc.drawContents(&painter);

		painter.restore();
	}
}


QVector3D CMeshShape::GetColor() const
{
	return m_color;
}


// private methods

void CMeshShape::SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection)
{
	// get rect-face intersections if any
	Indices intersectedIndicies;
	GetFaceRectIntersections(selectionRect, isCircle, intersectedIndicies);

	// select/deselect vertices
	SelectVertices(intersectedIndicies, clearPreviousSelection);
}


template <typename PointType>
void CMeshShape::DeleteSelectionHelper(imt3d::IMesh3d& mesh)
{
	if (m_selectedIndicies.empty()){
		return;
	}

	// prepare vertex indices
	Indices selectedVertexIndices;
	for (int selectedIndex : m_selectedIndicies){
		selectedVertexIndices.insert(m_indices[selectedIndex]);
	}

	// find remaining points count
	int newPointsCount = mesh.GetPointsCount() - static_cast<int>(selectedVertexIndices.size());
	int newIndicesCount = static_cast<int>(mesh.GetIndices().size() - m_selectedIndicies.size() / 3);

	if (newPointsCount <= 0 || newIndicesCount <= 0){
		return; // can't create empty mesh due to architectural reasons
	}

	// initialize new mesh indices
	imt3d::IMesh3d::Indices newIndices;
	newIndices.resize(newIndicesCount);
	for (std::vector<uint32_t>& v : newIndices){
		v.resize(3);
	}

	// prepare new mesh data: copy not selected vertices reordering them accordingly to the selected indices
	PointType* newPointsDataPtr = new PointType[newPointsCount];
	PointType* ptr = newPointsDataPtr;

	QMap<int, int> vertexIndicesMap; // old vertex index - new vertex index
	int currentIndexNum = 0;

	for (int i = 0; i < m_indices.size(); ++i){
		if (m_selectedIndicies.find(i) == m_selectedIndicies.end()){
			int oldVertexIndex = m_indices[i];

			QMap<int, int>::ConstIterator it = vertexIndicesMap.constFind(oldVertexIndex);

			if (it == vertexIndicesMap.constEnd()){
				const PointType* pointDataPtr = static_cast<const PointType*>(mesh.GetPointData(oldVertexIndex));
				memcpy(ptr++, pointDataPtr, sizeof(PointType));

				int newVertexIndex = vertexIndicesMap.size();

				vertexIndicesMap.insert(oldVertexIndex, newVertexIndex);

				newIndices[currentIndexNum / 3][currentIndexNum % 3] = newVertexIndex;
			}
			else{
				int newVertexIndex = *it;
				newIndices[currentIndexNum / 3][currentIndexNum % 3] = newVertexIndex;
			}

			currentIndexNum++;
		}
	}

	m_selectedIndicies.clear();

	// create new mesh
	mesh.CreateMesh(mesh.GetPointFormat(), newPointsCount, newPointsDataPtr, newIndices);
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


bool CMeshShape::GetFacePointIntersection(const QPoint& point, Indices& intersectedIndicies) const
{
	/*
	intersectedIndicies.clear();

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
	int intersectedFaceIndex = -1;

	for (int i = 0; i < m_indices.size(); i += 3){
		const QVector3D& position1 = m_vertices[i + 0].position;
		const QVector3D& position2 = m_vertices[i + 1].position;
		const QVector3D& position3 = m_vertices[i + 2].position;

		float distance = -1.0;
		QVector3D faceIntersectionPoint;

		if (IsFaceRayIntersection(rayOrigin, rayDirection, position1, position2, position3, distance, faceIntersectionPoint)){
			if (distance < minDistance){
				minDistance = distance;
				intersectedFaceIndex = i;
			}
		}
	}

	if (intersectedFaceIndex >= 0){
		intersectedIndicies.insert(intersectedFaceIndex + 0);
		intersectedIndicies.insert(intersectedFaceIndex + 1);
		intersectedIndicies.insert(intersectedFaceIndex + 2);
	}

	return !intersectedIndicies.empty();
	*/

	return false;
}


bool CMeshShape::GetFaceRectIntersections(const QRect& rect, bool isCircle, Indices& intersectedIndicies) const
{
	/*
	intersectedIndicies.clear();

	if (rect.isNull() || m_vertices.isEmpty()){
		return false;
	}

	for (int i = 0; i < m_indices.size(); i += 3){
		const QVector3D& pos3d1 = m_vertices[i + 0].position;
		const QVector3D& pos3d2 = m_vertices[i + 1].position;
		const QVector3D& pos3d3 = m_vertices[i + 2].position;

		QPoint pos2d1 = ModelToWindow(pos3d1);
		QPoint pos2d2 = ModelToWindow(pos3d2);
		QPoint pos2d3 = ModelToWindow(pos3d3);

		if (IsPointWithin(pos2d1, rect, isCircle) &&
			IsPointWithin(pos2d2, rect, isCircle) &&
			IsPointWithin(pos2d3, rect, isCircle)){
			intersectedIndicies.insert(i + 0);
			intersectedIndicies.insert(i + 1);
			intersectedIndicies.insert(i + 2);
		}
	}

	return !intersectedIndicies.empty();
	*/
	return false;
}


bool CMeshShape::IsFaceRayIntersection(
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


void CMeshShape::SelectVertices(Indices& intersectedIndicies, bool clearPreviousSelection)
{
	/*
	// update selected indices
	if (clearPreviousSelection){
		m_selectedIndicies.swap(intersectedIndicies);
	}
	else{
		m_selectedIndicies.insert(intersectedIndicies.begin(), intersectedIndicies.end());
	}

	// deselect all vertices
	for (Vertex& vertex : m_vertices){
		vertex.color = m_color;
	}

	// select intersected vertices
	for (int selectedIndex : m_selectedIndicies){
		int vertexIndex = m_indices[selectedIndex];

		m_vertices[vertexIndex].color = s_selectionColor;
	}

	// upload new geometry
	UpdateGeometry(m_vertices, m_vertexBuffer);
	*/
}


} // namespace imt3dgui


