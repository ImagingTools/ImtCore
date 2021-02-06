#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/ILicenseInfoProvider.h>
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


/**
	License manager implementation.
*/
class CLicenseManager:
			virtual public imtbase::ILicenseInfoProvider,
			virtual public imtbase::IObjectCollection,
			virtual public iser::ISerializable
{
public:
	CLicenseManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtbase::ILicenseInfo* GetLicenseInfo(const QByteArray & licenseId) const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual bool GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray()) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;
	virtual bool RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;
	virtual bool UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	CObjectCollection m_licenses;
};


} // namespace imtbase


