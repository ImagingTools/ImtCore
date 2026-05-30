// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CDocumentTable.h>


// STL includes
#include <limits>

// Qt includes
#include <QDebug>


namespace imtmdbx
{


// statics

static const QString s_index = QStringLiteral("Index");


// public methods

CDocumentTable::CDocumentTable(
			const QString& name,
			mdbx::txn_managed &txn,
			mdbx::key_mode keyMode,
			mdbx::value_mode valueMode,
			bool hasIndex):
	m_tableName(name),
	m_txn(txn),
	m_keyMode(keyMode),
	m_valueMode(valueMode),
	m_hasIndex(hasIndex)
{
	OpenCursors();
}


CDocumentTable::~CDocumentTable()
{
	// mdbx::cursor_managed is RAII — cursors close automatically on destruction
}


// reimplemented (imtmdbx::IDocumentTable)

quint64 CDocumentTable::AddDocument(const QByteArray& data)
{
	return AddDocument(data.data(), data.length());
}


quint64 CDocumentTable::AddDocument(quint64 data)
{
	return AddDocument((char*)&data, 8);
}


quint64 CDocumentTable::AddDocument(const QByteArray& key, const QByteArray& data)
{
	return AddDocument(data.data(), data.length(), key);
}


QByteArray CDocumentTable::GetDocument()
{
	QByteArray doc;

	mdbx::cursor::move_result result = m_cursor.current(false);
	if (result.done) {
		doc = QByteArray(static_cast<const char*>(result.value.data()),
						 static_cast<int>(result.value.size()));
	}

	return doc;
}


QByteArray CDocumentTable::GetDocument(quint64 key)
{
	QByteArray doc;
	mdbx::slice keySlice(&key, 8);

	mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
	if (result.done) {
		doc = QByteArray(static_cast<const char*>(result.value.data()),
						 static_cast<int>(result.value.size()));
	}

	return doc;
}


QByteArray CDocumentTable::GetDocument(const QByteArray& key)
{
	QByteArray doc;
	mdbx::slice keySlice(key.data(), key.length());

	mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
	if (result.done) {
		doc = QByteArray(static_cast<const char*>(result.value.data()),
						 static_cast<int>(result.value.size()));
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
	if (result.done) {
		key = QByteArray(static_cast<const char*>(result.key.data()),
						 static_cast<int>(result.key.size()));
	}

	return true;
}


bool CDocumentTable::HasRecord(quint64 key)
{
	mdbx::slice keySlice(&key, 8);

	return m_cursor.seek(keySlice);
}


bool CDocumentTable::HasRecord(const QByteArray& key)
{
	mdbx::slice keySlice(key.data(), key.length());

	return m_cursor.seek(keySlice);
}


bool CDocumentTable::UpdateDocument(quint64 key, const QByteArray& data)
{
	return UpdateDocument((char*)&key, 8, data);
}


bool CDocumentTable::UpdateDocument(const QByteArray& key, const QByteArray& data)
{
	return UpdateDocument(key.data(), key.length(), data);
}


bool CDocumentTable::GetKey(quint64& key, const QByteArray& value)
{
	bool ok = false;

	if(value.isEmpty()){
		mdbx::cursor::move_result result = m_cursor.current(false);
		ok = result.done;
		if(ok){
			key = result.key.as_uint64();
		}
	}
	else {//not empty value
		if(m_hasIndex){
			mdbx::slice valueSlice(value);
			mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice, false);
			ok = result.done;
			if(ok){
				key = result.value.as_uint64();
			}
		}
		else {
			try {
				mdbx::cursor::move_result result = m_cursor.to_first(false);
				if (result.done) {
					while (true) {
						if (result.done) {
							quint64 keyRead = result.key.as_uint64();
							QByteArray valueRead(
								static_cast<const char*>(result.value.data()),
								static_cast<int>(result.value.size()));
							if (valueRead == value) {
								key = keyRead;
								ok = true;
								break;
							}
						}

						result = m_cursor.to_next(false);
						if (!result.done) {
							break;
						}
					}
				}
			}
			catch (const mdbx::exception& e) {
				qWarning() << "CDocumentTable::GetKey: mdbx exception:" << e.what();
				return false;
			}
			catch (...) {
				qWarning() << "CDocumentTable::GetKey: unknown exception";
				return false;
			}
		}
	}

	return ok;
}

bool CDocumentTable::GetFirstKey(quint64& key)
{
	mdbx::cursor::move_result result = m_cursor.to_first(false);
	bool ok = result.done;
	if(ok){
		key = result.key.as_uint64();
	}

	return ok;
}


bool CDocumentTable::GetLastKey(quint64& key)
{
	mdbx::cursor::move_result result = m_cursor.to_last(false);
	bool ok = result.done;
	if(ok){
		key = result.key.as_uint64();
	}

	return ok;
}


QByteArray CDocumentTable::GetKeyBA(const QByteArray& value)
{
	QByteArray key;

	if (value.isEmpty()) {
		mdbx::cursor::move_result result = m_cursor.current(false);
		if (result.done) {
			key = QByteArray(static_cast<const char*>(result.key.data()),
							 static_cast<int>(result.key.size()));
		}
	}
	else {
		if (m_hasIndex) {
			mdbx::slice valueSlice(value);
			mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice, false);
			if (result.done) {
				key = QByteArray(static_cast<const char*>(result.value.data()),
								 static_cast<int>(result.value.size()));
			}
		}
		else {
			mdbx::cursor::move_result result = m_cursor.to_first(false);
			if (result.done) {
				while (true) {
					if (result.done) {
						QByteArray valueRead(
							static_cast<const char*>(result.value.data()),
							static_cast<int>(result.value.size()));
						if (valueRead == value) {
							key = QByteArray(static_cast<const char*>(result.key.data()),
											 static_cast<int>(result.key.size()));
							break;
						}
					}

					result = m_cursor.to_next(false);
					if (!result.done) {
						break;
					}
				}
			}
		}
	}

