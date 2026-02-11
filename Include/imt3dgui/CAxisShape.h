// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	QFont GetAxeLabelFont() const;

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

	static const float s_zoomFontFactor;
	bool m_doUpdate;
};


} // namespace imt3dgui


