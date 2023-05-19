#pragma once


// STL includes
#include <set>

// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3dgui/CShape3dBase.h>
#include <imt3d/IMesh3d.h>


namespace imt3dgui
{


/**
	Common mesh shape implementation in 3D space.
*/
class CWireShape : public imt3dgui::CShape3dBase
{
	typedef imt3dgui::CShape3dBase BaseClass;

public:
	CWireShape();

	void SetColor(const QVector3D& color);
	void SetInfoBoxEnabled(bool isEnabled = true);

protected:
	// reimplemented (imt3dgui::CShape3dBase)
	void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	QVector3D GetColor() const;

private:

private:
	QVector3D m_color;
	bool m_isInfoBoxEnabled;
};


} // namespace imt3dgui


