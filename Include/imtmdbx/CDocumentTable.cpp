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


// reimplemented (imtmdbx::IDocumentTable)
qint64 CDocumentTable::AddDocument(const QByteArray &data)
{
	return AddDocument(data.data(), data.length());
}


qint64 CDocumentTable::AddDocument(qint64 data)
{
	return AddDocument((char*)&data, 8);
}


qint64 CDocumentTable::AddDocument(const char *data, int count)
{
	try{
		mdbx::cursor::move_result result = m_cursor.to_last(false);

		qint64 key = 0;

		if(result.done){
			key = result.key.as_int64();
			key++;
		}

		mdbx::slice keySlice(&key, 8);
		mdbx::slice valueSlice(data, count);
		m_txn.put(m_mapHandle, keySlice, valueSlice, mdbx::put_mode::upsert);

		if(m_hasIndex){
			mdbx::slice keySliceIndex(data, count);
			mdbx::slice valueSliceIndex(&key, 8);
			m_txn.put(m_mapHandleIndex, keySliceIndex, valueSliceIndex, mdbx::put_mode::upsert);
		}

		return key;
	}
	catch(...){

	}

	return -1;
}


QByteArray CDocumentTable::GetDocument(qint64 key)
{
	QByteArray doc = QByteArray();
	mdbx::slice keySlice(&key, 8);

	try{
		mdbx::cursor::move_result result = m_cursor.find(keySlice);
		std::string value = result.value.as_string();
		doc = QByteArray(value.data());
	}
	catch(...){
		doc = QByteArray();
	}

	return doc;
}

bool CDocumentTable::UpdateDocument(qint64 key, const QByteArray &data)
{
	qDebug() << "CDocumentTable::UpdateDocument";

	try{

		bool exists =  Exists(m_tableName);
		if(!exists){
			qDebug() << "Database does not exist!";
			return false;
		}

		mdbx::slice keySlice(&key, 8);
		mdbx::slice valueSlice(data.data(), data.length());

		mdbx::map_handle mapHandle = m_txn.open_map(m_tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);

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
				key = -1;
			}
		}

		if(key < 0){
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


bool CDocumentTable::Exists(const QString& name)
{
	bool ok = true;

	try{
		mdbx::map_handle mapHandle = m_txn.open_map(name.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
	}
	catch (...){
		ok = false;
	}

	return ok;

}


}//namespace imtmdbx

