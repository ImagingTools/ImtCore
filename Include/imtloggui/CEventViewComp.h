#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// ACF includes
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CEventGroupManager.h>
#include <imtloggui/CTimeAxis.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			public iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_messageGroupInfoProviderCompPtr, "MessageGroupInfoProvider", "Message group info provider", false, "")
	I_END_COMPONENT;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& message) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

Q_SIGNALS:
	void AxisPositionChanged();

private:
	//QDateTime m_startTime;
	//QDateTime m_endTime;
	QGraphicsScene* m_scenePtr;
	CEventGraphicsView* m_viewPtr;
	CTimeAxis* m_timeAxisPtr;
	CEventGroupManager* m_groupManagerPtr;

	I_REF(imtbase::IMessageGroupInfoProvider, m_messageGroupInfoProviderCompPtr);
};


} // namespace imtloggui


