#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IEnableableParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseController.h>
#include <imtlic/ILicenseStatus.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CProductInstanceInfoViewComp.h>


namespace imtlicgui
{


/**
	License instance viewer. This component shows information about available licenses and features in the product.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewComp);
		I_ASSIGN(m_encryptionKeysProviderCompPtr, "EncryptionKeysProvider", "Provider of the license key", true, "EncryptionKeysProvider");
		I_ASSIGN(m_licenseKeyPersistenceCompPtr, "LicenseKeyPersistence", "Persistence for the license key", true, "LicenseKeyPersistence");
		I_ASSIGN(m_importLicenseEnablerCompPtr, "ImportLicenseEnabler", "Enabling of the license import", false, "ImportLicenseEnabler");
		I_ASSIGN(m_licenseRequestEnablerCompPtr, "LicenseRequestEnabler", "Enabling of the license request", false, "LicenseRequestEnabler");
		I_ASSIGN(m_productInstanceCompPtr, "ProductInstance", "Instance of the product installation", true, "ProductInstance");
		I_ASSIGN(m_licenseControllerCompPtr, "LicenseController", "License controller used for import", true, "LicenseController");
		I_ASSIGN(m_licenseStatusCompPtr, "LicenseStatus", "Status of the current license", true, "LicenseStatus");
		I_ASSIGN(m_licenseRequestDescriptionTextAttrPtr, "LicenseRequestDescriptionText", "Description text for the request button", true, "");
		I_ASSIGN(m_showLicenseControlPanelAttrPtr, "ShowLicenseControlPanel", "If enabled the license control panel will be shown", true, true);
		I_ASSIGN(m_showLicenseRequestAttrPtr, "ShowLicenseRequestPanel", "If enabled the new license request panel will be shown", true, false);
		I_END_COMPONENT;
	
	CProductInstanceInfoViewComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

private:
	void UpdateProductName();
	void UpdateFeatureTree();
	void OnImportLicenseEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* licenseImportEnablerPtr);
	void OnLicenseRequestEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* licenseRequstEnablerPtr);
	void OnLicenseStatusChanged(const istd::IChangeable::ChangeSet& changeSet, const imtlic::ILicenseStatus* licenseStatusPtr);

private Q_SLOTS:
	void on_NewLicenseRequestButton_clicked();
	void on_LoadLicenseButton_clicked();

private:
	I_REF(imtcrypt::IEncryptionKeysProvider, m_encryptionKeysProviderCompPtr);
	I_REF(ifile::IFilePersistence, m_licenseKeyPersistenceCompPtr);
	I_REF(iprm::IEnableableParam, m_importLicenseEnablerCompPtr);
	I_REF(iprm::IEnableableParam, m_licenseRequestEnablerCompPtr);
	I_REF(imtlic::IProductInstanceInfo, m_productInstanceCompPtr);
	I_REF(imtlic::ILicenseController, m_licenseControllerCompPtr);
	I_REF(imtlic::ILicenseStatus, m_licenseStatusCompPtr);
	I_TEXTATTR(m_licenseRequestDescriptionTextAttrPtr);
	I_ATTR(bool, m_showLicenseControlPanelAttrPtr);
	I_ATTR(bool, m_showLicenseRequestAttrPtr);

	mutable imtbase::TModelUpdateBinder<iprm::IEnableableParam, CProductInstanceInfoViewComp> m_importLicenseEnablerObserver;
	mutable imtbase::TModelUpdateBinder<iprm::IEnableableParam, CProductInstanceInfoViewComp> m_licenseRequestEnablerObserver;
	mutable imtbase::TModelUpdateBinder<imtlic::ILicenseStatus, CProductInstanceInfoViewComp> m_licenseStatusObserver;
};


} // namespace imtlicgui


