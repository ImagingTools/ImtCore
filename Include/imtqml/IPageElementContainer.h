#pragma once


// ImtCore includes
#include <imtgui/IGuiElementContainer.h>


namespace imtqml
{


class IPageElementContainer: virtual public imtgui::IGuiElementContainer
{
public:
	/**
		Get path to the item which first start on this page.
	*/
	virtual QString GetStartSourceItem(const QByteArray& elementId) const = 0;

	/**
		Get path to the document manager on this page.
	*/
	virtual QString GetPageSourceItem(const QByteArray& elementId) const = 0;
};


} // namespace imtgui


