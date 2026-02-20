// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CStandardPopupWidgetFactoryComp.h>


// ImtCore includes
#include <imtgui/CStandardPopupWidget.h>


namespace imtgui
{


// public methods

// reimplemented (istd::TIFactory)

istd::TUniqueInterfacePtr<IPopupWidget> CStandardPopupWidgetFactoryComp::CreateInstance(const QByteArray& keyId) const
{
	if (keyId.isEmpty()){
		return istd::TUniqueInterfacePtr<IPopupWidget>(new CStandardPopupWidget());
	}

	return istd::TUniqueInterfacePtr<IPopupWidget>();
}


// reimplemented (istd::IFactoryInfo)

istd::IFactoryInfo::KeyList CStandardPopupWidgetFactoryComp::GetFactoryKeys() const
{
	static QSet<QByteArray> keyList({QByteArray()});

	return keyList;
}


} // namespace imtgui


