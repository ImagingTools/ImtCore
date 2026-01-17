#pragma once


// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


class CAxisShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CAxisShape();

	enum AxisType
	{
		AT_X,
		AT_Y,
		AT_Z
	};


	void SetAxisLength(AxisType axis, double axisLength);
	void SetAxisRange(AxisType axis, const istd::CRange& range);
	void SetAxisLineWidth(AxisType axis, double lineWidth);
	void SetAxisLabel(AxisType axis, const QString& label);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IDrawable)
	virtual void Draw(QPainter& painter) override;

private:
	// Creates 3D line geometry for a letter (X, Y, or Z) at the specified position
	// Unknown letters will not be rendered
	void AddLetterGeometry(std::vector<imt3d::CPointCloud3d::PointXyzwRgba32>& vertices, 
	                       QVector<GLuint>& indices,
	                       const QString& letter,
	                       const QVector3D& position,
	                       float size,
	                       const QVector3D& color);

private:
	imt3d::CPointCloud3d m_data;

	struct AxisConfig
	{
		double axisLength = 1.0;
		istd::CRange axisRange = istd::CRange(0.0, 1.0);
		double lineWidth = 2.0;
		QString label;
	};

	QMap<AxisType, AxisConfig> m_axisConfigs;

	bool m_doUpdate;
};


} // namespace imt3dgui


