#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtloggui
{


class CProviderBase: virtual public imtbase::IObjectCollection
{
public:
	// reimplemented (imtbase::IObjectCollection)
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
	virtual bool GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;
	virtual bool RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;
	virtual bool UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (istd::IChangeable)
	//virtual int GetSupportedOperations() const override;
	//virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	//virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
};


} // namespace imtloggui


