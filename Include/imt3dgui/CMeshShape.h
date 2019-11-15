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
	void SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection, const QRect& viewPort);
	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
	void BoxFromSelection();

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplement (imt3dgui::IShape3d)
	ColorMode GetColorMode() const;
	QVector3D GetColor() const;
	bool HasNormals() const override;

private:
	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort);
	template <typename PointType> void UpdateShapeGeometryHelper(const imt3d::IMesh3d& mesh);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IMesh3d& mesh);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);
	static bool IsRayFaceIntersection(
				const QVector3D& rayOrigin,
				const QVector3D& rayDirection,
				const QVector3D& trianglePoint1,
				const QVector3D& trianglePoint2,
				const QVector3D& trianglePoint3,
				float& distanceToTriangle);
	bool CalculateSelectionBox(istd::TRange<float>& xRange, istd::TRange<float>& yRange, istd::TRange<float>& zRange);
	void SetSelectionCubeFaces(const istd::TRange<float>& xRange, const istd::TRange<float>& yRange, const istd::TRange<float>& zRange);
	void SetSelectionCubeFaceNormals();
	void SetSelectionCubeEdges(const istd::TRange<float>& xRange, const istd::TRange<float>& yRange, const istd::TRange<float>& zRange);
	void SetSelectionCubeEdgeColors();
	void SelectFaceVertices();

private:
	typedef std::set<int> VertexIndicies;

	QVector3D m_color;
	int m_meshSize;
	QVector3D m_cubeSelectionSize;
	VertexIndicies m_selectedVerticesIndicies;

	static const QVector3D s_selectionColor;
	static const QVector3D s_selectionCubeColor;
	static const QVector3D s_selectionCubeXColor;
	static const QVector3D s_selectionCubeYColor;
	static const QVector3D s_selectionCubeZColor;
	static const int s_selectionCubeFacesSize;
	static const int s_selectionCubeEdgesSize;
};


} // namespace imt3dgui


