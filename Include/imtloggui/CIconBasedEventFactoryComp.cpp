// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtloggui/CIconBasedEventFactoryComp.h>


// Qt includes
#include <QtGui/QIcon>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CIconBasedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& messagePtr) const
{
	if (!IsSupportedMessageId(messagePtr->GetInformationId())){
		return CreateInstanceWithSlaveFactory(messagePtr);
	}

	int iconSize = 24;

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	if (m_icon.isNull()){
		if (m_iconAttrPtr.IsValid()){
			m_icon = QIcon(*m_iconAttrPtr).pixmap(iconSize);
		}
	}

	CIconBasedEventItem* itemPtr = new imod::TModelWrap<CIconBasedEventItem>();
	itemPtr->SetParams(m_icon, messagePtr);

	return itemPtr;
}


} // namespace imtloggui


