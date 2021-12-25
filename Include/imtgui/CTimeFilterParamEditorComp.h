#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/CHierarchicalCommand.h>

// Acula includes
#include <imtbase/ITimeFilterParam.h>
#include <GeneratedFiles/imtgui/ui_CTimeFilterParamEditorComp.h>


namespace imtgui
{


class CTimeFilterParamEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CTimeFilterParamEditorComp, imtbase::ITimeFilterParam>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CTimeFilterParamEditorComp, imtbase::ITimeFilterParam> BaseClass;

	I_BEGIN_COMPONENT(CTimeFilterParamEditorComp);
	I_END_COMPONENT;

	CTimeFilterParamEditorComp();

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiRetranslate() override;

private Q_SLOTS:
	void on_TimeCombo_currentIndexChanged(int index);
	void on_CustomBeginDate_dateTimeChanged(const QDateTime& dateTime);
	void on_CustomEndDate_dateTimeChanged(const QDateTime& dateTime);

private:
	void SetFilterTimeRange(const QDateTime& beginTime, const QDateTime& endTime);
};


} // namespace imtgui


