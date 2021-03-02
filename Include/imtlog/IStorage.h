#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class IStorage: virtual public istd::IPolymorphic
{
public:
	typedef struct{
		int64_t ObjectId;
		QByteArray TypeId;
		CTimeRange timeRange;
	} ObjectInfo;
	typedef QVector<ObjectInfo> ObjectInfos;

	/*
		Get objects info for given sectionId/timeRange.
	*/
	virtual ObjectInfos GetObjectInfos(
				const QByteArray& sectionId,
				const CTimeRange& timeRange) const = 0;

	/*
		Get/Deserialize object with given sectionId/objectId from storage.
	*/
	//virtual bool GetObject(
	//			const QByteArray& sectionId,
	//			int64_t objectId,
	//			iser::ISerializable* objectPtr) const = 0;

	/*
		Add/Serialize object with given sectionId/typeId/timeRange into storage.
		Return: objectId in storage
	*/
	virtual int64_t AddObject(
				const QByteArray& sectionId,
				const QByteArray& typeId,
				const CTimeRange& timeRange,
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

