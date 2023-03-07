#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// Acula includes
#include <imtdev/IDeviceInfo.h>


namespace imtdev
{


class CSimpleDeviceInfo:
			public idoc::CStandardDocumentMetaInfo,
			virtual public IDeviceInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (IDeviceInfo)
	virtual const idoc::IDocumentMetaInfo* GetDeviceMetaInfo() const override;

	// reimplemented (iser::IVersionInfo)
	virtual VersionIds GetVersionIds() const override;
	virtual bool GetVersionNumber(int versionId, quint32& result) const override;
	virtual QString GetVersionIdDescription(int versionId) const override;
	virtual QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;
};


} // namespace imtdev


