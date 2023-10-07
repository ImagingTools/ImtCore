#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlicgui/TFeatureTreeModelCompWrap.h>
#include <imtlicgui/CItem.h>
#include <GeneratedFiles/imtlicgui/ui_CFeaturePackageGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the feature packages.
	\ingroup LicenseManagement
*/
class CFeaturePackageGuiComp:
			public TFeatureTreeModelCompWrap<
						iqtgui::TDesignerGuiObserverCompBase<
									Ui::CFeaturePackageGuiComp, imtlic::IFeaturePackage>>,
			virtual public imtlicgui::IItemChangeHandler
{
	Q_OBJECT
public:
	typedef TFeatureTreeModelCompWrap<
				iqtgui::TDesignerGuiObserverCompBase<
							Ui::CFeaturePackageGuiComp, imtlic::IFeaturePackage>> BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageGuiComp);
		I_REGISTER_SUBELEMENT(FeaturePackageProxy);
		I_REGISTER_SUBELEMENT_INTERFACE(FeaturePackageProxy, imtlic::IFeatureInfoProvider, ExtractFeaturePackageProxy);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
		I_ASSIGN(m_featureTreeCompPtr, "FeatureTreeView", "Feature tree view", true, "FeatureTreeGui");
		I_ASSIGN_TO(m_featureTreeObserverCompPtr, m_featureTreeCompPtr, true);
	I_END_COMPONENT;

	CFeaturePackageGuiComp();

	// reimplemented (imtlicgui::IItemChangeHandler)
	virtual void OnItemChanged(const QByteArray& itemId, ChangeId changeId, QVariantList params) override;

Q_SIGNALS:
	void EmitItemChangedHandler();

protected:
	// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)
	virtual void UpdateItemTree() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;
	
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private:
	void OnFeatureSelectionChanged(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::ISelection* selectionPtr);

	template <class InterfaceType>
	static InterfaceType* ExtractFeaturePackageProxy(CFeaturePackageGuiComp& component)
	{
		return &component.m_featurePackageProxy;
	}

private Q_SLOTS:
	void OnItemChangedHandler();

private:
	class FeaturePackageProxy: virtual public imtlic::IFeaturePackage
	{
	public:
		FeaturePackageProxy(CFeaturePackageGuiComp& parent);

		// reimplemented (IFeaturePackage)
		virtual QByteArray GetPackageId() const override;
		virtual void SetPackageId(const QByteArray& packageId) override;
		virtual QString GetPackageName() const override;
		virtual void SetPackageName(const QString& packageName) override;

		// reimplemented (imtlic::IFeatureInfoProvider)
		virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
		virtual const imtlic::IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;
		virtual quint32 GetMinimalVersion(int versionId) const override;

	private:
		CFeaturePackageGuiComp& m_parent;
		imtbase::CCollectionInfo m_collectionInfo;
	};

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_featureTreeCompPtr);
	I_REF(imod::IObserver, m_featureTreeObserverCompPtr);

	bool m_blockItemChangedHandler;

	FeaturePackageProxy m_featurePackageProxy;

	imtbase::TModelUpdateBinder<imtbase::ISelection, CFeaturePackageGuiComp> m_featureSelectionObserver;

	QByteArray m_selectedFeatureId;
	QByteArray m_missingDependenciesIds;
	imtlic::CFeaturePackageCollectionUtility::FeatureDependencyMap m_dependencyMap;
	QByteArray m_unsavedItemsGroupId;
	QByteArray m_missingItemsGroupId;
};


} // namespace imtlicgui


