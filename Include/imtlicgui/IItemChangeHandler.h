#pragma once


// Qt includes
#include <QtCore/QVariant>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlicgui
{


class IItemChangeHandler: virtual public istd::IPolymorphic
{
public:
	enum ChangeId
	{
		CI_ITEM_ID = 0,
		CI_ITEM_NAME,
		CI_ITEM_STATUS,
		CI_ITEM_ACTIVATION_ENABLED,
		CI_ITEM_ACTIVATED,
		CI_ITEM_ENABLED
	};

	virtual void OnItemChanged(const QByteArray& itemId, ChangeId changeId, QVariantList params = QVariantList()) = 0;
};


} // namespace imtlicgui


