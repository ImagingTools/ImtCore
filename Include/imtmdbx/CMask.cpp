// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMask.h>


namespace imtmdbx
{


// external variables

int getItemCount = 0;


// public methods

CMask::CMask(
			const QString &name,
			mdbx::txn_managed &txn,
			mdbx::key_mode keyMode,
			mdbx::value_mode valueMode,
			bool hasIndex):
	CDocumentTable(name, txn, keyMode, valueMode, hasIndex)
{
}


// reimplemented (imtmdbx::IMask)

bool CMask::GetUnit(quint64 position)
{
	quint64 offset = position / 64;
	quint64 bitPosition = position % 64;

	quint64 item;

	if (!GetItem(offset, item)){
		item = 0;
	}

	return (item >> bitPosition) & (quint64)1;
}


bool CMask::SetUnit(quint64 position, bool unit)
{
	quint64 offset = position / 64;
	quint64 bitPosition = position % 64;

	quint64 item;

	if (!GetItem(offset, item)){
		item = 0;
	}

	if(unit){
		item = item | ((quint64)1 << bitPosition);
	}
	else {
		item = item & ~((quint64)1 << bitPosition);
	}

	//item = (item & ~((quint64)1 << bitPosition)) | ((quint64)unit << bitPosition);

	SetItem(offset, item);

	return true;
}


// protected methods

// reimplemented (imtmdbx::IMask)

bool CMask::GetItem(quint64 offset, quint64& item)
{
	getItemCount++;

	mdbx::slice keySlice(&offset, 8);
	bool found = m_cursor.seek(keySlice);
	if (found){
		mdbx::cursor::move_result result = m_cursor.current();
		item = result.value.as_uint64();
	}

	return found;
}


bool CMask::SetItem(quint64 offset, quint64 item)
{
	mdbx::slice keySlice(&offset, 8);
	mdbx::slice valueSlice(&item, 8);

	m_cursor.upsert(keySlice, valueSlice);

	return true;
}


bool CMask::GetNearestOffset(quint64& offset, quint64 startOffset)
{
	mdbx::slice keySlice(&startOffset, 8);
	mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
	if(!result.done){
		return false;
	}

	offset = result.key.as_uint64();

	return true;
}


bool CMask::GetNextItemOffset(quint64& offset, quint64 startOffset)
{
	mdbx::slice keySlice(&startOffset, 8);
	mdbx::cursor::move_result result = m_cursor.upper_bound(keySlice, false);
	if (!result.done){
		return false;
	}

	if(result.key.as_uint64() == startOffset){
		result = m_cursor.to_next(false);
		if (!result.done){
			return false;
		}
	}
	offset = result.key.as_uint64();

	if(offset < startOffset){
		Q_ASSERT(0);
	}

	return true;
}


bool CMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
	mdbx::slice keySlice(&startOffset, 8);
	if (m_cursor.seek(keySlice)){
		mdbx::cursor::move_result result = m_cursor.current();
		if (!result.done){
			return false;
		}

		result = m_cursor.to_previous(false);
		if (!result.done){
			return false;
		}
		offset = result.key.as_uint64();
	}

	return true;
}


} // namespace imtmdbx

