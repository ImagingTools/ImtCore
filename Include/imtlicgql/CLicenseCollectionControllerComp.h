#pragma once


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/CPP/Licenses/Licenses.h>


namespace imtlicgql
{


class CLicenseCollectionControllerComp: public sdl::imtlic::Licenses::CLicenseCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Licenses::CLicenseCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseCollectionControllerComp);
		I_ASSIGN(m_licenseInfoFactCompPtr, "LicenseFactory", "Factory used for creation of the new license instance", true, "LicenseFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Licenses::CLicenseCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Licenses::CLicensesListGqlRequest& licensesListRequest,
				sdl::imtlic::Licenses::CLicenseItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Licenses::CLicenseData& licenseDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Licenses::CLicenseItemGqlRequest& licenseItemRequest,
				sdl::imtlic::Licenses::CLicenseDataPayload& representationPayload,
				QString& errorMessage) const override;

protected:
	I_FACT(imtlic::ILicenseDefinition, m_licenseInfoFactCompPtr);
};


} // namespace imtlicgql


