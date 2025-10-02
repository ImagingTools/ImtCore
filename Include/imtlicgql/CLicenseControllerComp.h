#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>

#undef GetObject

namespace imtlicgql
{


class CLicenseControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtlicgql


