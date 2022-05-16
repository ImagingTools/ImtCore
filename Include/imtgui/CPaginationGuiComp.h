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

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
	{
		return BaseClass::OnModelAttached(modelPtr, changeMask);
	}

	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

protected:
	void RefreshWidget();

private Q_SLOTS:
	void OnFirstClicked();
	void OnPrevClicked();
	void OnPageClicked();
	void OnNextClicked();
	void OnLastClicked();
};


} // namespace imtgui


