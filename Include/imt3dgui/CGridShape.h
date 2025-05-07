#pragma once


// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/CPointCloud3d.h>


namespace imt3dgui
{


class CGridShape: public CShape3dBase
{
	typedef CShape3dBase BaseClass;

public:
	CGridShape();

	void SetGridValue(double gridValue);
	void SetCount(int count);
	void SetPlanePositionX(double position);
	void SetPlanePositionY(double position);
	void SetPlanePositionZ(double position);

protected:
	// reimplement (imt3dgui::CShape3dBase)
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	virtual QVector3D GetColor() const override { return QVector3D(0.5, 0.5, 0.5); }

private:
	double m_gridValue = 10;
	int m_count = 10;
	imt3d::CPointCloud3d m_data;
	bool m_doUpdate = false;

	std::optional<double> m_planePositionX;
	std::optional<double> m_planePositionY;
	std::optional<double> m_planePositionZ;
};


} // namespace imt3dgui


