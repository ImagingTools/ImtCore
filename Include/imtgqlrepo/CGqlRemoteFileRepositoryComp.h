#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/CFileRepositoryComp.h>
#include <imtclientgql/IGqlClient.h>

// imtgqlrepo SDL includes
#include <GeneratedFiles/imtgqlrepo/SDL/CFileMetaInfo.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CInsertRequest.h>

// win-API bug
#undef GetObject


namespace imtgqlrepo
{


/**
	Gql Remote File Repository Comp
 */
class CGqlRemoteFileRepositoryComp:
			virtual public imtrepo::IFileObjectCollection,
			public imtrepo:: CFileRepositoryComp
{
public:
	typedef imtrepo:: CFileRepositoryComp BaseClass;
	typedef QList<sdl::CFileMetaInfo> FileMetaInfoList;

	I_BEGIN_COMPONENT(CGqlRemoteFileRepositoryComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::ICollectionInfo)
	int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const override;
	Ids GetElementIds(int offset, int count, const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const override;
	QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const override;
	idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const override;
	bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr) override;
	bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr) override;
	bool SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* logPtr) override;

	// reimplemented (imtbase::IObjectCollectionInfo)
	idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (imtrepo::IFileCollectionInfo)
	bool GetFileInfo(const QByteArray& objectId, imtrepo::IFileCollectionItem& collectionItem) const override;
	FileCollectionLayout GetCollectionFileLayout() const override;
	QString GetCollectionRootFolder() const override;

	// reimplemented (imtbase::IObjectCollection)
	int GetOperationFlags(const Id& elementId) const override;
	Id InsertNewObject(const QByteArray& typeId, const QString& name, const QString& description, DataPtr defaultValuePtr, const Id& proposedElementId, const idoc::IDocumentMetaInfo* dataMetaInfoPtr, const idoc::IDocumentMetaInfo* elementMetaInfoPtr, const imtbase::IOperationContext* operationContextPtr) override;
	bool RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr) override;

	// reimplemented (imtrepo::IFileObjectCollection)
	QString GetFile(const QByteArray& objectId, const QString& targetFilePath) const override;
	QByteArray InsertFile(const QString& filePath, const QByteArray& objectTypeId, const QString& objectName, const QString& objectDescription, const QByteArray& proposedObjectId, const idoc::IDocumentMetaInfo* dataMetaInfoPtr, const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr) override;
	bool UpdateFile(const QString& filePath, const QByteArray& objectId) override;

protected:
	bool SendInfoMessage(
		const QString& message,
		const QString& messageSource = QString(),
		ilog::IMessageConsumer* slaveLogPtr = nullptr) const;
	bool SendWarningMessage(
		const QString& message,
		const QString& messageSource = QString(),
		ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

	bool SendErrorMessage(
		const QString& message,
		const QString& messageSource = QString(),
		ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

	bool SendCriticalMessage(
		const QString& message,
		const QString& messageSource = QString(),
		ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	sdl::CFileMetaInfo GetMetaInfoById(
			const Id& elementId,
			ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

	imtclientgql::IGqlClient::GqlRequestPtr CreateApiRequest(
			const QString& requestName,
			ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

	FileMetaInfoList ApiGetListOfObjects(ilog::IMessageConsumer* slaveLogPtr = nullptr) const;

private:
	I_REF(imtclientgql::IGqlClient, m_graphQlApiClientCompPtr);
};


} // namespace imtgqlrepo


