#pragma once


// Qt includes
#include <QtGui/QStandardItemModel>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>


namespace imtgui
{


class CObjectCollectionViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
	I_END_COMPONENT;

	CObjectCollectionViewComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;
 
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;

private Q_SLOTS:
	void OnInsert();
	void OnRemove();

private:
	QStandardItemModel m_itemModel;
	QVector<QStandardItem*> m_typeItems;

	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_editCommands;
	iqtgui::CHierarchicalCommand m_insertCommand;
	iqtgui::CHierarchicalCommand m_removeCommand;
};


} // namespace imtgui
