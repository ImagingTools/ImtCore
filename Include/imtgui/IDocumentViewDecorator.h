#pragma once


// Qt includes
#include <QtWidgets/QWidget>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtgui
{


/**
	Interface for the document view decorator.
	View decorator is used for some extended (decorated) presentation of a document view.
*/
class IDocumentViewDecorator: virtual public istd::IPolymorphic
{
public:
	/**
		Get access to the main (root) widget of the decorator.
	*/
	virtual QWidget* GetDecoratorWidget() = 0;

	/**
		Get access to the view frame. The view frame is the container widget for the view component.
	*/
	virtual QWidget* GetViewFrame() = 0;

	/**
		Get access to the view object itself.
	*/
	virtual istd::IPolymorphic* GetView() const = 0;

	/**
		Set the view frame enabled or disabled.
	*/
	virtual void SetViewEnabled(bool isEnabled) = 0;

	/**
		Get the title of the document.
	*/
	virtual QString GetTitle() = 0;

	/**
		Set the title of the document.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Set the type name of the document.
	*/
	virtual void SetDocumentTypeName(const QString& name) = 0;
};


} // namespace imtgui


