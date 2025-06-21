#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <GeneratedFiles/imtauthgui/ui_CServerConnectionEditorComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CServerConnectionEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CServerConnectionEditorComp, imtcom::IServerConnectionInterface>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CServerConnectionEditorComp, imtcom::IServerConnectionInterface> BaseClass;

	I_BEGIN_COMPONENT(CServerConnectionEditorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_HostEdit_editingFinished();
	void on_HttpPortEdit_editingFinished();
	void on_WebsocketPortEdit_editingFinished();

};


} // namespace imtauthgui


