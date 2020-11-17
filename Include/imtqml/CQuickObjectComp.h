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


#ifndef imtqml_CQuickObjectComp_included
#define imtqml_CQuickObjectComp_included


// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuick/QQuickItem>

// ACF includes
//#include <istd/IPolymorphic.h>
#include <icomp/CComponentBase.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Common interface for GUI objects using in component context.
 */
class CQuickObjectComp:
        public icomp::CComponentBase,
        virtual public imtqml::IQuickObject
{
public:
    typedef icomp::CComponentBase BaseClass;
    I_BEGIN_COMPONENT(CQuickObjectComp);
        I_REGISTER_INTERFACE(imtqml::IQuickObject);
        I_ASSIGN(m_pathToQmlPtr, "PathToQmlPtr", "If enabled, this path used for load Qml", true, "");
    I_END_COMPONENT;

    CQuickObjectComp();

    // reimplemented (imtgui::IQuickObject)
    virtual bool IsItemCreated() const;
    virtual bool CreateItem(QQuickItem* parentPtr);
    virtual bool CreateItem(QQmlEngine* engine);
    virtual bool DestroyItem();
    virtual QQuickItem* GetItem() const;
    virtual void OnTryClose(bool* ignoredPtr = NULL);

    // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated();

protected:
    I_ATTR(QString, m_pathToQmlPtr);

    QQuickItem *m_quickItem;
};


} // namespace imtqml


#endif // !imtqml_CQuickObjectComp_included


