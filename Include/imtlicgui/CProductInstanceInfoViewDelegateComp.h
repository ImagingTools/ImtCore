#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtgui/CDocumentCollectionViewDelegateComp.h>
#include <imtcrypt/IEncryption.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtlicgui
{


class CProductInstanceInfoViewDelegateComp:
			public imtgui::CDocumentCollectionViewDelegateComp,
			virtual public imtcrypt::IEncryptionKeysProvider
{
	Q_OBJECT
public:
	typedef imtgui::CDocumentCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewDelegateComp);
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_licensePersistenceCompPtr, "LicensePersistence", "Persistence used for license export", false, "LicensePersistence");
		I_ASSIGN(m_vectorKeyCompPtr, "VectorKey", "Additional key for AES encryption", false, "VectorKey");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_LICENSE = 2150
	};

	CProductInstanceInfoViewDelegateComp();

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual bool GetSummaryInformation(
				const QByteArray& objectId,
				const QVector<QByteArray> fieldIds,
				ObjectMetaInfo& objectMetaInfo) const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

protected Q_SLOTS:
	virtual void OnExportLicense();

private:
	I_REF(ifile::IFilePersistence, m_licensePersistenceCompPtr);
	I_REF(iprm::IIdParam, m_vectorKeyCompPtr);

	iqtgui::CHierarchicalCommand m_licenseCommands;
	iqtgui::CHierarchicalCommand m_exportLicenseCommand;
};


} // namespace imtlicgui


