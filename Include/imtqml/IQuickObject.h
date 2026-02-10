// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtQuick/QQuickItem>


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtqml
{


/**
	Common interface for QML objects.
 */
class IQuickObject: virtual public istd::IPolymorphic
{
public:
	/**
		Inform if GUI was initilized.
	 */
	virtual bool IsItemCreated() const = 0;

	/**
		Initialize GUI and connect it to the parent.
		\return		true if initialization successed.
	 */
	virtual bool CreateQuickItem(QQuickItem* parentPtr) = 0;

	/**
		Release GUI and disconnect it from parent.
		\return		true if this operation successed.
	 */
	virtual bool DestroyQuickItem() = 0;

	/**
		Get access to internal QQuickItem object.
	*/
	virtual QQuickItem* GetQuickItem() const = 0;

	/**
		Called on trying to close application.
		\param	ignoredPtr		if it is not NULL, ignoring of close is allowed.
								In this case implementation should set pointed value to true.
	 */
	virtual void OnTryClose(bool* ignoredPtr = nullptr) = 0;
};


} // namespace imtqml


