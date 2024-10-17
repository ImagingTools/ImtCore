#pragma once


// ImtCore includes
#include <imtgql/CSerializableObjectCollectionControllerComp.h>


namespace imtauthgql
{


class CUserSerializableCollectionControllerComp: public imtgql::CSerializableObjectCollectionControllerComp
{
public:
	typedef imtgql::CSerializableObjectCollectionControllerComp BaseClass;

	I_BEGIN_COMPONENT(CUserSerializableCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CSerializableObjectCollectionControllerComp)
	virtual void CustomProcessObject(const imtgql::CGqlRequest& gqlRequest, iser::ISerializable& object) const override;
};


} // namespace imtauthgql


