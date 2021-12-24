#include <imtlog/CStorageComp.h>


namespace imtlog
{


// public methods

IStorage::ObjectInfos CStorageComp::GetObjectInfos(
			const QByteArray& /*sectionId*/,
			const imtbase::CTimeRange& /*timeRange*/) const
{
	return IStorage::ObjectInfos();
}


int64_t CStorageComp::CStorageComp::AddObject(
			const QByteArray& /*sectionId*/,
			const QByteArray& /*typeId*/,
			const imtbase::CTimeRange& /*timeRange*/,
			const iser::ISerializable* /*objectPtr*/)
{
	return -1;
}


bool CStorageComp::UpdateObject(
			const QByteArray& /*sectionId*/,
			int64_t /*objectId*/,
			const iser::ISerializable* /*objectPtr*/)
{
	return false;
}


bool CStorageComp::RemoveObjects(
			const QByteArray& /*sectionId*/,
			int64_t /*objectId*/)
{
	return false;
}


} // namespace imtlog

