#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <imod/CModelProxy.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgui/IViewExtender.h>
#include <GeneratedFiles/imtgui/ui_CCompositeObjectViewComp.h>


namespace imtgui
{


class CCompositeObjectViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CCompositeObjectViewComp, imtbase::IObjectCollection>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CCompositeObjectViewComp, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CCompositeObjectViewComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN_MULTI_0(m_objectTypeAttrPtr, "ObjectTypeId", "ID of objects on the container", true);
		I_ASSIGN_MULTI_0(m_objectViewFactoryListCompPtr, "ObjectView", "View for showing object", true);
		I_ASSIGN_TO(m_objectObserverFactoryListCompPtr, m_objectViewFactoryListCompPtr, true);
		I_ASSIGN_MULTI_0(m_objectsToExtendAttrPtr, "ObjectsToExtend", "Object-IDs which view will be extended", false);
		I_ASSIGN_MULTI_0(m_objectsFromExtendAttrPtr, "ObjectsFromExtend", "Object-IDs to extend view", false);
		I_ASSIGN_MULTI_0(m_viewExtendersCompPtr, "ViewExtenders", "View extenders", false);
	I_END_COMPONENT;

	CCompositeObjectViewComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

private:
	void CreateView();
	void DestroyView();

private:
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;

	I_MULTIATTR(QByteArray, m_objectTypeAttrPtr);
	I_MULTIFACT(iqtgui::IGuiObject, m_objectViewFactoryListCompPtr);
	I_MULTIFACT(imod::IObserver, m_objectObserverFactoryListCompPtr);

	I_MULTIATTR(QByteArray, m_objectsToExtendAttrPtr);
	I_MULTIATTR(QByteArray, m_objectsFromExtendAttrPtr);
	I_MULTIFACT(imtgui::IViewExtender, m_viewExtendersCompPtr);

	QList<QByteArray> m_objectIds;
	QList<icomp::IComponent*> m_views;
	QList<icomp::IComponent*> m_viewExtenders;
};


} // namespace imtgui


