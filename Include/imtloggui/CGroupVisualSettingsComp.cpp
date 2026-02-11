// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtloggui/CGroupVisualSettingsComp.h>

// Qt includes
#include <QtGui/QColor>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IGroupVisualSettings)

QColor CGroupVisualSettingsComp::GetBackgroundColor() const
{
	if (m_colorAttrPtr.IsValid()){
		return QColor(*m_colorAttrPtr);
	}

	return QColor(Qt::transparent);
}


int CGroupVisualSettingsComp::GetHeight() const
{
	if (m_colorAttrPtr.IsValid()){
		return *m_heightAttrPtr;
	}

	return 200;
}


} // namespace imtloggui


