// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


