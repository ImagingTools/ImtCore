// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual void UpdateShapeGeometry(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void DrawShapeGl(QOpenGLShaderProgram& program, QOpenGLFunctions& functions) override;

	// reimplemented (imt3dview::IDrawable)
	virtual void Draw(QPainter& painter) override;

	// reimplemented (imt3dgui::IShape3d)
	virtual QVector3D GetColor() const override;

private:

private:
	QVector3D m_color;
	bool m_isInfoBoxEnabled;
};


} // namespace imt3dgui


