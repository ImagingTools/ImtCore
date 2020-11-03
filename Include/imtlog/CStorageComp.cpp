#include <imtlog/CStorageComp.h>


namespace imtlog
{


// public methods

IStorage::ObjectInfos CStorageComp::GetObjectInfos(
			const QByteArray& sectionId,
			const CTimeRange& timeRange) const
{
	return IStorage::ObjectInfos();
}


//bool CStorageComp::GetObject(
//			const QByteArray& sectionId,
//			int64_t objectId,
//			iser::ISerializable* objectPtr) const
//{
//	return false;
//}


int64_t CStorageComp::CStorageComp::AddObject(
			const QByteArray& sectionId,
			const QByteArray& typeId,
			const CTimeRange& timeRange,
			const iser::ISerializable* objectPtr)
{
	return -1;
}


bool CStorageComp::UpdateObject(
			const QByteArray& sectionId,
			int64_t objectId,
			const iser::ISerializable* objectPtr)
{
	return false;
}


bool CStorageComp::RemoveObjects(
			const QByteArray& sectionId,
			int64_t objectId)
{
	return false;
}


} // namespace imtlog

