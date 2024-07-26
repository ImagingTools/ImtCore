#include <imtmdbx/CDoubleMask.h>


namespace imtmdbx
{


CDoubleMask::CDoubleMask(
			const QString &name,
			mdbx::txn_managed &txn,
			mdbx::key_mode keyMode,
			mdbx::value_mode valueMode,
			bool hasIndex,
			quint32 externKey):
	CMask(name, txn, keyMode, valueMode, hasIndex)
{
	m_externKey = externKey;
}


bool CDoubleMask::GetUnit(quint64 position)
{
	quint64 localPosition = (m_externKey << 32) + position;
	bool retVal = CMask::GetUnit(localPosition);

	return retVal;

//	try{
//		quint64 offset = position / 64;
//		quint64 bitPosition = position % 64;

//		quint64 item;

//		if (!GetItem(offset, item)){
//			item = 0;
//		}

//		return (item >> bitPosition) & (quint64)1;
//	}
//	catch(...){

//	}
//	return false;

}


bool CDoubleMask::SetUnit(quint64 position, bool unit)
{
	quint64 localPosition = (m_externKey << 32) + position;
	bool retVal = CMask::SetUnit(localPosition, unit);

	return retVal;

//	try{
//		quint64 offset = position / 64;
//		quint64 bitPosition = position % 64;

//		quint64 item;

//		if (!GetItem(offset, item)){
//			item = 0;
//		}

//		item = item | ((quint64)1 << bitPosition);
//		//item = (item & ~((quint64)1 << bitPosition)) | ((quint64)unit << bitPosition);

//		SetItem(offset, item);

//		return true;
//	}
//	catch(...){
//		//return false;
//	}
//	return false;
}


bool CDoubleMask::GetItem(quint64 offset, quint64& item)
{
	//qDebug() << "CDoubleMask::GetItem";
	quint64 localItem;
	quint64 localOffset = (m_externKey << 32) + offset;
	bool retVal = CMask::GetItem(localOffset, localItem);
	return retVal;

//	try{
//		mdbx::slice keySlice(&offset, 8);

//		mdbx::cursor::move_result result = m_cursor.find(keySlice);
//		item = result.value.as_uint64();

//		return true;
//	}
//	catch(...){
//		//item = 0;
//	}

//	return false;

}


bool CDoubleMask::SetItem(quint64 offset, quint64 item)
{
	//qDebug() << "CDoubleMask::SetItem";

	quint64 localOffset = (m_externKey << 32) + offset;
	bool retVal = CMask::SetItem(localOffset, item);
	return retVal;


//	try{
//		mdbx::slice keySlice(&offset, 8);
//		mdbx::slice valueSlice(&item, 8);

//		m_cursor.upsert(keySlice, valueSlice);

//		return true;
//	}
//	catch(...){
//		return false;
//	}

}


bool CDoubleMask::GetNextItemOffset(quint64& offset, qint64 startOffset)
{
	quint64 startLocalOffset = (m_externKey << 32) + startOffset;
	quint64 localOffset;

	bool retVal = CMask::GetNextItemOffset(localOffset, startLocalOffset);
	if (retVal){
		quint64 item;
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


}//namespace imtmdbx
