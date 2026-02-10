// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <optional>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


class CGridShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	enum PlaneMode
	{
		PM_XY,
		PM_XZ,
		PM_YZ
	};

	CGridShape();

	void SetGridValue(double gridValue);
	void SetCount(int count);
	void SetPlanePosition(double position);
	void SetPlaneMode(PlaneMode planeMode);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	virtual QVector3D GetColor() const override { return QVector3D(0.5, 0.5, 0.5); }

private:
	typedef std::vector<imt3d::CPointCloud3d::PointXyz32> Vertices;
	void CreateXyPlane(Vertices& vertices, double planePosition) const;
	void CreateXzPlane(Vertices& vertices, double planePosition) const;
	void CreateYzPlane(Vertices& vertices, double planePosition) const;

private:
	double m_gridValue = 0.1;
	int m_count = 10;
	imt3d::CPointCloud3d m_data;
	bool m_doUpdate = false;
	PlaneMode m_planeMode = PM_XY;

	std::optional<double> m_planePosition;
};


} // namespace imt3dgui