	return key;
}


bool CDocumentTable::CreateIndex()
{
	try {
		if (m_hasIndex) {
			qWarning() << "CDocumentTable::CreateIndex: index already exists for table" << m_tableName;
			return false;
		}

		mdbx::cursor::move_result result = m_cursor.to_first();

		while (true) {
			if (result.done) {
				quint64 keyRead = result.key.as_uint64();
				std::string valueRead = result.value.as_string();
				mdbx::slice keyIndex(valueRead.data());
				mdbx::slice valueIndex(&keyRead, 8);
				mdbx::map_handle mapHandleIndex = m_txn.create_map(
					(m_tableName + s_index).toStdString(),
					m_keyMode,
					m_valueMode);
				m_txn.put(mapHandleIndex, keyIndex, valueIndex, mdbx::put_mode::upsert);
			}

			result = m_cursor.to_next();
			if (!result.done) {
				break;
			}
		}
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::CreateIndex: mdbx exception:" << e.what();
		return false;
	}
	catch (...) {
		qWarning() << "CDocumentTable::CreateIndex: unknown exception";
		return false;
	}

	return true;
}


bool CDocumentTable::MoveTo(quint64 key)
{
	mdbx::slice keySlice(&key, 8);
	bool ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::MoveTo(const QByteArray& key)
{
	mdbx::slice keySlice(key.data(), key.length());
	bool ok = m_cursor.seek(keySlice);

	return ok;
}


bool CDocumentTable::MoveToFirst()
{
	mdbx::cursor::move_result result = m_cursor.to_first(false);

	return result.done;
}


bool CDocumentTable::MoveToLast()
{
	mdbx::cursor::move_result result = m_cursor.to_last(false);

	return result.done;
}


bool CDocumentTable::MoveToLowerBound(quint64 key)
{
	mdbx::slice keySlice(&key, 8);
	mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);

	return result.done;
}


bool CDocumentTable::MoveToLowerBound(const QByteArray& key)
{
	mdbx::slice keySlice(key.data(), key.length());
	mdbx::cursor::move_result result = m_cursor.lower_bound(keySlice, false);

	return result.done;
}


bool CDocumentTable::MoveToUpperBound(quint64 key)
{
	mdbx::slice keySlice(&key, 8);
	mdbx::cursor::move_result result = m_cursor.upper_bound(keySlice, false);

	return result.done;
}


bool CDocumentTable::MoveToUpperBound(const QByteArray& key)
{
	mdbx::slice keySlice(key.data(), key.length());
	mdbx::cursor::move_result result = m_cursor.upper_bound(keySlice, false);

	return result.done;
}


bool CDocumentTable::MoveToNext()
{
	mdbx::cursor::move_result result = m_cursor.to_next(false);

	return result.done;
}


bool CDocumentTable::MoveToPrevious()
{
	mdbx::cursor::move_result result = m_cursor.to_previous(false);

	return result.done;
}


