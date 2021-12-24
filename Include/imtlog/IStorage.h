#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/CTimeRange.h>


namespace imtlog
{


class IStorage: virtual public istd::IPolymorphic
{
public:
	struct ObjectInfo
	{
		int64_t ObjectId;
		QByteArray TypeId;
		imtbase::CTimeRange timeRange;
	};

	typedef QVector<ObjectInfo> ObjectInfos;

	/*
		Get objects info for given sectionId/timeRange.
	*/
	virtual ObjectInfos GetObjectInfos(
				const QByteArray& sectionId,
				const imtbase::CTimeRange& timeRange) const = 0;
	/*
		Add/Serialize object with given sectionId/typeId/timeRange into storage.
		Return: objectId in storage
	*/
	virtual int64_t AddObject(
				const QByteArray& sectionId,
				const QByteArray& typeId,
				const imtbase::CTimeRange& timeRange,
				const iser::ISerializable* objectPtr) = 0;
	/*
		Update/Serialize object with given sectionId/objectId into storage.
	*/
	virtual bool UpdateObject(
				const QByteArray& sectionId,
				int64_t objectId,
				const iser::ISerializable* objectPtr) = 0;
	/*
		Removeobject with given sectionId/objectId from storage.
	*/
	virtual bool RemoveObjects(
				const QByteArray& sectionId,
				int64_t objectId) = 0;
};


} // namespace imtlog


