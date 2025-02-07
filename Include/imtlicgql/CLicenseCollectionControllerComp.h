#pragma once


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Licenses.h>


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
				sdl::imtlic::Licenses::CLicenseItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& licenseDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Licenses::CLicenseItemGqlRequest& licenseItemRequest,
				sdl::imtlic::Licenses::CLicenseDataPayload::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtlic::Licenses::CLicenseUpdateGqlRequest& licenseUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
private:
	bool FillObjectFromRepresentation(
				const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtlic::ILicenseDefinition, m_licenseInfoFactCompPtr);
};


} // namespace imtlicgql


