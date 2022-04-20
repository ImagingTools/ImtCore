#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/CSelection.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlicgui/IItemChangeHandler.h>
#include <imtlicgui/TFeatureTreeModelCompWrap.h>
#include <GeneratedFiles/imtlicgui/ui_CProductLicensingInfoGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/

class CProductLicensingInfoGuiComp:
			public TFeatureTreeModelCompWrap<
						iqtgui::TDesignerGuiObserverCompBase <
									Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo>>,
			virtual public imtlicgui::IItemChangeHandler
{
	Q_OBJECT
public:
	typedef TFeatureTreeModelCompWrap<
				iqtgui::TDesignerGuiObserverCompBase <
							Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo>> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
		I_ASSIGN(m_featureTreeCompPtr, "FeatureTreeView", "Feature tree view", true, "FeatureTreeGui");
		I_ASSIGN_TO(m_featureTreeObserverCompPtr, m_featureTreeCompPtr, true);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

	// reimplemented (imtlicgui::IFeatureItemStateHandler)
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
	void OnLicenseSelectionChanged(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::ISelection* selectionPtr);
	void EnumerateDependencies(const QByteArrayList& featureIds);
	int FindFeatureById(const QByteArray& featureId, const imtlic::ILicenseInfo::FeatureInfos& featureContainer) const;
	imtlic::ILicenseInfo::FeatureInfo GetFeatureInfo(const QByteArray& featureId) const;
	QByteArrayList GetSelectedFeatureIds() const;

private Q_SLOTS:
	void OnItemChangedHandler();
	void on_ProductIdEdit_textChanged();
	void on_ProductNameEdit_textChanged();

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_featureTreeCompPtr);
	I_REF(imod::IObserver, m_featureTreeObserverCompPtr);

	bool m_blockItemChangedHandler;

	imtbase::TModelUpdateBinder<imtbase::ISelection, CProductLicensingInfoGuiComp> m_licenseSelectionObserver;

	QByteArray m_selectedLicenseId;
	imtlic::ILicenseInfo::FeatureInfos m_selectedFeatures;
	QByteArrayList m_missingFeatureIds;
	QByteArray m_missingItemsGroupId;
};


} // namespace imtlicgui


