#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IEnableableParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtlic/IProductInstanceInfo.h>
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
		I_ASSIGN(m_licensePathCompPtr, "LicensePath", "Path to the license file", true, "LicensePath");
		I_ASSIGN(m_importLicenseEnablerCompPtr, "ImportLicenseEnabler", "Enabling of the license import", false, "ImportLicenseEnabler");
		I_ASSIGN(m_licensePersistenceCompPtr, "LicensePersistence", "License persistence for the applying the imported license", true, "LicensePersistence");
		I_ASSIGN(m_productInstanceCompPtr, "ProductInstance", "Instance of the product installation", true, "ProductInstance");
	I_END_COMPONENT;

	CProductInstanceInfoViewComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private:
	void UpdateProductName();
	void UpdateFeatureTree();
	void OnImportLicenseEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* licenseImportEnablerPtr);

private Q_SLOTS:
	void on_NewLicenseRequestButton_clicked();
	void on_LoadLicenseButton_clicked();

private:
	I_REF(imtcrypt::IEncryptionKeysProvider, m_encryptionKeysProviderCompPtr);
	I_REF(ifile::IFilePersistence, m_licenseKeyPersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_licensePathCompPtr);
	I_REF(iprm::IEnableableParam, m_importLicenseEnablerCompPtr);
	I_REF(ifile::IFilePersistence, m_licensePersistenceCompPtr);
	I_REF(imtlic::IProductInstanceInfo, m_productInstanceCompPtr);

	mutable imtbase::TModelUpdateBinder<iprm::IEnableableParam, CProductInstanceInfoViewComp> m_importLicenseEnablerObserver;

};


} // namespace imtlicgui


