// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imttag/ISystemTagSeeder.h>
#include <imttag/ITag.h>


namespace imttag
{


/**
	Seeds the default system tags with fixed IDs.
	The lists of IDs, names, colors and descriptions are matched by index.
*/
class CSystemTagSeederComp:
			public ilog::CLoggerComponentBase,
			virtual public ISystemTagSeeder
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemTagSeederComp);
		I_REGISTER_INTERFACE(ISystemTagSeeder);
		I_ASSIGN(m_tagCollectionCompPtr, "TagCollection", "Tag catalog", true, "TagCollection");
		I_ASSIGN(m_tagFactoryCompPtr, "TagFactory", "Factory of tags", true, "Tag");
		I_ASSIGN_MULTI_0(m_tagIdsAttrPtr, "TagIds", "Fixed IDs of the default system tags", true);
		I_ASSIGN_MULTI_0(m_tagNamesAttrPtr, "TagNames", "Names of the default system tags", true);
		I_ASSIGN_MULTI_0(m_tagColorsAttrPtr, "TagColors", "Hex colors of the default system tags", true);
		I_ASSIGN_MULTI_0(m_tagDescriptionsAttrPtr, "TagDescriptions", "Descriptions of the default system tags", false);
	I_END_COMPONENT;

	CSystemTagSeederComp();

	// reimplemented (imttag::ISystemTagSeeder)
	virtual bool EnsureSystemTags() override;

private:
	bool TagExists(const QByteArray& tagId) const;

	I_REF(imtbase::IObjectCollection, m_tagCollectionCompPtr);
	I_FACT(ITag, m_tagFactoryCompPtr);
	I_MULTIATTR(QByteArray, m_tagIdsAttrPtr);
	I_MULTITEXTATTR(m_tagNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_tagColorsAttrPtr);
	I_MULTITEXTATTR(m_tagDescriptionsAttrPtr);

	QMutex m_mutex;
	bool m_isSeeded;
};


} // namespace imttag


