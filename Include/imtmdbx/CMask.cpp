#include <imtmdbx/CMask.h>


namespace imtmdbx
{


CMask::CMask(const QString &name, mdbx::txn_managed &txn, mdbx::key_mode keyMode, mdbx::value_mode valueMode):
	CDocumentTable(name, txn, keyMode, valueMode)
{

}


bool CMask::GetUnit(qint64 position)
{
	try{
		qint64 offset = position / 64;
		qint64 bitPosition = position % 64;

		quint64 item = GetItem(offset);

		return (item >> bitPosition) & (unsigned long)1;
	}
	catch(...){
		return false;
	}

}


bool CMask::SetUnit(qint64 position, bool unit)
{
	try{
		qint64 offset = position / 64;
		qint64 bitPosition = position % 64;

		quint64 item = GetItem(offset);
		if (item < 0){
			item = 0;
		}

		item = item | ((unsigned long)1 << bitPosition);
		//item = (item & ~((unsigned long)1 << bitPosition)) | ((unsigned long)unit << bitPosition);

		SetItem(offset, item);

		return true;
	}
	catch(...){
		return false;
	}

}


quint64 CMask::GetItem(qint64 offset)
{
	//qDebug() << "CMask::GetItem";

	try{
		mdbx::slice keySlice(&offset, 8);

		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		quint64 value = result.value.as_int64();

		return value;
	}
	catch(...){
		return -1;
	}
}


bool CMask::SetItem(qint64 offset, quint64 item)
{
	//qDebug() << "CMask::SetItem";

	try{
		mdbx::slice keySlice(&offset, 8);
		mdbx::slice valueSlice(&item, 8);

		m_cursor.upsert(keySlice, valueSlice);

		return true;
	}
	catch(...){
		return false;
	}

}


qint64 CMask::GetNextItemOffset(qint64 startOffset)
{
	qint64 offset = -1;

	try{
		mdbx::slice keySlice(&startOffset, 8);

		mdbx::cursor::move_result result = m_cursor.to_first();
		if (startOffset > -1){
			result = m_cursor.find(keySlice);
			result = m_cursor.to_next(true);
		}
		offset = result.key.as_int64();

		return offset;
	}
	catch(...){
		return -1;
	}
}


qint64 CMask::GetPreviosItemOffset(qint64 startOffset)
{
	qint64 offset = -1;

	try{
		qint64 lastOffset = startOffset;

		if(lastOffset == 0xffffffff){
			mdbx::cursor::move_result resultLast = m_cursor.to_last(true);
			lastOffset = resultLast.key.as_int64();
		}
		mdbx::slice keySlice(&lastOffset, 8);

		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		result = m_cursor.to_previous(true);
		offset = result.key.as_int64();

		return offset;
	}
	catch(...){
		return -1;
	}
}


}//namespace imtmdbx
