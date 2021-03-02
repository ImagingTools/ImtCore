#pragma once


// Qt includes
#include <QtWidgets/QLabel>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtloggui/IEventStatisticsProvider.h>
#include <GeneratedFiles/imtloggui/ui_CEventStatisticsViewComp.h>


namespace imtloggui
{


class CEventStatisticsViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CEventStatisticsViewComp,
						IEventStatisticsProvider>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CEventStatisticsViewComp,
				IEventStatisticsProvider> BaseClass;
	
	I_BEGIN_COMPONENT(CEventStatisticsViewComp)
	I_END_COMPONENT

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

private:
	int GetGroupIndex(const QByteArray& id);

private:
	class GroupStatisticsWidget: public QWidget
	{
	public:
		GroupStatisticsWidget(QWidget *parentPtr = nullptr);

		void SetName(const QString& name);
		void SetCounters(
					qint64 noneCouner,
					qint64 infoCouner,
					qint64 warningCouner,
					qint64 errorCouner,
					qint64 criticalCouner);

	private:
		QLabel* m_name;
		QLabel* m_counters[5];
	};

	struct GroupItem
	{
		QByteArray id;
		GroupStatisticsWidget* widgetPtr;
	};


	typedef QList<GroupItem> GroupList;

private:
	GroupList m_groups;
	QSpacerItem* m_spacerPtr;
};


} // namespace imtloggui


