#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtlicgui/IItem.h>
#include <imtlicgui/IItemChangeHandler.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureTreeGuiComp.h>


namespace imtlicgui
{


class CFeatureTreeGuiComp: public iqtgui::TDesignerGuiObserverCompBase<
			Ui::CFeatureTreeGuiComp, imtlicgui::IItemTree>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureTreeGuiComp, imtlicgui::IItemTree> BaseClass;

	I_BEGIN_COMPONENT(CFeatureTreeGuiComp);
		I_ASSIGN(m_headerLabelAttrPtr, "HeaderLabel", "Header label", true, "Features");
		I_ASSIGN(m_showFeatureStatesAttrPtr, "ShowFeatureStates", "Show feature selection states", true, true);
	I_END_COMPONENT;

	CFeatureTreeGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();

private:
	void CreteTreeItems(QTreeWidgetItem* parentTreeItemPtr, QList<IItemTree*> items);
	IItemTree* FindItem(IItemTree* fromItemPtr, const QByteArray& itemId) const;

private Q_SLOTS:
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

private:
	I_TEXTATTR(m_headerLabelAttrPtr);
	I_ATTR(bool, m_showFeatureStatesAttrPtr);

	bool m_blockItemChangedSignal;
};


} // namespace imtlicgui


