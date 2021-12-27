#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtgui/ICollectionViewDelegate.h>


namespace imtguigql
{


class CObjectCollectionControllerCompBase: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionControllerCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_viewDelegateCompPtr, "ViewDelegate", "View delegate (corresponding with the object type) used for the collection", true, "FeaturePackageViewDelegate");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
	I_END_COMPONENT;

	enum OperationType
	{
		OT_UNKNOWN,
		OT_NEW,
		OT_UPDATE,
		OT_DELETE,
		OT_RENAME,
		OT_SET_DESCRIPTION,
		OT_LIST,
		OT_HEADERS,
		OT_COMMANDS,
		OT_USER_OPERATION = 1000
	};

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetHeaders(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetCommands(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

protected:
	I_REF(imtgui::ICollectionViewDelegate, m_viewDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtguigql


