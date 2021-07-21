#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
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

private Q_SLOTS:
	void on_NewLicenseRequestButton_clicked();
	void on_LoadLicenseButton_clicked();

private:
	I_REF(imtcrypt::IEncryptionKeysProvider, m_encryptionKeysProviderCompPtr);
	I_REF(ifile::IFilePersistence, m_licenseKeyPersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_licensePathCompPtr);
};


} // namespace imtlicgui


