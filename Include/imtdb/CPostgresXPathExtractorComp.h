// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/ISqlJsonXPathExtractor.h>


namespace imtdb
{


class CPostgresXPathExtractorComp: public icomp::CComponentBase, virtual public ISqlJsonXPathExtractor
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPostgresXPathExtractorComp);
		I_REGISTER_INTERFACE(ISqlJsonXPathExtractor);
	I_END_COMPONENT;

	// reimplemented (ISqlJsonXPathExtractor)
	virtual QString ExtractXPath(
				const QString& jsonKey,
				const QString& fieldId,
				QMetaType::Type metaType = QMetaType::QString,
				const QString& tableAlias = QString()) const override;
};


} // namespace imtdb


