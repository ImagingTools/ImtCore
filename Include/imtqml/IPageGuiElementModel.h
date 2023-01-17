#pragma once


// ImtCore includes
#include <imtgui/IGuiElementModel.h>


namespace imtqml
{


class IPageGuiElementModel: virtual public imtgui::IGuiElementModel
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
};


} // namespace imtqml


