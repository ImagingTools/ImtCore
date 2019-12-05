#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtgui
{


/**
	Common interface for visualization and editing constraints of a document view.
*/
class IDocumentViewConstraints: virtual public istd::IChangeable
{
public:
	enum ConstraintFlags
	{
		CF_SHOW_TITLE = 1,
		CF_EDIT_TITLE = 2,
		CF_SHOW_COMMENT = 4,
		CF_EDIT_COMMENT = 8,
		CF_EDIT_DOCUMENT = 16,
		CF_SAVE_DOCUMENT = 32,
		CF_CLOSE_SUPPORT = 64,
		CF_UNDO_SUPPORT = 128
	};

	/**
		Get view contsraints flags to describe the supported options for the visualization and editing the data.
	*/
	virtual int GetViewConstraints() const = 0;

	/**
		Set view contsraints flags.
		\sa GetViewConstraints
	*/
	virtual bool SetViewConstraints(int flags) = 0;
};


} // namespace imtgui