bool CDocumentTable::MoveToValue(const QByteArray& value)
{
	bool ok = false;
	mdbx::slice valueSlice(value.data(), value.length());
	if(m_hasIndex){
		mdbx::cursor::move_result result = m_cursorIndex.find(valueSlice, false);
		if(result.done){
			mdbx::slice valueSliceIndex = result.value;

			if(m_valueMode == mdbx::value_mode::multi){
				ok = m_cursor.find_multivalue(valueSliceIndex, valueSlice, false);
			}
			else {
				ok = m_cursor.find(valueSliceIndex, false);
			}
		}

	}// has index
	else {// has no index
		try {
			mdbx::cursor::move_result result = m_cursor.to_first(false);
			if (result.done) {
				while (true) {
					if (result.done) {
						std::string valueRead = result.value.as_string();
						ok = valueRead == value.toStdString();
						if (ok) {
							break;
						}
					}

					result = m_cursor.to_next(false);
					if (!result.done) {
						break;
					}
				}
			}
		}
		catch (const mdbx::exception& e) {
			qWarning() << "CDocumentTable::MoveToValue: mdbx exception:" << e.what();
			return false;
		}
		catch (...) {
			qWarning() << "CDocumentTable::MoveToValue: unknown exception";
			return false;
		}
	}

	return ok;
}


bool CDocumentTable::RemoveDocument()
{
	bool ok = false;

	try {
		mdbx::slice valueSlice;
		mdbx::cursor::move_result result = m_cursor.current(false);

		if (result.done){
			valueSlice = result.value;
			ok = m_cursor.erase();
		}

		if(m_hasIndex){
			mdbx::cursor::move_result resultIndex = m_cursorIndex.find(valueSlice, false);
			if(resultIndex.done){
				ok = ok && m_cursorIndex.erase();
			}
		}

	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::RemoveDocument: mdbx exception:" << e.what();
		return false;
	}
	catch (...) {
		qWarning() << "CDocumentTable::RemoveDocument: unknown exception";
		return false;
	}

	return ok;
}

bool CDocumentTable::RemoveDocument(quint64 key, const QByteArray& value)
{
	bool ok = false;

	try {
		mdbx::slice keySlice(&key, 8);
		mdbx::slice valueSlice(value.data(), value.length());

		mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
		if (result.done){
			if(value.isEmpty()){
				valueSlice = result.value;
				ok = m_cursor.erase();
			}
			else {
				ok = m_cursor.erase(keySlice, valueSlice);
			}
		}

		if(m_hasIndex){
			mdbx::cursor::move_result resultIndex = m_cursorIndex.find(valueSlice, false);
			if(resultIndex.done){
				if(value.isEmpty()){
					ok = ok && m_cursorIndex.erase();
				}
				else {
					ok = ok && m_cursorIndex.erase(valueSlice, keySlice);
				}
			}
		}
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::RemoveDocument: mdbx exception:" << e.what();
		return false;
	}
	catch (...) {
		qWarning() << "CDocumentTable::RemoveDocument: unknown exception";
		return false;
	}

	return ok;
}


bool CDocumentTable::RemoveDocument(const QByteArray& key, const QByteArray& value)
{
	bool ok = false;

	try {
		mdbx::slice keySlice(key.data(), key.length());
		mdbx::slice valueSlice(value.data(), value.length());

		mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
		if (result.done){
			if(value.isEmpty()){
				valueSlice = result.value;
				ok = m_cursor.erase();
			}
			else {
				ok = m_cursor.erase(keySlice, valueSlice);
			}
		}

		if(m_hasIndex){
			mdbx::cursor::move_result resultIndex = m_cursorIndex.find(valueSlice, false);
			if(resultIndex.done){
				if(value.isEmpty()){
					ok = ok && m_cursorIndex.erase();
				}
				else {
					ok = ok && m_cursorIndex.erase(valueSlice, keySlice);
				}
			}
		}
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::RemoveDocument: mdbx exception:" << e.what();
		return false;
	}
	catch (...) {
		qWarning() << "CDocumentTable::RemoveDocument: unknown exception";
		return false;
	}

	return ok;
}


bool CDocumentTable::CloseTable(mdbx::env_managed& env)
{
	try {
		m_cursor.close();
		env.close_map(m_mapHandle);

		return true;
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::CloseTable: mdbx exception:" << e.what();
	}
	catch (...) {
		qWarning() << "CDocumentTable::CloseTable: unknown exception";
	}

	return false;
}


