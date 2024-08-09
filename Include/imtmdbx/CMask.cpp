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
	try{
		quint64 offset = position / 64;
		quint64 bitPosition = position % 64;

		quint64 item;

		if (!GetItem(offset, item)){
			item = 0;
		}

		return (item >> bitPosition) & (quint64)1;
	}
	catch(...){

	}
	return false;
}


bool CMask::SetUnit(quint64 position, bool unit)
{
	try{
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
	catch(...){
		//return false;
	}
	return false;
}


bool CMask::GetItem(quint64 offset, quint64& item)
{
	getItemCount++;
	try{
		mdbx::slice keySlice(&offset, 8);

		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		item = result.value.as_uint64();

		return true;
	}
	catch(...){
		//item = 0;
	}

	return false;
}


bool CMask::SetItem(quint64 offset, quint64 item)
{
	try{
		mdbx::slice keySlice(&offset, 8);
		mdbx::slice valueSlice(&item, 8);

		m_cursor.upsert(keySlice, valueSlice);

		return true;
	}
	catch(...){

	}

	return false;
}


bool CMask::GetNearestOffset(quint64& offset, quint64 startOffset)
{
	try{
//		if(startOffset == 0){
//			mdbx::cursor::move_result result  = m_cursor.to_first(false);
//			if(!result.done){
//				return false;
//			}
//		}
		mdbx::slice keySlice(&startOffset, 8);

		mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
		if(!result.done){
			return false;
		}

		offset = result.key.as_uint64();

		return true;
	}
	catch(...){

	}

	return false;
}

bool CMask::GetNextItemOffset(quint64& offset, qint64 startOffset)
{
	try{
		mdbx::slice keySlice(&startOffset, 8);

		mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
		if (result.done){
//			if (m_cursor.eof()){
//				return false;
//			}
			result = m_cursor.to_next(false);
			if (!result.done){
				return false;
			}
		}
		else{
			return false;
		}

		offset = result.key.as_uint64();

		return true;
	}
	catch(...){

	}

	return false;
}


bool CMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
	try{
		quint64 lastOffset = startOffset;

		if(lastOffset == 0xffffffff){
			mdbx::cursor::move_result resultLast = m_cursor.to_last(true);
			lastOffset = resultLast.key.as_uint64();
		}
		mdbx::slice keySlice(&lastOffset, 8);

		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		result = m_cursor.to_previous(true);
		offset = result.key.as_uint64();

		return true;
	}
	catch(...){

	}
	return false;
}


}//namespace imtmdbx
