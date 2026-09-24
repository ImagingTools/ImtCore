// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CSystemTagSeederComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imttag/CTagNameLookup.h>


namespace imttag
{


// public methods

CSystemTagSeederComp::CSystemTagSeederComp()
	:m_isSeeded(false)
{
}


// reimplemented (imttag::ISystemTagSeeder)

bool CSystemTagSeederComp::EnsureSystemTags()
{
	QMutexLocker locker(&m_mutex);

	if (m_isSeeded){
		return true;
	}

	if (!m_tagCollectionCompPtr.IsValid() || !m_tagFactoryCompPtr.IsValid()){
		return false;
	}

	bool retVal = true;

	const int count = qMin(m_tagIdsAttrPtr.GetCount(), qMin(m_tagNamesAttrPtr.GetCount(), m_tagColorsAttrPtr.GetCount()));
	for (int i = 0; i < count; ++i){
		const QByteArray tagId = m_tagIdsAttrPtr[i];
		if (tagId.isEmpty() || TagExists(tagId)){
			continue;
		}

		const QString name = CTagNameLookup::NormalizeName(m_tagNamesAttrPtr[i]);
		if (!CTagNameLookup::FindTagIds(*m_tagCollectionCompPtr, name, nullptr).isEmpty()){
			SendWarningMessage(0, QStringLiteral("System tag '%1' was not created: the name is already used").arg(name));

			continue;
		}

		ITagUniquePtr tagPtr = m_tagFactoryCompPtr.CreateInstance();
		if (!tagPtr.IsValid()){
			return false;
		}

		const QString description = (i < m_tagDescriptionsAttrPtr.GetCount()) ? m_tagDescriptionsAttrPtr[i] : QString();

		tagPtr->SetId(tagId);
		tagPtr->SetName(name);
		tagPtr->SetColor(QString::fromUtf8(m_tagColorsAttrPtr[i]));
		tagPtr->SetDescription(description);
		tagPtr->SetSystem(true);

		const QByteArray newId = m_tagCollectionCompPtr->InsertNewObject("Tag", name, description, tagPtr.GetPtr(), tagId);
		if (newId.isEmpty()){
			SendErrorMessage(0, QStringLiteral("System tag '%1' could not be created").arg(name));

			retVal = false;
		}
	}

	m_isSeeded = retVal;

	return retVal;
}


// private methods

bool CSystemTagSeederComp::TagExists(const QByteArray& tagId) const
{
	imtcol::CDocumentIdFilter* documentIdFilterPtr = new imtcol::CDocumentIdFilter;
	documentIdFilterPtr->SetDocumentIds(QByteArrayList() << tagId);
	documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_IN);

	// A deleted default tag must stay deleted, so every document state counts.
	imtcol::CDocumentCollectionFilter* documentFilterPtr = new imtcol::CDocumentCollectionFilter;
	documentFilterPtr->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_ACTIVE);
	documentFilterPtr->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_INACTIVE);
	documentFilterPtr->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("DocumentIdFilter", documentIdFilterPtr, true);
	filterParams.SetEditableParameter("DocumentFilter", documentFilterPtr, true);

	return m_tagCollectionCompPtr->GetElementsCount(&filterParams) > 0;
}


} // namespace imttag


