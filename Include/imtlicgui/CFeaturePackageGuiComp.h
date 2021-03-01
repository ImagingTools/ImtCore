#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CFeaturePackageGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CFeaturePackageGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider> BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
	I_END_COMPONENT;

	CFeaturePackageGuiComp();

protected:
	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

protected Q_SLOTS:
	virtual void OnShowCollectionEditor();
	virtual void OnShowFeatureDependencyEditor();

protected:
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_showCollectionEditorCommand;
	iqtgui::CHierarchicalCommand m_showDependenciesEditorCommand;

private:
	void UpdateCommands();

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
};


} // namespace imtlicgui


