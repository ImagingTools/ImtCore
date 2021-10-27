#pragma once


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
	void SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection);
	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
	void SetInfoBoxEnabled(bool isEnabled = true);

protected:
	// reimplemented (imt3dview::CShape3dBase)
	void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	QVector3D GetColor() const override;

private:
	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IPointCloud3d& pointCloud);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);

private:
	typedef std::set<int> SelectedVerticesIndicies;

	QVector3D m_color;
	float m_pointSize;
	SelectedVerticesIndicies m_selectedVerticesIndicies;
	bool m_isInfoBoxEnabled;

	static const QVector3D s_selectionColor;
};


} // namespace imt3dgui
