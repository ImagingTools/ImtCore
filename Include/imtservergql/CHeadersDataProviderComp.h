// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		I_ASSIGN_MULTI_0(m_headerPermissionsAttrPtr, "HeaderPermissions", "Header permissions", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool CheckHeaderPermission(const QByteArray& headerId, const imtgql::IGqlContext& gqlContext) const;

private:
	I_MULTIATTR(QString, m_headersIdsAttrPtr);
	I_MULTIATTR(QString, m_headersSearchByFilterAttrPtr);
	I_MULTITEXTATTR(m_headersNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_headerPermissionsAttrPtr);
};


} // namespace imtservergql

