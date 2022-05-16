#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CPaginationGuiComp.h>


namespace imtgui
{


class CPaginationGuiComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::CPaginationGuiComp, iprm::ISelectionParam>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CPaginationGuiComp, iprm::ISelectionParam> BaseClass;

	I_BEGIN_COMPONENT(CPaginationGuiComp);
	I_END_COMPONENT;

	CPaginationGuiComp();

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IObserver)
	virtual void OnGuiModelDetached() override;

	// reimplemented (ibase::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged();

protected:
	void UpdateIcons();
	void UpdateNavigationButtons();
	void UpdatePageButtons();

private Q_SLOTS:
	void OnFirstClicked();
	void OnPrev10Clicked();
	void OnPageClicked();
	void OnNext10Clicked();
	void OnLastClicked();

private:
	int m_firstVisiblePageIndex;
};


} // namespace imtgui


