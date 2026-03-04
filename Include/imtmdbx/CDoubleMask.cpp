// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CDoubleMask.h>


namespace imtmdbx
{


CDoubleMask::CDoubleMask(const QString &name,
			mdbx::txn_managed &txn,
			quint32 externKey,
			mdbx::key_mode keyMode,
			mdbx::value_mode valueMode,
			bool hasIndex):
	CMask(name, txn, keyMode, valueMode, hasIndex)
{
	m_externKey = externKey;
}


bool CDoubleMask::GetItem(quint64 offset, quint64& item)
{
	quint64 localOffset = (m_externKey << 32) + offset;
	bool retVal = CMask::GetItem(localOffset, item);

	return retVal;
}


bool CDoubleMask::SetItem(quint64 offset, quint64 item)
{
	quint64 localOffset = (m_externKey << 32) + offset;
	bool retVal = CMask::SetItem(localOffset, item);

	return retVal;
}


bool CDoubleMask::GetNearestOffset(quint64& offset, quint64 startOffset)
{
	quint64 startLocalOffset = (m_externKey << 32) + startOffset;
	quint64 localOffset = 0;

	bool retVal = CMask::GetNearestOffset(localOffset, startLocalOffset);
	if (retVal){
		quint64 externKey = localOffset >> 32;
		if(externKey == m_externKey){
			offset = localOffset - (externKey << 32);
		}
		else {
			retVal = false;
		}
	}
	else {
		retVal = false;
	}

	return retVal;
}


bool CDoubleMask::GetNextItemOffset(quint64& offset, quint64 startOffset)
{
	quint64 externKeyTEST = m_externKey;

	quint64 startLocalOffset = (m_externKey << 32);
	startLocalOffset += startOffset;
	quint64 localOffset = 0;


	bool retVal = CMask::GetNextItemOffset(localOffset, startLocalOffset);
	if (retVal){
		quint64 externKey = localOffset >> 32;
		if(externKey == m_externKey){
			offset = localOffset - (externKey << 32);
		}
		else {
			retVal = false;
		}
	}
	else {
		retVal = false;
	}

	if(retVal && offset == imtmdbx::QUINT64_MAX){
		Q_ASSERT(0);
	}

	if(retVal && offset < startOffset){
		Q_ASSERT(0);
	}
	return retVal;
}


bool CDoubleMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
	quint64 startLocalOffset = (m_externKey << 32) + startOffset;
	quint64 localOffset;

	bool retVal = CMask::GetPreviosItemOffset(localOffset, startLocalOffset);
	if (retVal){
		quint64 item = 0;
		if(GetItem(item, localOffset)){
			item = item >> 32;
			if(item == m_externKey){
				offset = localOffset - item;
			}
			else {
				retVal = false;
			}
		}
		else {
			retVal = false;
		}
	}
	else {
		retVal = false;
	}
	return retVal;
}

bool CDoubleMask::SetExternalKey(quint64 externalKey)
{
	m_externKey = externalKey;
	return true;
}


}//namespace imtmdbx
