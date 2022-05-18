#pragma once


// Qt includes
#include <QtWidgets/QSystemTrayIcon>

// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iprm/IEnableableParam.h>
#include <ilog/IMessageConsumer.h>
#include <ibase/ICommandsProvider.h>
#include <icomp/IComponent.h>

// ImtCore includes
#include <imtqml/CQuickApplicationCompBase.h>
#include <imtqml/IQuickObject.h>
#include <imtqml/IQuickApplication.h>


namespace imtqml
{


class CQuickApplicationComp:
			public QObject,
			public CQuickApplicationCompBase,
			protected imod::TSingleModelObserverBase<iprm::IEnableableParam>
{
	Q_OBJECT
public:
	typedef imtqml::CQuickApplicationCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<iprm::IEnableableParam> BaseClass2;

	I_BEGIN_COMPONENT(CQuickApplicationComp);
		I_REGISTER_INTERFACE(ibase::IApplication);
		I_REGISTER_SUBELEMENT(TrayMessages);
		I_REGISTER_SUBELEMENT_INTERFACE(TrayMessages, ilog::IMessageConsumer, ExtractTrayMessages);
		I_ASSIGN(m_mainQuickCompPtr, "MainQuickWindow", "QuickWindow object shown as main window", false, "MainGui");
		I_ASSIGN(m_allowApplicationCloseCompPtr, "AllowClose", "Controls closing of the application", false, "AllowClose");
		I_ASSIGN_TO(m_allowApplicationCloseModelCompPtr, m_allowApplicationCloseCompPtr, false);
		I_ASSIGN(m_trayIconCommandsCompPtr, "TrayIconsCommands", "Provider of tray icons commands. The commands are shown in the menu of the system tray", false, "TrayIconsCommands");
		I_ASSIGN(m_frameSpaceSizeAttrPtr, "FrameSpaceSize", "Number of pixels will be added on the all window sides", false, 9);
		I_ASSIGN(m_uiStartModeAttrPtr, "UiStartMode", "UI mode by application start up.\n0 - normal\n1 - full screen\n2 - minimized\n3 - maximized", false, 0);
		I_ASSIGN(m_useMenuIconsOnMacAttrPtr, "UseMenuIconsOnMac", "If enabled, the menu icons will be used in the application's menu bar on Mac OS", true, false);
		I_ASSIGN(m_useTrayIconAttrPtr, "UseTrayIcon", "If enabled, the tray icon for the application will be used", true, false);
		I_ASSIGN(m_useFullScreenBorderOnWindowsAttrPtr, "UseFullScreenBorderOnWindows", "If enabled, 1-pixel border will be created by Windows in full screen mode", true, false);
		I_ASSIGN(m_contextCompPtr, "Context", "Context", false, "Context");
	I_END_COMPONENT;

	CQuickApplicationComp();

	// reimplemented (ibase::IGuiApplication)
	virtual const imtqml::IQuickObject* GetApplicationItem() const;

	// reimplemented (ibase::IApplication)
	virtual bool InitializeApplication(int argc, char** argv);
	virtual int Execute(int argc, char** argv);
	virtual QString GetHelpText() const;

protected:
	// reimplemented (imod::TSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed();

protected:
	template <class InterfaceType>
	static InterfaceType* ExtractTrayMessages(CQuickApplicationComp& component)
	{
		return &component.m_trayMessages;
	}

	class TrayMessages: virtual public ilog::IMessageConsumer
	{
	public:
		explicit TrayMessages(CQuickApplicationComp& parent);

		// reimplemented (ilog::IMessageConsumer)
		virtual bool IsMessageSupported(
					int messageCategory = -1,
					int messageId = -1,
					const istd::IInformationProvider* messagePtr = NULL) const;
		virtual void AddMessage(const MessagePtr& messagePtr);

	private:
		CQuickApplicationComp& m_parent;
	};

private:
	void UpdateMainWidgetDecorations();
	void ShowWindow();

private Q_SLOTS:
	void OnQuit();
	void OnEventLoopStarted();

protected:
	I_REF(imtqml::IQuickObject, m_mainQuickCompPtr);
	I_REF(iprm::IEnableableParam, m_allowApplicationCloseCompPtr);
	I_REF(imod::IModel, m_allowApplicationCloseModelCompPtr);
	I_REF(ibase::ICommandsProvider, m_trayIconCommandsCompPtr);
	I_REF(icomp::IComponent, m_contextCompPtr);
	I_ATTR(int, m_frameSpaceSizeAttrPtr);
	I_ATTR(int, m_uiStartModeAttrPtr);
	I_ATTR(bool, m_useMenuIconsOnMacAttrPtr);
	I_ATTR(bool, m_useTrayIconAttrPtr);

	/**
		If enabled, the 1-pixel border will be created for in the full screen mode.
		It can be necessary for the OpenGL-based applications due to a limitation in Windows OS.
	*/
	I_ATTR(bool, m_useFullScreenBorderOnWindowsAttrPtr);

	Qt::WindowFlags m_defaultWidgetFlags;
	QRect m_lastWidgetGeometry;

	TrayMessages m_trayMessages;
	
	istd::TDelPtr<QSystemTrayIcon> m_trayIconPtr;
};


} // namespace imtqml


