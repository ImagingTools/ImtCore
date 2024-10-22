#include <imtmdbx/CDocumentTable.h>

// std includes
#include <iostream>

// Qt includes
#include <QDebug>


namespace imtmdbx
{


CDocumentTable::CDocumentTable(const QString& name,
                               mdbx::txn_managed &txn,
							   mdbx::key_mode keyMode,
							   mdbx::value_mode valueMode,
							   bool hasIndex):
	m_tableName(name),
	m_txn(txn),
    m_hasIndex(hasIndex),
    m_keyMode(keyMode),
    m_valueMode(valueMode)

{
	bool isReadOnly = m_txn.is_readonly();

	if (isReadOnly){
		m_mapHandle = m_txn.open_map(m_tableName.toStdString(), keyMode, valueMode);
	}
	else{
		m_mapHandle = m_txn.create_map(m_tableName.toStdString(), keyMode, valueMode);
	}
	m_cursor = m_txn.open_cursor(m_mapHandle);

	if(m_hasIndex){
		if (isReadOnly){
			m_mapHandleIndex = m_txn.open_map(m_tableName.toStdString() + "Index", keyMode, valueMode);
		}
		else {
			m_mapHandleIndex = m_txn.create_map(m_tableName.toStdString() + "Index", keyMode, valueMode);
		}
		m_cursorIndex = m_txn.open_cursor(m_mapHandleIndex);
	}
}

CDocumentTable::~CDocumentTable()
{
	try{
		m_cursor.close();

		if(m_hasIndex){
			m_cursorIndex.close();
		}
	}
	catch(...){

	}


}


// reimplemented (imtmdbx::IDocumentTable)
quint64 CDocumentTable::AddDocument(const QByteArray &data)
{
	return AddDocument(data.data(), data.length());
}


quint64 CDocumentTable::AddDocument(quint64 data)
{
	return AddDocument((char*)&data, 8);
}

quint64 CDocumentTable::AddDocument(const QByteArray &key, const QByteArray &data)
{
	return AddDocument(data.data(), data.length(), key);
}


quint64 CDocumentTable::AddDocument(const char *data, int count, const QByteArray& keyStr)
{
	try{
		mdbx::cursor::move_result result = m_cursor.to_last(false);

		quint64 key = 0;


		if(result.done && keyStr.isEmpty()){
			key = result.key.as_uint64();
			key++;
		}

		mdbx::slice keySlice(&key, 8);
		mdbx::slice keyStrSlice(keyStr.data(), keyStr.length());
		mdbx::slice valueSlice(data, count);
		if(keyStr.isEmpty()){
			m_txn.put(m_mapHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
		}
		else {
			m_txn.put(m_mapHandle, keyStrSlice, valueSlice, mdbx::put_mode::upsert);
		}

		if(m_hasIndex){
			mdbx::slice keySliceIndex(data, count);
			mdbx::slice valueSliceIndex(&key, 8);
			mdbx::slice valueStrSliceIndex(keyStr.data(), keyStr.length());
			if(keyStr.isEmpty()){
				m_txn.put(m_mapHandleIndex, keySliceIndex, valueSliceIndex, mdbx::put_mode::upsert);
			}
			else {
				m_txn.put(m_mapHandleIndex, keySliceIndex, valueStrSliceIndex, mdbx::put_mode::upsert);
			}
		}

		return key;
	}
	catch(...){

	}

	return -1;
}


QByteArray CDocumentTable::GetDocument()
{
	QByteArray doc = QByteArray();

	mdbx::cursor::move_result result = m_cursor.current(false);
	if (result.done){
		std::string value;
		value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}

	return doc;
}


QByteArray CDocumentTable::GetDocument(quint64 key)
{
	QByteArray doc = QByteArray();
	mdbx::slice keySlice(&key, 8);

	mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
	if (result.done){
		std::string value;
		value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}

	return doc;
}


QByteArray CDocumentTable::GetDocument(const QByteArray &key)
{
	QByteArray doc = QByteArray();
	mdbx::slice keySlice(key.data(), key.length());

	mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
	if (result.done){
		std::string value;
		value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}

	return doc;
}


bool CDocumentTable::GetKey(quint64& key) const
{
    if (m_cursor.eof()){
        return false;
    }
    mdbx::cursor::move_result result = m_cursor.current(false);
    if (result.done){
        key = result.key.as_int64();
    }

    return true;
}


bool CDocumentTable::GetKey(QByteArray& key) const
{
    if (m_cursor.eof()){
        return false;
    }
    mdbx::cursor::move_result result = m_cursor.current(false);
    if (result.done){
        std::string value;
		value = result.key.as_string();
        key = QByteArray::fromStdString(value);
    }

    return true;
}


bool CDocumentTable::HasRecord(quint64 key)
{
	bool ok = false;
	mdbx::slice keySlice(&key, 8);

	ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::HasRecord(const QByteArray &key)
{
	bool ok = false;
	mdbx::slice keySlice(key.data(), key.length());

	ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::UpdateDocument(quint64 key, const QByteArray &data)
{
	return UpdateDocument((char*)&key, 8, data);
}


bool CDocumentTable::UpdateDocument(const QByteArray &key, const QByteArray &data)
{
	return UpdateDocument(key.data(), key.length(), data);
}


bool CDocumentTable::UpdateDocument(const char *key, int count, const QByteArray &data)
{
	qDebug() << "CDocumentTable::UpdateDocument";

	try{

		mdbx::slice keySlice(key, count);
		mdbx::slice valueSlice(data.data(), data.length());

		mdbx::map_handle mapHandle = m_txn.open_map(m_tableName.toStdString(), m_keyMode, m_valueMode);

		try{
			m_txn.update(mapHandle, keySlice, valueSlice);
		}
		catch(...){
			qDebug()<< "Index out of range";
			return false;
		}

		if(m_hasIndex){
			mdbx::cursor::move_result result = m_cursorIndex.to_last(false);
			m_txn.put(m_mapHandleIndex, valueSlice, keySlice, mdbx::put_mode::upsert);
		}
	}
	catch(...){
		return false;
	}

	return true;
}

bool CDocumentTable::GetKey(quint64& key, const QByteArray &value)
{
	//qDebug() << "CDocumentTable::GetKey";

	bool ok = false;

	if(value.isEmpty()){
        mdbx::cursor::move_result result = m_cursor.current(false);
		if(result.done){
			key = result.key.as_uint64();
			ok = true;
		}
	}

	else {//not empty value
		if(m_hasIndex){
			mdbx::slice valueSlice(value);
			mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice, false);
			if(result.done){
				key = result.value.as_uint64();
				ok = true;
			}
		}
		else {
			try{
				mdbx::cursor::move_result result = m_cursor.to_first(false);
				if(result.done){
					while(1){
						quint64 keyRead;
						std::string valueRead;
						if (result.done){
							keyRead = result.key.as_uint64();
							valueRead = result.value.as_string();
							if(valueRead.data() == value){
								key = keyRead;
								ok = true;
								break;
							}
						}
						bool onLast = m_cursor.on_last();
						if(onLast){
							break;
						}
						if(!onLast){
							result = m_cursor.to_next(false);
							if(!result.done){
								break;
							}
						}
					}
				}
			}
			catch(...){
				return false;
			}
		}
	}

	return ok;
}

bool CDocumentTable::GetFirstKey(quint64& key)
{
	bool ok = false;
	mdbx::cursor::move_result result = m_cursor.to_first(false);
	if(result.done){
		key = result.key.as_uint64();
		ok = true;
	}

	return ok;
}


bool CDocumentTable::GetLastKey(quint64& key)
{
	bool ok = false;
	mdbx::cursor::move_result result = m_cursor.to_last(false);
	if(result.done){
		key = result.key.as_uint64();
		ok = true;
	}

	return ok;
}


QByteArray CDocumentTable::GetKeyBA(const QByteArray &value)
{
	//qDebug() << "CDocumentTable::GetKey";


	QByteArray key = QByteArray();

	if(value.isEmpty()){
		mdbx::cursor::move_result result = m_cursor.current(false);
        if(result.done){
            std::string valueStr;
            valueStr = result.value.as_string();
            key = QByteArray::fromStdString(valueStr);
		}
	}
	else {//not empty value
		if(m_hasIndex){
			mdbx::slice valueSlice(value);
			mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice, false);
			if(result.done){
                std::string valueStr;
                valueStr = result.value.as_string();
                key = QByteArray::fromStdString(valueStr);
			}
		}
		else {
			mdbx::cursor::move_result result = m_cursor.to_first(false);
			if(result.done){
				while(1){
					QByteArray keyRead;
					std::string valueRead;
					if (result.done){
                        std::string valueStr;
                        valueStr = result.value.as_string();
                        keyRead = QByteArray::fromStdString(valueStr);
						valueRead = result.value.as_string();
						if(valueRead.data() == value){
							key = keyRead;
							break;
						}
					}
					bool onLast = m_cursor.on_last();
					if(onLast){
						break;
					}
					if(!onLast){
						result = m_cursor.to_next(false);
						if(!result.done){
							break;
						}
					}
				}
			}
		}
	}

	return key;
}


bool CDocumentTable::CreateIndex()
{
	qDebug() << "CDocumentTable::CreateIndex";

	try{

		if(m_hasIndex){
			qDebug() << "Index exists!";
			return false;
		}

		mdbx::cursor::move_result result = m_cursor.to_first();

		while(1){
			quint64 keyRead;
			std::string valueRead;
			if (result.done){
				keyRead = result.key.as_uint64();
				valueRead = result.value.as_string();
				mdbx::slice keyIndex (valueRead.data());
				mdbx::slice valueIndex (&keyRead, 8);
				mdbx::map_handle mapHandleIndex = m_txn.create_map(m_tableName.toStdString() + "Index", m_keyMode, m_valueMode);
				m_txn.put(mapHandleIndex, keyIndex, valueIndex, mdbx::put_mode::upsert);
			}
			bool onLast = m_cursor.on_last();
			if(onLast){
				break;
			}
			if(!onLast){
				result = m_cursor.to_next();
			}
		}


	}
	catch(...){
		return false;
	}

	return true;
}


bool CDocumentTable::MoveTo(quint64 key)
{
	bool ok = false;

	mdbx::slice keySlice(&key, 8);
	ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::MoveTo(const QByteArray &key)
{
	bool ok = false;

	mdbx::slice keySlice(key.data(), key.length());
	ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::MoveToFirst()
{
	bool ok = false;

	mdbx::cursor::move_result result = m_cursor.to_first(false);
	if(result.done){
		ok = true;
	}

	return ok;

}


bool CDocumentTable::MoveToLast()
{
	bool ok = false;

	mdbx::cursor::move_result result = m_cursor.to_last(false);
	if(result.done){
		ok = true;
	}

	return ok;
}


bool CDocumentTable::MoveToLowerBound(quint64 key)
{
	bool ok = false;

	mdbx::slice keySlice(&key, 8);
	mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
	if(result.done){
		ok = true;
	}

	return ok;
}


bool CDocumentTable::MoveToLowerBound(const QByteArray &key)
{
	bool ok = false;

	mdbx::slice keySlice(key.data(), key.length());
	mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
	if(result.done){
		ok = true;
	}

	return ok;
}


bool CDocumentTable::MoveToNext()
{
	bool ok = false;

	mdbx::cursor::move_result result = m_cursor.to_next(false);
	if(result.done){
		ok = true;
	}

	return ok;
}


bool CDocumentTable::Exists(const QString& name)
{
	bool ok = true;

    try{
        mdbx::map_handle mapHandle = m_txn.open_map(name.toStdString(), m_keyMode, m_valueMode);
    }
    catch (...){
        ok = false;
    }

	return ok;
}


bool CDocumentTable::CloseTable(mdbx::env_managed& env)
{
	try{
		m_cursor.close();
		env.close_map(m_mapHandle);

		return true;
	}
	catch(...){
		//qDebug() << "НЕ УДАЛОСЬ ЗАКРЫТЬ ТАБЛИЦУ";
	}
	return false;
}


}//namespace imtmdbx

