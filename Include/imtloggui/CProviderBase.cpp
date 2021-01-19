#include <imtloggui/CProviderBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


// public methods

// reimplemented (imtbase::IObjectCollection)

const imtbase::IRevisionController* CProviderBase::GetRevisionController() const
{
	return nullptr;
}


int CProviderBase::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return 0;
}


bool CProviderBase::GetDataMetaInfo(const QByteArray& /*objectId*/, MetaInfoPtr& /*metaInfoPtr*/) const
{
	return false;
}


QByteArray CProviderBase::InsertNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			const istd::IChangeable* /*defaultValuePtr*/,
			const QByteArray& /*proposedObjectId*/)
{
	return QByteArray();
}


bool CProviderBase::RemoveObject(const QByteArray& /*objectId*/)
{
	return false;
}


bool CProviderBase::GetObjectData(const QByteArray& /*objectId*/, DataPtr& /*dataPtr*/) const
{
	return false;
}


bool CProviderBase::SetObjectData(const QByteArray& /*objectId*/, const istd::IChangeable& /*object*/, CompatibilityMode /*mode*/)
{
	return false;
}


void CProviderBase::SetObjectName(const QByteArray& /*objectId*/, const QString& /*objectName*/)
{
}


void CProviderBase::SetObjectDescription(const QByteArray& /*objectId*/, const QString& /*objectDescription*/)
{
}


void CProviderBase::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
}


bool CProviderBase::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* /*eventHandler*/)
{
	return false;
}


bool CProviderBase::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* /*eventHandler*/)
{
	return false;
}


// reimplemented (IObjectCollectionInfo)

bool CProviderBase::GetCollectionItemMetaInfo(const QByteArray& /*objectId*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return false;
}


const iprm::IOptionsList* CProviderBase::GetObjectTypesInfo() const
{
	return nullptr;
}


imtbase::ICollectionInfo::Id CProviderBase::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return QByteArray();
}


// reimplemented (istd::IChangeable)

int CProviderBase::GetSupportedOperations() const
{
	return SO_NONE;
}


bool CProviderBase::CopyFrom(const IChangeable& /*object*/, CompatibilityMode /*mode*/)
{
	return false;
}


bool CProviderBase::ResetData(CompatibilityMode /*mode*/)
{
	return false;
}


} // namespace imtloggui


