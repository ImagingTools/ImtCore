#include <imtmdbx/CMask.h>


namespace imtmdbx
{

int getItemCount = 0;

CMask::CMask(const QString &name, mdbx::txn_managed &txn, mdbx::key_mode keyMode, mdbx::value_mode valueMode, bool hasIndex):
	CDocumentTable(name, txn, keyMode, valueMode, hasIndex)
{
}


bool CMask::GetUnit(quint64 position)
{
//	try{
		quint64 offset = position / 64;
		quint64 bitPosition = position % 64;

		quint64 item;

		if (!GetItem(offset, item)){
			item = 0;
		}

		return (item >> bitPosition) & (quint64)1;
//	}
//	catch(...){

//	}
//	return false;
}


bool CMask::SetUnit(quint64 position, bool unit)
{
//	try{
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
//	}
//	catch(...){
//		//return false;
//	}
//	return false;
}


bool CMask::GetItem(quint64 offset, quint64& item)
{
//	if (m_cache.contains(offset)){
//		item = m_cache.value(offset);
//		return true;
//	}

	getItemCount++;

//	try{
		mdbx::slice keySlice(&offset, 8);

		if (m_cursor.seek(keySlice)){
			mdbx::cursor::move_result result = m_cursor.current();
			item = result.value.as_uint64();
//			m_cache.insert(offset, item);
		}
		else {
			return false;
		}

		return true;
//	}
//	catch(...){
//		//item = 0;
//	}

//	return false;
}


bool CMask::SetItem(quint64 offset, quint64 item)
{
//	try{
		mdbx::slice keySlice(&offset, 8);
		mdbx::slice valueSlice(&item, 8);

		m_cursor.upsert(keySlice, valueSlice);

		return true;
//	}
//	catch(...){

//	}

//	return false;
}


bool CMask::GetNearestOffset(quint64& offset, quint64 startOffset)
{
//	try{
		mdbx::slice keySlice(&startOffset, 8);
		mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
		if(!result.done){
			return false;
		}

		offset = result.key.as_uint64();

		return true;
//	}
//	catch(...){

//	}

//	return false;
}

bool CMask::GetNextItemOffset(quint64& offset, qint64 startOffset)
{
//	try{
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

		return true;
//	}
//	catch(...){

//	}

//	return false;
}


bool CMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
//	try{
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
//	}
//	catch(...){

//	}
//	return false;
}


}//namespace imtmdbx
