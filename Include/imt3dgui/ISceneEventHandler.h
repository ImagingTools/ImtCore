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

// Qt includes
#include <QtCore/QRect>
#include <QtGui/QMouseEvent>


namespace imt3dgui {

/**
	Scene event handler Interface
*/
class ISceneEventHandler
{
public:
	virtual void OnShowGrid(bool show) = 0;
	virtual void OnShowAxis(bool show) = 0;
	virtual void OnShowRuler(bool show) = 0;
	virtual void OnPointSelection(const QPoint& point, bool clearPreviousSelection) = 0;
	virtual void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) = 0;
	virtual void OnClearSelection() = 0;
	virtual void OnAllSelection() = 0;
	virtual void OnInvertSelection() = 0;
	virtual void OnDeleteSelection() = 0;
	virtual bool OnMousePress(QMouseEvent& e) = 0;
	virtual bool OnMouseMove(QMouseEvent& e) = 0;
	virtual bool OnMouseRelease(QMouseEvent& e) = 0;
};

} //namespace imt3dgui
