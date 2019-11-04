#pragma once


// Qt includes
#include <QtGui/QOpenGLBuffer>

// STL includes
#include <set>

// eagle includes
#include <eaglegui/CShape3dBase.h>

// ImtCore includes
#include <imt3d/IPointCloud3d.h>


namespace eaglegui
{


/**
	Common point cloud implementation in 3D-space.
*/
class CPointCloudShape: public eaglegui::CShape3dBase
{
	typedef eaglegui::CShape3dBase BaseClass;

public:
	CPointCloudShape();

	void SetColor(const QVector3D& color);
	void SetPointSize(float pointSize);

	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection, const QRect& viewPort);
	void ClearSelection();
	void InvertSelection();
	void DeleteSelection();

protected:
	// reimplemented (eagle::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (eaglegui::IShape3d)
	ColorMode GetColorMode() const override;
	QVector3D GetColor() const override;

private:
	/**
		Performs 3D picking - searches for a vertex closest to a point in 2D window.
		Returns found vertex index or -1 if not found.
	*/
	int SelectVertex(const QPoint& windowPoint, const QRect& viewPort) const;
	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection, const QRect& viewPort);
	template <typename PointType> void UpdateShapeGeometryHelper(const imt3d::IPointCloud3d& pointCloud);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IPointCloud3d& pointCloud);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);

private:
	QVector3D m_color;
	const QVector3D m_selectionColor;
	float m_pointSize;

	typedef std::set<int> SelectedVerticesIndicies;
	SelectedVerticesIndicies m_selectedVerticesIndicies;

	std::vector<int> m_pointCloudIndicies;
};


} // namespace eaglegui
