#include <imtmdbx/CDocumentTable.h>

// std includes
#include <iostream>

// Qt includes
#include <QDebug>


namespace imtmdbx
{


CDocumentTable::CDocumentTable(const QString& name, mdbx::txn_managed &txn,
							   mdbx::key_mode keyMode,
							   mdbx::value_mode valueMode,
							   bool hasIndex):
			m_tableName(name),
			m_txn(txn),
			m_keyMode(keyMode),
			m_valueMode(valueMode)
{
	m_hasIndex = hasIndex ? hasIndex : Exists(m_tableName + "Index");
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
qint64 CDocumentTable::AddDocument(const QByteArray &data)
{
	return AddDocument(data.data(), data.length());
}


qint64 CDocumentTable::AddDocument(qint64 data)
{
	return AddDocument((char*)&data, 8);
}

qint64 CDocumentTable::AddDocument(const QByteArray &key, const QByteArray &data)
{
	return AddDocument(data.data(), data.length(), key);
}


qint64 CDocumentTable::AddDocument(const char *data, int count, const QByteArray& keyStr)
{
	try{
		mdbx::cursor::move_result result = m_cursor.to_last(false);

		qint64 key = 0;


		if(result.done && keyStr.isEmpty()){
			key = result.key.as_int64();
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

	try{
		mdbx::cursor::move_result result = m_cursor.current();
		std::string value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}
	catch(...){
		doc = QByteArray();
	}

	return doc;

}


QByteArray CDocumentTable::GetDocument(qint64 key)
{
	QByteArray doc = QByteArray();
	mdbx::slice keySlice(&key, 8);

	try{
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		std::string value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}
	catch(...){
		doc = QByteArray();
	}

	return doc;
}


QByteArray CDocumentTable::GetDocument(const QByteArray &key)
{
	QByteArray doc = QByteArray();
	mdbx::slice keySlice(key.data(), key.length());

	try{
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		std::string value = result.value.as_string();
		doc = QByteArray::fromStdString(value);
	}
	catch(...){
		doc = QByteArray();
	}

	return doc;
}


bool CDocumentTable::HasRecord(qint64 key)
{
	bool ok = false;
	mdbx::slice keySlice(&key, 8);

	try{
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		ok = true;
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::HasRecord(const QByteArray &key)
{
	bool ok = false;
	mdbx::slice keySlice(key.data(), key.length());

	try{
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		ok = true;
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::UpdateDocument(qint64 key, const QByteArray &data)
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

		bool exists =  Exists(m_tableName);
		if(!exists){
			qDebug() << "Database does not exist!";
			return false;
		}

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

qint64 CDocumentTable::GetKey(const QByteArray &value)
{
	//qDebug() << "CDocumentTable::GetKey";

	try{
		qint64 key = -1;

		if(m_hasIndex){
			mdbx::slice valueSlice(value);
			try{
				mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice);
				key = result.value.as_int64();
			}
			catch(...){
				return -1;
			}
		}

		else {
			try{
				mdbx::cursor::move_result result = m_cursor.to_first();
				while(1){
					qint64 keyRead;
					std::string valueRead;
					if (result.done){
						keyRead = result.key.as_int64();
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
						result = m_cursor.to_next();
					}
				}
			}
			catch(...){
				return -1;
			}

		}

		return key;
	}
	catch(...){
		return -1;
	}

	return -1;
}

qint64 CDocumentTable::GetFirstKey()
{
	try{
		qint64 key = -1;

		mdbx::cursor::move_result result = m_cursor.to_first(false);
		if(result.done){
			key = result.key.as_int64();
			return key;
		}
	}
	catch(...){
		return -1;
	}

	return -1;
}


qint64 CDocumentTable::GetLastKey()
{
	try{
		qint64 key = -1;

		mdbx::cursor::move_result result = m_cursor.to_last(false);
		if(result.done){
			key = result.key.as_int64();
			return key;
		}

	}
	catch(...){
		return -1;
	}

	return -1;
}


QString CDocumentTable::GetStringKey(const QByteArray &value)
{
	//qDebug() << "CDocumentTable::GetKey";

	try{
		QString key = QString();

		if(m_hasIndex){
			mdbx::slice valueSlice(value);
			try{
				mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice);
				key = QString::fromStdString(result.value.as_string());
			}
			catch(...){
				return QString();
			}
		}

		else {
			try{
				mdbx::cursor::move_result result = m_cursor.to_first();
				while(1){
					QString keyRead;
					std::string valueRead;
					if (result.done){
						keyRead = QString::fromStdString(result.key.as_string());
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
						result = m_cursor.to_next();
					}
				}
			}
			catch(...){
				return QString();
			}

		}

		return key;
	}
	catch(...){
		return QString();
	}

	return QString();

}


bool CDocumentTable::CreateIndex()
{
	qDebug() << "CDocumentTable::CreateIndex";

	try{

		bool exists = Exists(m_tableName);
		if(!exists){
			qDebug() << "Database does not exist!";
			return false;
		}

		if(m_hasIndex){
			qDebug() << "Index exists!";
			return false;
		}

		mdbx::cursor::move_result result = m_cursor.to_first();

		while(1){
			qint64 keyRead;
			std::string valueRead;
			if (result.done){
				keyRead = result.key.as_int64();
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


bool CDocumentTable::MoveTo(qint64 key)
{
	bool ok = false;

	try{
		mdbx::slice keySlice(&key, 8);
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::MoveTo(const QByteArray &key)
{
	bool ok = false;

	try{
		mdbx::slice keySlice(key.data(), key.length());
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::MoveToFirst()
{
	bool ok = false;

	try{
		mdbx::cursor::move_result result = m_cursor.to_first(false);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
	}

	return ok;

}


bool CDocumentTable::MoveToLast()
{
	bool ok = false;

	try{
		mdbx::cursor::move_result result = m_cursor.to_last(false);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::MoveToLowerBound(qint64 key)
{
	bool ok = false;

	try{
		mdbx::slice keySlice(&key, 8);
		mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::MoveToLowerBound(const QByteArray &key)
{
	bool ok = false;

	try{
		mdbx::slice keySlice(key.data(), key.length());
		mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);
		if(result.done){
			ok = true;
		}

	}
	catch(...){
		ok = false;
	}

	return ok;
}


bool CDocumentTable::MoveToNext()
{
	bool ok = false;

	try{
		mdbx::cursor::move_result result = m_cursor.to_next(false);
		if(result.done){
			ok = true;
		}
	}
	catch(...){
		ok = false;
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

