#pragma once


// ImtCore includes
#include <imtservergql/CSerializableObjectCollectionControllerComp.h>


namespace imtauthgql
{


class CUserSerializableCollectionControllerComp: public imtservergql::CSerializableObjectCollectionControllerComp
{
public:
	typedef imtservergql::CSerializableObjectCollectionControllerComp BaseClass;

	I_BEGIN_COMPONENT(CUserSerializableCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CSerializableObjectCollectionControllerComp)
	virtual void CustomProcessObject(const imtgql::CGqlRequest& gqlRequest, iser::ISerializable& object) const override;
};


} // namespace imtauthgql


