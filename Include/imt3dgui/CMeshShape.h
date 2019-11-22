#pragma once


// STL includes
#include <set>

// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/IMesh3d.h>


namespace imt3dgui
{


/**
	Common mesh shape implementation in 3D space.
*/
class CMeshShape: public imt3dgui::CShape3dBase
{
	typedef imt3dgui::CShape3dBase BaseClass;

public:
	CMeshShape();

	void SetColor(const QVector3D& color);
	void SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection);
	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
	float CalculateRulerLength(const QLine& rulerLine) const;

protected:
	// reimplemented (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	ColorMode GetColorMode() const;
	QVector3D GetColor() const;
	bool HasNormals() const override;

private:
	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection);
	template <typename PointType> void UpdateShapeGeometryHelper(const imt3d::IMesh3d& mesh);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IMesh3d& mesh);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);
	bool IsPointFaceIntersection(const QPoint& point, int& intersectedVertexIndex, QVector3D& intersectionPoint) const;
	static bool IsRayFaceIntersection(
				const QVector3D& rayOrigin,
				const QVector3D& rayDirection,
				const QVector3D& trianglePoint1,
				const QVector3D& trianglePoint2,
				const QVector3D& trianglePoint3,
				float& distanceToTriangle,
				QVector3D& intersectionPoint);
	void SelectFaceVertices();

private:
	QVector3D m_color;
	typedef std::set<int> VertexIndicies;
	VertexIndicies m_selectedVerticesIndicies;

	static const QVector3D s_selectionColor;
};


} // namespace imt3dgui


