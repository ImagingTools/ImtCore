#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlicgui/CFeatureDependencyEditorBase.h>
#include <GeneratedFiles/imtlicgui/ui_CFeaturePackageGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the feature packages.
	\ingroup LicenseManagement
*/
class CFeaturePackageGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider>,
			protected CFeatureDependencyEditorBase,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider> BaseClass;
	typedef CFeatureDependencyEditorBase BaseClass2;

	I_BEGIN_COMPONENT(CFeaturePackageGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_SUBELEMENT(FeaturePackageProxy);
		I_REGISTER_SUBELEMENT_INTERFACE(FeaturePackageProxy, imtlic::IFeatureInfoProvider, ExtractFeaturePackageProxy);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
	I_END_COMPONENT;

	CFeaturePackageGuiComp();

Q_SIGNALS:
	void EmitFeatureTreeItemChanged();

protected:
	void UpdateFeatureList();

	virtual void UpdateFeaturePackageModel() override;
	virtual void FeatureTreeItemChanged() override;

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

private Q_SLOTS:
	void on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column);
	virtual void OnShowCollectionEditor();
	virtual void OnShowFeatureDependencyEditor();
	virtual void OnFeatureListSelectionChanged();
	virtual void OnFeatureTreeItemChanged();

protected:
	class FeaturePackageProxy: virtual public imtlic::IFeatureInfoProvider
	{
	public:
		FeaturePackageProxy(CFeaturePackageGuiComp& parent);

		// reimplemented (imtlic::IFeatureInfoProvider)
		virtual const imtbase::IObjectCollection* GetFeaturePackages() const override;
		virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
		virtual const imtlic::IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
		virtual const imtlic::IFeatureDependenciesProvider* GetDependenciesInfoProvider() const override;
		virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const override;
		virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;
		virtual quint32 GetMinimalVersion(int versionId) const override;

	private:
		CFeaturePackageGuiComp& m_parent;
		imtbase::CCollectionInfo m_collectionInfo;
	};
	
	template <class InterfaceType>
	static InterfaceType* ExtractFeaturePackageProxy(CFeaturePackageGuiComp& component)
	{
		return &component.m_featurePackageProxy;
	}

protected:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);

	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_showCollectionEditorCommand;
	iqtgui::CHierarchicalCommand m_showDependenciesEditorCommand;

	FeaturePackageProxy m_featurePackageProxy;
};


} // namespace imtlicgui


