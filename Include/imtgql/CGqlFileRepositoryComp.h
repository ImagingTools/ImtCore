#pragma once


// ACF includes
#include <iprm/CParamsSet.h>
#include <ifile/IFileNameParam.h>

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

		// Known-Limitation: nedd to assign here for ASSIGN_TO instructions
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN_TO(m_fileObjectCollectionCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_hashGeneratorCompPtr, "HashGenerator", "The Generator, used to calc file's hash summ", false, "HashGenerator");
		I_ASSIGN(m_requestCollectionCompPtr, "RequestCollection", "Collection, that stores uploaded/downloading files paths", false, "RequestCollection");
		I_ASSIGN(m_tempDirectoryPathCompPtr, "TempDirectoryPath", "The path where temp files will be stored, while it is required. \nWarning: this path will be cleared on suutdown", true, "TempDirectoryPath");
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

	QString GetFilePathFromRequestQueue(const QByteArray& queueRequestId) const;

	// reimplemented (CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
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

protected:
	I_MULTIATTR(QByteArray, m_supportedTypeListAttrPtr);
	I_REF(imtrepo::IFileObjectCollection, m_fileObjectCollectionCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashGeneratorCompPtr);
	I_REF(imtbase::IObjectCollection, m_requestCollectionCompPtr);
	I_REF(ifile::IFileNameParam, m_tempDirectoryPathCompPtr);
};


} // namespace imtguigql


