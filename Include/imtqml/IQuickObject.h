/********************************************************************************
**
**	Copyright (C) 2007-2017 Witold Gantzke & Kirill Lepskiy
**
**	This file is part of the ACF Toolkit.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
** 	See http://www.ilena.org or write info@imagingtools.de for further
** 	information about the ACF.
**
********************************************************************************/


#ifndef imtqml_IQuickObject_included
#define imtqml_IQuickObject_included


// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuick/QQuickItem>

// ACF includes
#include <istd/IPolymorphic.h>
//#include <iqtgui/iqtgui.h>


namespace imtqml
{


/**
    Common interface for QML objects using in component context.
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
    virtual bool CreateItem(QQuickItem* parentPtr) = 0;
    /**
        Initialize GUI root Item
        \return		true if initialization successed.
     */
    virtual bool CreateItem(QQmlEngine* engine) = 0;
    /**
		Release GUI and disconnect it from parent.
		\return		true if this operation successed.
	 */
    virtual bool DestroyItem() = 0;
	/**
        Get access to internal QQuickItem object.
	*/
    virtual QQuickItem* GetItem() const = 0;
	/**
		Called on trying to close application.
		\param	ignoredPtr		if it is not NULL, ignoring of close is allowed.
								In this case implementation should set pointed value to true.
	 */
	virtual void OnTryClose(bool* ignoredPtr = NULL) = 0;
};


} // namespace imtqml


#endif // !imtqml_IQuickObject_included


