#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// ACF includes
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>

// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/CEventGroupManager.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			virtual public ilog::IMessageConsumer,
			public iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_messageGroupInfoProviderCompPtr, "MessageGroupInfoProvider", "Message group info provider", false, "")
	I_END_COMPONENT;

	CEventViewComp();

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& messagePtr) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	QGraphicsScene* m_scenePtr;
	CEventGraphicsView* m_viewPtr;
	CTimeAxis* m_timeAxisPtr;
	CEventGroupManager* m_groupManagerPtr;

	I_REF(imtbase::IMessageGroupInfoProvider, m_messageGroupInfoProviderCompPtr);
};


} // namespace imtloggui


