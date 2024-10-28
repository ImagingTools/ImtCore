#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


/**
	Returns headers for a collection of objects.
*/
class CHeadersDataProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHeadersDataProviderComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "HeadersIds", "Headers Ids", true);
		I_ASSIGN_MULTI_0(m_headersSearchByFilterAttrPtr, "SearchByFilterIds", "Search by filter", false);
		I_ASSIGN_MULTI_0(m_headersNamesAttrPtr, "HeadersNames", "Headers names", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_MULTIATTR(QString, m_headersIdsAttrPtr);
	I_MULTIATTR(QString, m_headersSearchByFilterAttrPtr);
	I_MULTIATTR(QString, m_headersNamesAttrPtr);
};


} // namespace imtservergql


