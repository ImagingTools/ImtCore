// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>


namespace imtbase
{


class CObjectMetaInfoCreatorCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IMetaInfoCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectMetaInfoCreatorCompBase);
		I_REGISTER_INTERFACE(imtbase::IMetaInfoCreator);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "Type-IDs of the objects supported by this meta info creator", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual TypeIds GetSupportedTypeIds() const override;

protected:
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
};


} // namespace imtbase


