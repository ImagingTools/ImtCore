#pragma once


// Qt includes
#include <QtGui/QOpenGLBuffer>

// STL includes
#include <set>

// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/IPointCloud3d.h>


namespace imt3dgui
{


/**
	Common point cloud implementation in 3D-space.
*/
class CPointCloudShape: public imt3dgui::CShape3dBase
{
	typedef imt3dgui::CShape3dBase BaseClass;

public:
	CPointCloudShape();

	void SetColor(const QVector3D& color);
	void SetPointSize(float pointSize);

	void SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection, const QRect& viewPort);
	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
	void BoxFromSelection();

protected:
	// reimplemented (imt3dview::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	ColorMode GetColorMode() const override;
	QVector3D GetColor() const override;

private:
	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort);
	template <typename PointType> void UpdateShapeGeometryHelper(const imt3d::IPointCloud3d& pointCloud);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IPointCloud3d& pointCloud);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);
	bool CalculateSelectionBox(istd::TRange<float>& xRange, istd::TRange<float>& yRange, istd::TRange<float>& zRange);
	void SetSelectionCubeFaces(const istd::TRange<float>& xRange, const istd::TRange<float>& yRange, const istd::TRange<float>& zRange);
	void SetSelectionCubeEdges(const istd::TRange<float>& xRange, const istd::TRange<float>& yRange, const istd::TRange<float>& zRange);
	void SetSelectionCubeEdgeColors();

private:
	QVector3D m_color;
	float m_pointSize;
	int m_pointCloudSize;
	QVector3D m_cubeSelectionSize;

	typedef std::set<int> SelectedVerticesIndicies;
	SelectedVerticesIndicies m_selectedVerticesIndicies;

	static const QVector3D s_selectionColor;
	static const QVector3D s_selectionCubeColor;
	static const QVector3D s_selectionCubeXColor;
	static const QVector3D s_selectionCubeYColor;
	static const QVector3D s_selectionCubeZColor;
	static const int s_selectionCubeFacesSize;
	static const int s_selectionCubeEdgesSize;
};


} // namespace imt3dgui
