#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IStorage.h>


namespace imtlog
{


class CStorageComp:
			public icomp::CComponentBase,
			virtual public IStorage
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStorageComp);
		I_REGISTER_INTERFACE(IStorage);
	I_END_COMPONENT;

	virtual ObjectInfos GetObjectInfos(
				const QByteArray& sectionId,
				const CTimeRange& timeRange) const override;
	//virtual bool GetObject(
	//			const QByteArray& sectionId,
	//			int64_t objectId,
	//			iser::ISerializable* objectPtr) const override;
	virtual int64_t AddObject(
				const QByteArray& sectionId,
				const QByteArray& typeId,
				const CTimeRange& timeRange,
				const iser::ISerializable* objectPtr) override;
	virtual bool UpdateObject(
				const QByteArray& sectionId,
				int64_t objectId,
				const iser::ISerializable* objectPtr) override;
	virtual bool RemoveObjects(
				const QByteArray& sectionId,
				int64_t objectId) override;
};


} // namespace imtlog


