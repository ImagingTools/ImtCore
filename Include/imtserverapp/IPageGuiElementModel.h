// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


class IPageGuiElementModel: virtual public imtserverapp::IGuiElementModel
{
public:
	/**
		Get path to the QML file item describing this page.
	*/
	virtual QString GetPageQmlItemFilePath() const = 0;

	/**
		Get path to the document manager on this page.
	*/
	virtual QString GetStartSourceItem() const = 0;

	/**
		Get ID of the menu group this page belongs to.
		\return Empty ID if the page is not part of any group.
	*/
	virtual QByteArray GetGroupId() const = 0;
};


} // namespace imtserverapp


