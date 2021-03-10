#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Interface describing a feature package.
*/
class IFeaturePackage: virtual public IFeatureInfoProvider, virtual public iser::ISerializable
{
public:
	virtual QByteArray GetPackageId() const = 0;
	virtual void SetPackageId(const QByteArray& packageId) = 0;
};


} // namespace imtlic


