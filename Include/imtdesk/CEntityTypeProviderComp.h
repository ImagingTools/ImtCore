// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdesk/IEntityTypeProvider.h>


namespace imtdesk
{


/**
	Configurable component that provides a single entity type.

	Instances are configured in .acc files with Id and Name attributes.
	Multiple instances are collected via I_MULTIREF in the document manager.

	\ingroup imtdesk
*/
class CEntityTypeProviderComp:
			public icomp::CComponentBase,
			virtual public IEntityTypeProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEntityTypeProviderComp)
		I_REGISTER_INTERFACE(IEntityTypeProvider)
		I_ASSIGN(m_entityTypeIdAttrPtr, "EntityTypeId", "Unique identifier of the entity type", true, "");
		I_ASSIGN(m_entityTypeNameAttrPtr, "EntityTypeName", "Human-readable name of the entity type", true, "");
	I_END_COMPONENT

	// reimplemented (imtdesk::IEntityTypeProvider)
	virtual QString GetEntityTypeId() const override;
	virtual QString GetEntityTypeName() const override;

private:
	I_ATTR(QByteArray, m_entityTypeIdAttrPtr);
	I_ATTR(QByteArray, m_entityTypeNameAttrPtr);
};


} // namespace imtdesk
