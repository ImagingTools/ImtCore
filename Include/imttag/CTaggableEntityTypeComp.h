// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IEntityTypeProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionProvider.h>


namespace imttag
{


/**
	Registers a collection whose entities can be tagged.
	The entity type ID must equal the collection ID used by the client and the
	TaggableEntityType attribute of the collection's SQL delegate.
*/
class CTaggableEntityTypeComp:
			public icomp::CComponentBase,
			virtual public imtbase::IEntityTypeProvider,
			virtual public imtbase::IObjectCollectionProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTaggableEntityTypeComp);
		I_REGISTER_INTERFACE(imtbase::IEntityTypeProvider);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionProvider);
		I_ASSIGN(m_entityTypeIdAttrPtr, "EntityTypeId", "Unique identifier of the entity type (the collection ID)", true, "");
		I_ASSIGN(m_entityTypeNameAttrPtr, "EntityTypeName", "Human-readable name of the entity type", true, "");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Collection storing the entities", true, "ObjectCollection");
	I_END_COMPONENT;

	// reimplemented (imtbase::IEntityTypeProvider)
	virtual QString GetEntityTypeId() const override;
	virtual QString GetEntityTypeName() const override;

	// reimplemented (imtbase::IObjectCollectionProvider)
	virtual const imtbase::IObjectCollection* GetObjectCollection() const override;

private:
	I_ATTR(QByteArray, m_entityTypeIdAttrPtr);
	I_TEXTATTR(m_entityTypeNameAttrPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imttag