void CDocumentTable::Reopen()
{
	// Reassigning cursor_managed objects in OpenCursors() will close the old cursors via RAII
	OpenCursors();
}


void CDocumentTable::OpenCursors()
{
	bool isReadOnly = m_txn.is_readonly();
	if (isReadOnly){
		m_mapHandle = m_txn.open_map(
							m_tableName.toStdString(),
							m_keyMode,
							m_valueMode);
	}
	else{
		m_mapHandle = m_txn.create_map(
							m_tableName.toStdString(),
							m_keyMode,
							m_valueMode);
	}
	m_cursor = m_txn.open_cursor(m_mapHandle);

	if(m_hasIndex){
		if (isReadOnly){
			m_mapHandleIndex = m_txn.open_map(
									(m_tableName + s_index).toStdString(),
									m_keyMode,
									m_valueMode);
		}
		else {
			m_mapHandleIndex = m_txn.create_map(
									(m_tableName + s_index).toStdString(),
									m_keyMode,
									m_valueMode);
		}
		m_cursorIndex = m_txn.open_cursor(m_mapHandleIndex);
	}
}


// protected methods

quint64 CDocumentTable::AddDocument(const char *data, int count, const QByteArray& keyStr)
{
	try {
		mdbx::cursor::move_result result = m_cursor.to_last(false);

		quint64 key = 0;

		if (result.done && keyStr.isEmpty()) {
			key = result.key.as_uint64();
			key++;
		}

		mdbx::slice keySlice(&key, 8);
		mdbx::slice keyStrSlice(keyStr.data(), keyStr.length());
		mdbx::slice valueSlice(data, count);
		if (keyStr.isEmpty()) {
			m_txn.put(m_mapHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
		}
		else {
			m_txn.put(m_mapHandle, keyStrSlice, valueSlice, mdbx::put_mode::upsert);
		}

		if (m_hasIndex) {
			mdbx::slice keySliceIndex(data, count);
			mdbx::slice valueSliceIndex(&key, 8);
			mdbx::slice valueStrSliceIndex(keyStr.data(), keyStr.length());
			if (keyStr.isEmpty()) {
				m_txn.put(m_mapHandleIndex, keySliceIndex, valueSliceIndex, mdbx::put_mode::upsert);
			}
			else {
				m_txn.put(m_mapHandleIndex, keySliceIndex, valueStrSliceIndex, mdbx::put_mode::upsert);
			}
		}

		return key;
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::AddDocument: mdbx exception:" << e.what();
	}
	catch (...) {
		qWarning() << "CDocumentTable::AddDocument: unknown exception";
	}

	return std::numeric_limits<quint64>::max();
}


bool CDocumentTable::UpdateDocument(const char *key, int count, const QByteArray& data)
{
	try {
		mdbx::slice keySlice(key, count);
		mdbx::slice valueSlice(data.data(), data.length());
		mdbx::slice valueSliceOld;

		mdbx::cursor::move_result result = m_cursor.find(keySlice, false);
		bool oldValueFound = result.done;
		if (oldValueFound) {
			valueSliceOld = result.value;
		}

		try {
			m_txn.update(m_mapHandle, keySlice, valueSlice);
		}
		catch (const mdbx::exception& e) {
			qWarning() << "CDocumentTable::UpdateDocument: update failed:" << e.what();
			return false;
		}

		if (m_hasIndex) {
			if (oldValueFound) {
				mdbx::cursor::move_result resultIndex = m_cursorIndex.find(valueSliceOld, false);
				if (resultIndex.done) {
					m_cursorIndex.erase();
				}
				resultIndex = m_cursorIndex.to_last(false);
				if (resultIndex.done) {
					m_txn.put(m_mapHandleIndex, valueSlice, keySlice, mdbx::put_mode::upsert);
				}
			}
		}
	}
	catch (const mdbx::exception& e) {
		qWarning() << "CDocumentTable::UpdateDocument: mdbx exception:" << e.what();
		return false;
	}
	catch (...) {
		qWarning() << "CDocumentTable::UpdateDocument: unknown exception";
		return false;
	}

	return true;
}


bool CDocumentTable::Exists(const QString& name)
{
	bool ok = true;

	try {
		m_txn.open_map(name.toStdString(), m_keyMode, m_valueMode);
	}
	catch (...) {
		ok = false;
	}

	return ok;
}


}//namespace imtmdbx

