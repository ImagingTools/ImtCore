#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtgql/IGqlClient.h>

// GmgSolutions includes
#include <imtcom/IFileTransfer.h>


namespace imtgql
{


/**
	Common interface for the GraphQL-based document database delegate.
	The job of the document delegate is the realization of the GraphQL requests for the common database operations (CRUD/L)
*/
class IGqlObjectCollectionDelegate: virtual public istd::IPolymorphic
{
public:
	enum MetaInfoType
	{
		MIT_KEYWORDS = idoc::IDocumentMetaInfo::MIT_USER + 30000,
		MIT_VERSION
	};

	struct ElementDescription
	{
		QString name;
		QString description;
	};

	/**
		Definition of a document information record used by listing of documents.
	*/
	struct ElementInfo: public ElementDescription
	{
		ElementInfo()
			:isBranch(false),
			version(-1)
		{
		}

		QByteArray id;
		QByteArray typeId;
		QByteArray orgId;

		QVector<QByteArray> path;
		bool isBranch;

		istd::TSmartPtr<idoc::IDocumentMetaInfo> dataMetaInfoPtr;
		istd::TSmartPtr<idoc::IDocumentMetaInfo> elementMetaInfoPtr;

		/**
			The server assigns the version to the document to be used when updating the data.
			When we want to change a document or information about it on the server,
			this version must be passed in the request. The version is used to synchronize
			simultaneous access to a document by multiple clients.
		*/
		int version;
	};

	typedef QVector<ElementInfo> ElementList;

	/**
		Type of the operation triggered by request
	*/
	enum OperationType
	{
		OT_UNKNOWN = 0,

		/**
			Insertion of a new document into the database.
		*/
		OT_INSERT,

		/**
			Update of the contents of an existing document.
		*/
		OT_UPDATE,

		/**
			Deleting of an existing document.
		*/
		OT_DELETE,

		/**
			Listing of existing documents in the database.
		*/
		OT_LIST,

		/**
			Getting the document data from the database.
		*/
		OT_GET
	};

	/**
		Definition of possible parts/blocks of a response
	*/
	enum ResponseDataType
	{
		/**
			URL used for upload the document data (given as a QByteArray).
		*/
		RDT_UPLOAD_URL,

		/**
			URL used for download the document data (given as a QByteArray).
		*/
		RDT_DOWNLOAD_URL,

		RDT_ELEMENT_COUNT,

		/**
			ID of the document in the database (given as a QByteArray).
		*/
		RDT_OBJECT_ID,

		/**
			Content of the document in the database (given as a QByteArray).
		*/
		RDT_OBJECT_CONTENT,

		/**
			Status message related to an operation (given as a QString).
		*/
		RDT_OPERATION_MESSAGE,

		/**
			Success status related to an operation (given as a bool).
			\todo Think about using of status codes at this place!
		*/
		RDT_OPERATION_STATUS
	};

	/**
		Get the ID of the supported document type.
	*/
	virtual QByteArray GetSupportedObjectTypeId() const = 0;

	/**
		Create the request for the inserting a new document of the given type to the database.
	*/
	virtual imtgql::IGqlRequest* CreateInsertBranchRequest(
				const QString& name,
				const QString& description,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const QByteArray& proposedElementId = QByteArray(),
				const QByteArray& parentId = QByteArray(),
				const QByteArray& orgId = QByteArray()) const = 0;

	/**
		Create the request for the inserting a new document of the given type to the database.
	*/
	virtual imtgql::IGqlRequest* CreateInsertObjectRequest(
				const QString& name,
				const QString& description,
				const istd::IChangeable& object,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const QString& uploadUrl = QString(),
				const QByteArray& proposedElementId = QByteArray(),
				const QByteArray& parentId = QByteArray()) const = 0;

	/**
		Create the request to get document content.
	*/
	virtual imtgql::IGqlRequest* CreateGetObjectContentRequest(const QByteArray& objectId) const = 0;

	/**
		Create request for the updating an existing document in the collection.
	*/
	virtual imtgql::IGqlRequest* CreateUpdateObjectContentRequest(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				int clientObjectVersion = -1,
				const QString& uploadUrl = QString()) const = 0;

	/**
		Create request for the removing an existing document from the collection.
	*/
	virtual imtgql::IGqlRequest* CreateRemoveElementRequest(
				const QByteArray& elementId,
				int clientElementVersion = -1) const = 0;


	// Element modification

	virtual imtgql::IGqlRequest* CreateUpdateElementDescriptionRequest(
				const QByteArray& elementId,
				const ElementDescription& description,
				int clientElementVersion) const = 0;
	virtual imtgql::IGqlRequest* CreateRenameBranchRequest(
				const QByteArray& branchId,
				const QByteArray& parentId,
				const QByteArray& orgId,
				const QString& newName) const = 0;


	// Elements enumeration

