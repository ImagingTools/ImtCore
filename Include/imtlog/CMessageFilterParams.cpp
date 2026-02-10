// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CMessageFilterParams.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtlog
{


// public methods


CMessageFilterParams::CMessageFilterParams()
{
	ResetData();
}


CMessageFilterParams::FilterMode CMessageFilterParams::GetFilterMode() const
{
	return m_mode;
}


void CMessageFilterParams::SetFilterMode(FilterMode mode)
{
	m_mode = mode;
}


QSet<int> CMessageFilterParams::GetMessageFilterIds() const
{
	return m_ids;
}


void CMessageFilterParams::SetMessageFilterIds(const QSet<int>& ids)
{
	m_ids = ids;
}


imtbase::CTimeRange CMessageFilterParams::GetFilterTimeRange() const
{
	return m_timeRange;
}


void CMessageFilterParams::SetFilterTimeRange(const imtbase::CTimeRange timeRange)
{
	m_timeRange = timeRange;
}


// reimplemented (istd::IChangeable)

int CMessageFilterParams::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CMessageFilterParams::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CMessageFilterParams* messageFilterParamsPtr = dynamic_cast<const CMessageFilterParams*>(&object);
	if (messageFilterParamsPtr != nullptr){
		m_mode = messageFilterParamsPtr->m_mode;
		m_ids = messageFilterParamsPtr->m_ids;
		m_timeRange = messageFilterParamsPtr->m_timeRange;

		return true;
	}

	return false;
}


bool CMessageFilterParams::IsEqual(const IChangeable& object) const
{
	const CMessageFilterParams* messageFilterParamsPtr = dynamic_cast<const CMessageFilterParams*>(&object);
	if (messageFilterParamsPtr != nullptr){
		return	m_mode == messageFilterParamsPtr->m_mode &&
					m_ids == messageFilterParamsPtr->m_ids &&
					m_timeRange == messageFilterParamsPtr->m_timeRange;
	}

	return false;
}


istd::IChangeableUniquePtr CMessageFilterParams::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CMessageFilterParams());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CMessageFilterParams::ResetData(CompatibilityMode /*mode*/)
{
	m_mode = FM_INCLUDE;
	m_ids.clear();
	m_timeRange = imtbase::CTimeRange();

	return true;
}


}


