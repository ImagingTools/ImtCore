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

	void SetAxisLength(double axisLength);
	void SetAxisRange(const istd::CRange& range);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IDrawable)
	void Draw(QPainter& painter) override;

private:
	QFont GetAxeLabelFont() const;

private:
	imt3d::CPointCloud3d m_data;
	double m_axisLength;
	istd::CRange m_axisRange;
	static const float s_zoomFontFactor;
	bool m_doUpdate;
};


} // namespace imt3dgui


