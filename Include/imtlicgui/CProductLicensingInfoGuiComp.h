#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/CMultiSelection.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlicgui/IFeatureItemStateHandler.h>
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
			virtual public imtlicgui::IFeatureItemStateHandler
{
	Q_OBJECT
public:
	typedef public TFeatureTreeModelCompWrap<
				iqtgui::TDesignerGuiObserverCompBase <
							Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo>> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
		I_REGISTER_INTERFACE(imtlicgui::IFeatureItemStateHandler)
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
		I_ASSIGN(m_featureTreeCompPtr, "FeatureTreeView", "Feature tree view", true, "FeatureTreeGui");
		I_ASSIGN_TO(m_featureTreeObserverCompPtr, m_featureTreeCompPtr, true);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

	// reimplemented (imtlicgui::IFeatureItemStateHandler)
	virtual void OnItemStateChanged(const QByteArray& itemId, bool isChecked) override;
	
protected:
	// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)
	virtual void UpdateFeatureTreeModels(
				imtbase::IObjectCollection* featureTreeModelPtr,
				imtbase::IMultiSelection* selectedFeaturesModelPtr,
				imtbase::IMultiSelection* disabledFeaturesModelPtr) override;

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
				const imtbase::IMultiSelection* selectionPtr);
	void EnumerateDependencies(const QByteArrayList& featureIds);
	void EnumerateMissingFeatures();

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_featureTreeCompPtr);
	I_REF(imod::IObserver, m_featureTreeObserverCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IMultiSelection, CProductLicensingInfoGuiComp> m_licenseSelectionObserver;

	QByteArray m_selectedLicenseId;
	QByteArrayList m_selectedFeatureIds;
	QByteArrayList m_missingFeatureIds;
};


} // namespace imtlicgui


