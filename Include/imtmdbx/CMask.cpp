#include <imtmdbx/CMask.h>

// Qt includes
#include <QtCore/QDebug>


namespace imtmdbx
{

int getItemCount = 0;

CMask::CMask(const QString &name, mdbx::txn_managed &txn, mdbx::key_mode keyMode, mdbx::value_mode valueMode, bool hasIndex):
	CDocumentTable(name, txn, keyMode, valueMode, hasIndex)
{
}


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

	item = item | ((quint64)1 << bitPosition);
	//item = (item & ~((quint64)1 << bitPosition)) | ((quint64)unit << bitPosition);

	SetItem(offset, item);

	return true;
}


bool CMask::GetItem(quint64 offset, quint64& item)
{

	getItemCount++;

	mdbx::slice keySlice(&offset, 8);

	if (m_cursor.seek(keySlice)){
		mdbx::cursor::move_result result = m_cursor.current();
		item = result.value.as_uint64();
	}
	else {
		return false;
	}

	return true;
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

	if (m_cursor.seek(keySlice)){
		mdbx::cursor::move_result result = m_cursor.current();
		if (result.done){
			result = m_cursor.to_next(false);
			if (!result.done){
				return false;
			}
			offset = result.key.as_uint64();
		}
		else{
			return false;
		}
	}
	else {
		return false;
	}

	if(offset < startOffset){
		Q_ASSERT(0);
	}
	return true;


	/******************************lower_bound*****************************/

//	mdbx::slice keySlice(&startOffset, 8);

//		mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);

//		if (result.done){
//			if(result.key.as_uint64() == startOffset){
//				result = m_cursor.to_next(false);
//				if (!result.done){
//					return false;
//				}
//			}
//			offset = result.key.as_uint64();
//		}
//		else{
//			return false;
//		}

//		if(offset < startOffset){
//			Q_ASSERT(0);
//		}

//	return true;
}


bool CMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
	mdbx::slice keySlice(&startOffset, 8);

	if (m_cursor.seek(keySlice)){
		mdbx::cursor::move_result result = m_cursor.current();
		if (result.done){
			result = m_cursor.to_previous(false);
			if (!result.done){
				return false;
			}
			offset = result.key.as_uint64();
		}
		else{
			return false;
		}
	}

	return true;
}


}//namespace imtmdbx
