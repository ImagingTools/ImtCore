#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtrest/imtrest.h>
#include <imtgql/CGqlRequest.h>

namespace imtgql
{


/**
	Base interface for all data controllers
	\ingroup LicenseManagement
*/
class IGqlRepresentationDataController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetModelId() const = 0;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace imtgql


