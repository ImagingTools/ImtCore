// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CPageGuiElementModelComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtqml::IPageElementContainer)

QString CPageGuiElementModelComp::GetStartSourceItem() const
{
    if (!m_startSourceItemAttrPtr.IsValid()){
        return QString();
    }

	return *m_startSourceItemAttrPtr;
}


QString CPageGuiElementModelComp::GetPageQmlItemFilePath() const
{
    if (!m_pageSourceItemAttrPtr.IsValid()){
        return QString();
    }

	return *m_pageSourceItemAttrPtr;
}


} // namespace imtqml