	virtual imtgql::IGqlRequest* CreateElementCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const QByteArray& parentId = QByteArray()) const = 0;
	virtual imtgql::IGqlRequest* CreateElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const QByteArray& parentId = QByteArray()) const = 0;
	virtual imtgql::IGqlRequest* CreateElementInfoRequest(const QByteArray& elementId) const = 0;

	/**
		Create list of documents based on the response data.
	*/
	virtual ElementList CreateElementListFromResponse(
				const imtgql::IGqlRequest& request,
				const QByteArray& responseData) const = 0;

	virtual ElementInfo CreateElementInfoFromResponse(
				const imtgql::IGqlRequest& request,
				const QByteArray& responseData) const = 0;

	/**
		Get/Extract data from the server response according to the given operation type.
	*/
	virtual QVariant GetDataFromResponse(
				OperationType operationType,
				ResponseDataType responseDataType,
				const QByteArray& response) const = 0;

	/**
		Set the data content to the document instance.
	*/
	virtual bool SetObjectContent(const QByteArray& content, istd::IChangeable& object) const = 0;

	/**
		If \c OF_SUPPORT_FILE_UPLOAD flag is supported, this method should return the request for getting the upload-URL for the new document.
	*/
	virtual imtgql::IGqlRequest* CreateUploadUrlRequest(const QString& fileName, const QByteArray& parentId) const = 0;

	/**
		Create the request to get document download URL.
	*/
	virtual imtgql::IGqlRequest* CreateDownloadUrlRequest(const QByteArray& objectId) const = 0;

	virtual imtcom::IFileTransfer* GetFileTransfer() const = 0;

	// Responses

	class IResponseBase: virtual public imtgql::IGqlClient::ResponseHandler
	{
	public:
		virtual int GetErrorCount() const = 0;
		virtual QString GetErrorType(int index) const = 0;
		virtual QString GetErrorInfo(int index) const = 0;
		virtual QString GetMessage(int index) const = 0;
	};

	class IInsertBranchResponse: virtual public IResponseBase
	{
	public:
		virtual QByteArray GetBranchId() const = 0;
	};
	typedef QSharedPointer<IInsertBranchResponse> InsertBranchResponsePtr;
	virtual InsertBranchResponsePtr CreateInsertBranchResponsePtr() const = 0;

	class IInsertObjectResponse: virtual public IResponseBase
	{
	public:
		virtual QByteArray GetObjectId() const = 0;
	};
	typedef QSharedPointer<IInsertObjectResponse> InsertObjectResponsePtr;
	virtual InsertObjectResponsePtr CreateInsertObjectResponsePtr() const = 0;

	class IGetObjectContentResponse: virtual public IResponseBase
	{
	public:
	};
	typedef QSharedPointer<IGetObjectContentResponse> GetObjectContentResponsePtr;
	virtual GetObjectContentResponsePtr CreateGetObjectContentResponsePtr() const = 0;

	class IUpdateObjectContentResponse: virtual public IResponseBase
	{
	public:
		virtual bool Result() const = 0;
	};
	typedef QSharedPointer<IUpdateObjectContentResponse> UpdateObjectContentResponsePtr;
	virtual UpdateObjectContentResponsePtr CreateUpdateObjectContentResponsePtr() const = 0;

	class IRemoveElementResponse: virtual public IResponseBase
	{
	public:
		virtual bool Result() const = 0;
	};
	typedef QSharedPointer<IRemoveElementResponse> RemoveElementResponsePtr;
	virtual RemoveElementResponsePtr CreateRemoveElementResponsePtr() const = 0;

	class IRenameBranchResponse : virtual public IResponseBase
	{
	public:
		virtual bool Result() const = 0;
	};
	typedef QSharedPointer<IRenameBranchResponse> RenameBranchResponsePtr;
	virtual RenameBranchResponsePtr CreateRenameBranchResponsePtr() const = 0;

	class IElementCountResponse: virtual public IResponseBase
	{
	public:
		virtual int GetCount() const = 0;
	};
	typedef QSharedPointer<IElementCountResponse> ElementCountResponsePtr;
	virtual ElementCountResponsePtr CreateElementCountResponsePtr() const = 0;

	class IElementListResponse: virtual public IResponseBase
	{
	public:
		virtual ElementList GetElementList() const = 0;
	};
	typedef QSharedPointer<IElementListResponse> ElementListResponsePtr;
	virtual ElementListResponsePtr CreateElementListResponsePtr() const = 0;

	class IElementInfoResponse: virtual public IResponseBase
	{
	public:
		virtual ElementInfo GetElementInfo() const = 0;
	};
	typedef QSharedPointer<IElementInfoResponse> ElementInfoResponsePtr;
	virtual ElementInfoResponsePtr CreateElementInfoResponsePtr() const = 0;

	class IUploadUrlResponse: virtual public IResponseBase
	{
	public:
		virtual QString GetUploadUrl() const = 0;
	};
	typedef QSharedPointer<IUploadUrlResponse> UploadUrlResponsePtr;
	virtual UploadUrlResponsePtr CreateUploadUrlResponsePtr() const = 0;

	class IDownloadUrlResponse: virtual public IResponseBase
	{
	public:
		virtual QString GetDownloadUrl() const = 0;
	};
	typedef QSharedPointer<IDownloadUrlResponse> DownloadUrlResponsePtr;
	virtual DownloadUrlResponsePtr CreateDownloadUrlResponsePtr() const = 0;
};


} // namespace imtgql


