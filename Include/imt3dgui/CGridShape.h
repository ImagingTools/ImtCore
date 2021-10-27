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

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	QVector3D GetColor() const { return QVector3D(0.5, 0.5, 0.5); }

private:
	double m_gridValue;
	int m_count;
	imt3d::CPointCloud3d m_data;
	bool m_doUpdate;
};


} // namespace imt3dgui


