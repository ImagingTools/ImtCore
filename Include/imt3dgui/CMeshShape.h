/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


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
class CMeshShape: public imt3dgui::CShape3dBase
{
	typedef imt3dgui::CShape3dBase BaseClass;

public:
	CMeshShape();

	void SetColor(const QVector3D& color);
	void SetPointSelection(const QPoint& selectionPoint, bool clearPreviousSelection);
	void SetBoxSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void SetCircleSelection(const QRect& selectionRect, bool clearPreviousSelection);
	void ClearSelection();
	void AllSelection();
	void InvertSelection();
	void DeleteSelection();
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
	typedef std::set<int> Indices;

	void SetRectSelection(const QRect& selectionRect, bool isCircle, bool clearPreviousSelection);
	template <typename PointType> void DeleteSelectionHelper(imt3d::IMesh3d& mesh);
	static bool IsPointWithin(const QPoint& point, const QRect& rect, bool isCircle);
	bool GetFacePointIntersection(const QPoint& point, Indices& intersectedIndicies) const;
	bool GetFaceRectIntersections(const QRect& rect, bool isCircle, Indices& intersectedIndicies) const;
	static bool IsFaceRayIntersection(
				const QVector3D& rayOrigin,
				const QVector3D& rayDirection,
				const QVector3D& trianglePoint1,
				const QVector3D& trianglePoint2,
				const QVector3D& trianglePoint3,
				float& distanceToTriangle,
				QVector3D& intersectionPoint);
	void SelectVertices(Indices& intersectedIndicies, bool clearPreviousSelection);

private:
	QVector3D m_color;
	Indices m_selectedIndicies;
	bool m_isInfoBoxEnabled;

	static const QVector3D s_selectionColor;
};


} // namespace imt3dgui


