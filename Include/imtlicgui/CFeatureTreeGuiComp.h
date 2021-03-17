#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtlicgui/IFeatureItemStateHandler.h>
#include <imtlicgui/IItem.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureTreeGuiComp.h>


namespace imtlicgui
{


class CFeatureTreeGuiComp: public iqtgui::TDesignerGuiObserverCompBase<
			Ui::CFeatureTreeGuiComp, imtlicgui::IItemTree>
{
	Q_OBJECT
public:
	typedef public iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureTreeGuiComp, imtlicgui::IItemTree> BaseClass;

	I_BEGIN_COMPONENT(CFeatureTreeGuiComp);
		I_ASSIGN(m_headerLabelAttrPtr, "HeaderLabel", "Header label", true, "Features");
		I_ASSIGN(m_showFeatureStatesAttrPtr, "ShowFeatureStates", "Show feature selection states", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();

private:
	void CreteTreeItems(QTreeWidgetItem* parentTreeItemPtr, QList<IItemTree*> items);

private Q_SLOTS:
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

private:
	I_TEXTATTR(m_headerLabelAttrPtr);
	I_ATTR(bool, m_showFeatureStatesAttrPtr);
};


} // namespace imtlicgui


