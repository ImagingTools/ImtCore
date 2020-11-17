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


#ifndef imtqml_CGuiQuickWrapComp_included
#define imtqml_CGuiQuickWrapComp_included


// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QVBoxLayout>

// ACF includes
//#include <istd/IPolymorphic.h>
#include <icomp/CComponentBase.h>
#include <imtqml/IQuickObject.h>
#include <iqtgui/IGuiObject.h>


namespace imtqml
{


/**
    Common interface for QML objects using in component context.
 */
class CGuiQuickWrapComp:
        public icomp::CComponentBase,
        virtual public iqtgui::IGuiObject
{
public:
    typedef icomp::CComponentBase BaseClass;
    I_BEGIN_COMPONENT(CGuiQuickWrapComp);
        I_REGISTER_INTERFACE(iqtgui::IGuiObject);
        I_ASSIGN(m_quickCompPtr, "QuickObject", "Wrap to QuickObject", false, "QuickObject");
    I_END_COMPONENT;

    CGuiQuickWrapComp();

    // reimplemented (iqtgui::IGuiObject)
    virtual bool IsGuiCreated() const override;
    virtual bool CreateGui(QWidget* parentPtr) override;
    virtual bool DestroyGui() override;
    virtual QWidget* GetWidget() const override;
    virtual void OnTryClose(bool* ignoredPtr = NULL) override;

     // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated() override;

protected:
//    I_ATTR(QString, m_pathToQmlPtr);
    I_REF(imtqml::IQuickObject, m_quickCompPtr);

    QQuickWidget *m_quickWidget;
    QVBoxLayout *m_layout;
};


} // namespace imtqml


#endif // !imtqml_CGuiQuickWrapComp_included


