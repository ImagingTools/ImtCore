#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlicgui/IItemSelectionChangeDelegate.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureTreeGuiComp.h>


namespace imtlicgui
{


class CFeatureTreeGuiComp: public iqtgui::TDesignerGuiObserverCompBase<
			Ui::CFeatureTreeGuiComp, imtbase::IObjectCollection>
{
	Q_OBJECT
public:
	typedef public iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureTreeGuiComp, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CFeatureTreeGuiComp);
		I_ASSIGN(m_showFeatureStatesAttrPtr, "ShowFeatureStates", "Show feature selection states", true, true);
		I_ASSIGN(m_itemSelectionChangeDelegateCompPtr, "ItemSelectionChangeDelegate", "Delegate for item selection change", false, "");
	I_END_COMPONENT;

	CFeatureTreeGuiComp();

Q_SIGNALS:
	void EmitFeatureTreeItemChanged(const QByteArray& itemId, bool isChecked);

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

private:
	QTreeWidgetItem* FindItem(const QByteArray& itemId);

private Q_SLOTS:
	void OnFeatureTreeItemChanged(const QByteArray& itemId, bool isChecked);
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

private:
	enum DataRole
	{
		DR_ITEM_ID = Qt::UserRole,
		DR_ITEM_TYPE
	};

	enum ItemType
	{
		IT_PACKAGE = 0,
		IT_FEATURE
	};

private:
	I_ATTR(bool, m_showFeatureStatesAttrPtr);
	I_REF(imtlicgui::IItemSelectionChangeDelegate, m_itemSelectionChangeDelegateCompPtr);
};


} // namespace imtlicgui


