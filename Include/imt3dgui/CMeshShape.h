#pragma once


// Qt includes
#include <QtCore/QMutex>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/IMesh3d.h>


namespace imt3dgui
{


/**
	Common mesh shape implementation in 3D space.
*/
class CMeshShape : public imt3dgui::CShape3dBase
{
	typedef imt3dgui::CShape3dBase BaseClass;

public:
	CMeshShape();
	~CMeshShape();

protected:
	// reimplement (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imt3dgui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_ITEM; }
	QVector3D GetColor() const { return QVector3D(0.0, 1.0, 1.0); }
	bool HasNormals() const override;

private:
	template <typename PointType> void TUpdateShapeGeometry(const imt3d::IMesh3d& mesh);
	void EnsureNormalExist(int pointIndex);
};


} // namespace imt3dgui


