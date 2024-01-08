#pragma once


// ACF includes
#include <iprm/CParamsSet.h>
#include <ifile/IFileNameParam.h>
#include <ifile/ITempFileManager.h>

// ImtCore includes
#include <imtcrypt/IHashGenerator.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>

#undef GetObject


namespace imtgql
{


class CGqlFileRepositoryComp:
			virtual public imtbase::IMetaInfoCreator,
			public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlFileRepositoryComp);
		I_REGISTER_INTERFACE(imtbase::IMetaInfoCreator);
		I_ASSIGN_MULTI_1(m_supportedTypeListAttrPtr, "SupportedTypeList", "The type list, supported by repository", true, "File");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN_TO(m_fileObjectCollectionCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_hashGeneratorCompPtr, "HashGenerator", "The Generator, used to calc file's hash summ", false, "HashGenerator");
		I_ASSIGN(m_tempFileManagerCompPtr, "TempFileManager", "Temporary file manager used to store uploading files", true, "TempFileManager");
	I_END_COMPONENT;

	// reimplemented(imtbase::IMetaInfoCreator)
	virtual TypeIds GetSupportedTypeIds() const override;
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const override;

protected:
	static void SetErrorToModel(
				imtbase::CTreeItemModel& rootModel,
				imtbase::CTreeItemModel& dataModel,
				imtbase::CTreeItemModel& itemsModel,
				imtbase::CTreeItemModel& notificationModel,
				const QString& errorMessage);

	// reimplemented (CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const QByteArray& objectId,
				QString& errorMessage) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_MULTIATTR(QByteArray, m_supportedTypeListAttrPtr);
	I_REF(imtrepo::IFileObjectCollection, m_fileObjectCollectionCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashGeneratorCompPtr);
	I_REF(ifile::ITempFileManager, m_tempFileManagerCompPtr);
};


} // namespace imtguigql


