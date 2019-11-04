#pragma once


// Qt includes
#include <QtCore/QMutex>

// eagle includes
#include <eaglegui/CShape3dBase.h>

// ImtCore includes
#include <imt3d/IMesh3d.h>


namespace eaglegui
{


/**
	Common mesh shape implementation in 3D space.
*/
class CMeshShape : public eaglegui::CShape3dBase
{
	typedef eaglegui::CShape3dBase BaseClass;

public:
	CMeshShape();
	~CMeshShape();

protected:
	// reimplement (eaglegui::CShape3dBase)
	void UpdateShapeGeometry() override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplement (eaglegui::IShape3d)
	ColorMode GetColorMode() const { return IShape3d::CM_ITEM; }
	QVector3D GetColor() const { return QVector3D(0.0, 1.0, 1.0); }
	bool HasNormals() const override;

private:
	template <typename PointType> void TUpdateShapeGeometry(const imt3d::IMesh3d& mesh);
};


} // namespace eaglegui


