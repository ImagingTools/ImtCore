#pragma once


// Acf includes
#include <ibase/ICommandsProvider.h>
#include <imod/CModelProxy.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtgui/ui_CCompositeObjectViewComp.h>


namespace imtgui
{


class CCompositeObjectViewComp:
			public iqtgui::TRestorableGuiWrap<
						iqtgui::TDesignerGuiObserverCompBase<Ui::CCompositeObjectViewComp, imtbase::IObjectCollection>>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiObserverCompBase<Ui::CCompositeObjectViewComp, imtbase::IObjectCollection>> BaseClass;

	I_BEGIN_COMPONENT(CCompositeObjectViewComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN_MULTI_0(m_objectViewFactoryListCompPtr, "ObjectView", "View for showing object", true);
		I_ASSIGN_MULTI_0(m_objectTypeAttrPtr, "ObjectTypeId", "ID of objects on the container", true);
	I_END_COMPONENT;

	CCompositeObjectViewComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

private:
	void CreateView();
	void DestroyView();

private:
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;

	I_MULTIFACT(iqtgui::IGuiObject, m_objectViewFactoryListCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeAttrPtr);

	QVector<QByteArray> m_objectIds;
	QVector<iqtgui::IGuiObject*> m_objectViews;
};


} // namespace imtgui


