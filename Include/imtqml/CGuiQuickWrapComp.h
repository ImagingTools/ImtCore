#pragma once


// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QVBoxLayout>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iqtgui/IVisualStatus.h>
#include <iqtgui/IGuiObject.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iqtgui/TMakeIconProviderCompWrap.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Common interface for QML objects using in component context.
*/
class CGuiQuickWrapComp:
			public iqtgui::TMakeIconProviderCompWrap<icomp::CComponentBase>,
			virtual public iqtgui::IGuiObject
{
public:
	typedef iqtgui::TMakeIconProviderCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGuiQuickWrapComp);
		I_REGISTER_INTERFACE(iqtgui::IGuiObject);
		I_REGISTER_SUBELEMENT(VisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, iqtgui::IVisualStatus, ExtractVisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, istd::IChangeable, ExtractVisualStatus);
		I_REGISTER_SUBELEMENT_INTERFACE_T(VisualStatus, imod::IModel, ExtractVisualStatus);
		I_ASSIGN(m_quickCompPtr, "QuickObject", "Quick", false, "QuickObject");
		I_ASSIGN(m_defaultStatusIconPathAttrPtr, "DefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN(m_defaultStatusTextAttrPtr, "DefaultStatusText", "Status text used by default", true, "");
	I_END_COMPONENT;

	CGuiQuickWrapComp();

	// reimplemented (iqtgui::IGuiObject)
	virtual bool IsGuiCreated() const override;
	virtual bool CreateGui(QWidget* parentPtr) override;
	virtual bool DestroyGui() override;
	virtual QWidget* GetWidget() const override;
	virtual void OnTryClose(bool* ignoredPtr = NULL) override;
	
protected:
	// reimplemented (ibase::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged(const QByteArray& themeId) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	class VisualStatus: virtual public iqtgui::IVisualStatus
	{
	public:
		// reimplemented (iqtgui::IVisualStatus)
		virtual QIcon GetStatusIcon() const;
		virtual QString GetStatusText() const;

		friend class CGuiQuickWrapComp;

	private:
		QIcon m_statusIcon;
		QString m_statusText;
	};

	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractVisualStatus(CGuiQuickWrapComp& component)
	{
		return &component.m_visualStatus;
	}

protected:
	I_REF(imtqml::IQuickObject, m_quickCompPtr);
	I_ATTR(QString, m_defaultStatusIconPathAttrPtr);
	I_TEXTATTR(m_defaultStatusTextAttrPtr);

	QQuickWidget* m_quickWidget;

	imod::TModelWrap<VisualStatus> m_visualStatus;
};


} // namespace imtqml


