#pragma once


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Licenses.h>


namespace imtlicgql
{


class CLicenseCollectionControllerComp: public sdl::imtlic::Licenses::V1_0::CLicenseCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Licenses::V1_0::CLicenseCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseCollectionControllerComp);
		I_ASSIGN(m_licenseInfoFactCompPtr, "LicenseFactory", "Factory used for creation of the new license instance", true, "LicenseFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Licenses::CLicenseCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Licenses::V1_0::CLicensesListGqlRequest& licensesListRequest,
				sdl::imtlic::Licenses::V1_0::CLicenseItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Licenses::V1_0::CLicenseDefinitionData& licenseDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Licenses::V1_0::CLicenseItemGqlRequest& licenseItemRequest,
				sdl::imtlic::Licenses::V1_0::CLicenseDataPayload& representationPayload,
				QString& errorMessage) const override;

protected:
	I_FACT(imtlic::ILicenseDefinition, m_licenseInfoFactCompPtr);
};


} // namespace imtlicgql


