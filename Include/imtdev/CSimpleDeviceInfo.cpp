#include <imtdev/CSimpleDeviceInfo.h>


namespace imtdev
{


// public methods

// reimplemented (IDeviceInfo)

const idoc::IDocumentMetaInfo* CSimpleDeviceInfo::GetDeviceMetaInfo() const
{
	return this;
}


// reimplemented (iser::IVersionInfo)

iser::IVersionInfo::VersionIds CSimpleDeviceInfo::GetVersionIds() const
{
	return VersionIds();
}


bool CSimpleDeviceInfo::GetVersionNumber(int /*versionId*/, quint32& /*result*/) const
{
	return false;
}


QString CSimpleDeviceInfo::GetVersionIdDescription(int /*versionId*/) const
{
	return QString();
}


QString CSimpleDeviceInfo::GetEncodedVersionName(int /*versionId*/, quint32 /*versionNumber*/) const
{
	return QString();
}


} // namespace imtdev


