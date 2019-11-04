#pragma once


// Qt includes
#include <QtCore/QMutex>

// ImtCore includes
#include <imtgui3d/CShape3dBase.h>
#include <imt3d/IMesh3d.h>


namespace imtgui3d
{


/**
	Common mesh shape implementation in 3D space.
*/
class CMeshShape : public imtgui3d::CShape3dBase
{
	typedef imtgui3d::CShape3dBase BaseClass;

public:
	CMeshShape();
	~CMeshShape();

protected:
	// reimplement (imtgui3d::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (imtgui3d::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_ITEM; }
	QVector3D GetColor() const { return QVector3D(0.0, 1.0, 1.0); }
	bool HasNormals() const override;

private:
	template <typename PointType> void TUpdateShapeGeometry(const imt3d::IMesh3d& mesh);
};


} // namespace imtgui3d


