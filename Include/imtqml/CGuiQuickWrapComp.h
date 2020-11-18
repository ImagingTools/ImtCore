#pragma once


// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QVBoxLayout>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imtqml/IQuickObject.h>
#include <iqtgui/IGuiObject.h>
#include <iqtgui/TGuiComponentBase.h>



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

protected:
	I_REF(imtqml::IQuickObject, m_quickCompPtr);

	QQuickWidget *m_quickWidget;
};


} // namespace imtqml


