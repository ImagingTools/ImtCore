#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
//#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtguigql
{


class CGetStyleDataControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGetStyleDataControllerComp);
		I_ASSIGN(m_parameterCompPtr, "Parameter", "Parameter of params data", false, "");
		I_ASSIGN(m_prefixFileNameAttrPtr, "Prefix", "Prefix of the files", false, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(iser::ISerializable, m_parameterCompPtr);
	I_TEXTATTR(m_prefixFileNameAttrPtr);
};


} // namespace imtguigql


