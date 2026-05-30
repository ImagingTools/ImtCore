// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Licenses_fwd.h>


namespace imtlicgql
{


class CLicenseCollectionControllerComp: public sdl::V1_0::imtlic::CLicenseCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtlic::CLicenseCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseCollectionControllerComp);
		I_ASSIGN(m_licenseInfoFactCompPtr, "LicenseFactory", "Factory used for creation of the new license instance", true, "LicenseFactory");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", false, "ProductCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtlic::CLicenseCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtlic::CLicensesListGqlRequest& licensesListRequest,
				sdl::V1_0::imtlic::CLicenseItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtlic::CLicenseDefinitionData& licenseDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtlic::CLicenseItemGqlRequest& licenseItemRequest,
				sdl::V1_0::imtlic::CLicenseDefinitionData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtlic::CLicenseUpdateGqlRequest& licenseUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::imtlic::CLicenseDefinitionData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtlic::ILicenseDefinition, m_licenseInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtlicgql


