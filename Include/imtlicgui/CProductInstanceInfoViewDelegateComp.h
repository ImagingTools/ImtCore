#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>
#include <ifile/IFilePersistence.h>
#include <imtcrypt/IEncryption.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtlicgui
{


class CProductInstanceInfoViewDelegateComp:
			public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp,
			public imtcrypt::IEncryptionKeysProvider
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewDelegateComp);
	I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
	I_ASSIGN(m_filePersistence, "filePersistence", "license file export instances", false, "");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_LICENSE = 2150
	};

	CProductInstanceInfoViewDelegateComp();

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
	virtual void SetupCommands() override;

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

protected Q_SLOTS:
	virtual void OnExportLicense();

private:
	I_REF(ifile::IFilePersistence, m_filePersistence);

	iqtgui::CHierarchicalCommand m_licenseCommands;
	iqtgui::CHierarchicalCommand m_exportLicenseCommand;
};


} // namespace imtlicgui


