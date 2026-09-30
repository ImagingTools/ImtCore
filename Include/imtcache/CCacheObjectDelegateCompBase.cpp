#include <imtcache/CCacheObjectDelegateCompBase.h>


namespace imtcache
{


// protected methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

CCacheObjectDelegateCompBase::NewObjectQuery CCacheObjectDelegateCompBase::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* /*valuePtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return NewObjectQuery();
}


QByteArray CCacheObjectDelegateCompBase::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCacheObjectDelegateCompBase::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCacheObjectDelegateCompBase::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


QByteArray CCacheObjectDelegateCompBase::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCacheObjectDelegateCompBase::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


} // namespace imtcache
