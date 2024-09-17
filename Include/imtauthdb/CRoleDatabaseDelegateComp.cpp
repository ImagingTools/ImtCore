#include <imtauthdb/CRoleDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IEnableableParam.h>
#include <iprm/CTextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

bool CRoleDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::TParamsPtr<iprm::IEnableableParam> isDefaultParamPtr(&filterParams, "IsDefault");
	if (isDefaultParamPtr.IsValid()){
		if (isDefaultParamPtr->IsEnabled()){
			filterQuery = QString("(\"Document\"->>'IsDefault')::boolean is TRUE");
		}
		else{
			filterQuery = QString("(\"Document\"->>'IsDefault')::boolean is FALSE");
		}
	}

	iprm::TParamsPtr<iprm::IEnableableParam> isGuestParamPtr(&filterParams, "IsGuest");
	if (isGuestParamPtr.IsValid()){
		if (isGuestParamPtr->IsEnabled()){
			filterQuery = QString("(\"Document\"->>'IsGuest')::boolean is TRUE");
		}
		else{
			filterQuery = QString("(\"Document\"->>'IsGuest')::boolean is FALSE");
		}
	}

	return BaseClass::CreateObjectFilterQuery(filterParams, filterQuery);
}


QByteArray CRoleDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const imtbase::IOperationContext* operationContextPtr) const
{
	imtauth::CRole* roleInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (collection.GetObjectData(objectId, dataPtr)){
		roleInfoPtr = dynamic_cast<imtauth::CRole*>(dataPtr.GetPtr());
	}

	if (roleInfoPtr == nullptr){
		return QByteArray();
	}

	if (roleInfoPtr->IsDefault() || roleInfoPtr->IsGuest()){
		return QByteArray();
	}

	return BaseClass::CreateDeleteObjectQuery(collection, objectId, operationContextPtr);
}


QByteArray CRoleDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool useExternDelegate) const
{
	return BaseClass::CreateUpdateObjectQuery(collection, objectId, object, operationContextPtr, useExternDelegate);
}


} // namespace imtauthdb


