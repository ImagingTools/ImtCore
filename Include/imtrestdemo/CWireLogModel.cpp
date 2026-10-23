// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CWireLogModel.h>


// Qt includes
#include <QtCore/QCoreApplication>


namespace imtrestdemo
{


CWireLogModel::CWireLogModel(QObject* parentPtr)
	:QAbstractListModel(parentPtr)
{
	m_flushTimer.setSingleShot(true);
	m_flushTimer.setInterval(150);

	connect(&m_flushTimer, &QTimer::timeout, this, &CWireLogModel::FlushDirtyRow);
}


void CWireLogModel::AddData(int connectionId, int kind, const QByteArray& data)
{
	if (data.isEmpty()){
		return;
	}

	// The same direction of the same connection goes on
	if (!m_entries.isEmpty()){
		Entry& last = m_entries.last();
		if (last.connectionId == connectionId && last.kind == kind){
			const bool isHeadGrowing = last.head.size() < s_previewLimit;
			if (isHeadGrowing){
				last.head += data.left(s_previewLimit - int(last.head.size()));
				last.isEscapedHeadValid = false;
			}

			last.size += data.size();

			const int lastRow = int(m_entries.size()) - 1;
			if (isHeadGrowing){
				const QModelIndex lastIndex = index(lastRow);
				Q_EMIT dataChanged(lastIndex, lastIndex, {R_SIZE, R_TEXT, R_IS_TRUNCATED});
			}
			else{
				m_dirtyRow = lastRow;

				if (!m_flushTimer.isActive()){
					m_flushTimer.start();
				}
			}

			return;
		}
	}

	FlushDirtyRow();

	Entry entry;
	entry.connectionId = connectionId;
	entry.kind = kind;
	entry.time = QDateTime::currentDateTime();
	entry.size = data.size();
	entry.head = data.left(s_previewLimit);

	beginInsertRows(QModelIndex(), int(m_entries.size()), int(m_entries.size()));
	m_entries.append(entry);
	endInsertRows();

	Q_EMIT countChanged();
}


void CWireLogModel::AddEvent(int connectionId, const QString& text)
{
	FlushDirtyRow();

	Entry entry;
	entry.connectionId = connectionId;
	entry.kind = K_EVENT;
	entry.time = QDateTime::currentDateTime();
	entry.eventText = text;

	beginInsertRows(QModelIndex(), int(m_entries.size()), int(m_entries.size()));
	m_entries.append(entry);
	endInsertRows();

	Q_EMIT countChanged();
}


void CWireLogModel::clear()
{
	m_flushTimer.stop();
	m_dirtyRow = -1;

	beginResetModel();
	m_entries.clear();
	endResetModel();

	Q_EMIT countChanged();
}


void CWireLogModel::Retranslate()
{
	if (!m_entries.isEmpty()){
		Q_EMIT dataChanged(index(0), index(m_entries.size() - 1));
	}
}


QString CWireLogModel::getAllText() const
{
	QString retVal;

	for (const Entry& entry : m_entries){
		const QString direction = entry.kind == K_CLIENT_TO_SERVER ? QCoreApplication::translate("CWireLogModel", "client -> server")
					: entry.kind == K_SERVER_TO_CLIENT ? QCoreApplication::translate("CWireLogModel", "server -> client") : QCoreApplication::translate("CWireLogModel", "event");

		retVal += QCoreApplication::translate("CWireLogModel", "#%1 %2 %3 [%4 bytes]\n%5\n\n")
					.arg(entry.connectionId)
					.arg(entry.time.toString("hh:mm:ss.zzz"), direction)
					.arg(entry.size)
					.arg(GetEntryText(entry));
	}

	return retVal;
}


QString CWireLogModel::EscapeBytes(const QByteArray& data)
{
	QString retVal;
	retVal.reserve(int(data.size()) + 16);

	for (const char byteChar : data){
		const unsigned char byte = static_cast<unsigned char>(byteChar);

		if (byte == '\r'){
			retVal += QStringLiteral("\\r");
		}
		else if (byte == '\n'){
			retVal += QStringLiteral("\\n\n");
		}
		else if (byte == '\t'){
			retVal += QStringLiteral("\\t");
		}
		else if (byte == '\\'){
			retVal += QStringLiteral("\\\\");
		}
		else if (byte >= 0x20 && byte < 0x7f){
			retVal += QChar(byte);
		}
		else{
			retVal += QStringLiteral("\\x%1").arg(byte, 2, 16, QChar('0'));
		}
	}

	return retVal;
}


// reimplemented (QAbstractItemModel)

int CWireLogModel::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : int(m_entries.size());
}


QVariant CWireLogModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()){
		return QVariant();
	}

	const Entry& entry = m_entries[index.row()];

	switch (role){
	case R_CONNECTION_ID:
		return entry.connectionId;
	case R_KIND:
		return entry.kind;
	case R_TIME:
		return entry.time.toString("hh:mm:ss.zzz");
	case R_SIZE:
		return entry.size;
	case R_TEXT:
		return GetEntryText(entry);
	case R_IS_TRUNCATED:
		return entry.size > entry.head.size();
	default:
		return QVariant();
	}
}


QHash<int, QByteArray> CWireLogModel::roleNames() const
{
	return {
		{R_CONNECTION_ID, "connectionId"},
		{R_KIND, "kind"},
		{R_TIME, "time"},
		{R_SIZE, "size"},
		{R_TEXT, "text"},
		{R_IS_TRUNCATED, "isTruncated"}};
}


// private methods

QString CWireLogModel::GetEntryText(const Entry& entry)
{
	if (entry.kind == K_EVENT){
		return entry.eventText;
	}

	if (!entry.isEscapedHeadValid){
		entry.escapedHead = EscapeBytes(entry.head);
		entry.isEscapedHeadValid = true;
	}

	QString retVal = entry.escapedHead;
	if (entry.size > entry.head.size()){
		retVal += QCoreApplication::translate("CWireLogModel", "\n... %1 more bytes are not shown").arg(entry.size - entry.head.size());
	}

	return retVal;
}


void CWireLogModel::FlushDirtyRow()
{
	if (m_dirtyRow >= 0 && m_dirtyRow < m_entries.size()){
		const QModelIndex dirtyIndex = index(m_dirtyRow);
		Q_EMIT dataChanged(dirtyIndex, dirtyIndex, {R_SIZE, R_TEXT, R_IS_TRUNCATED});
	}

	m_dirtyRow = -1;
	m_flushTimer.stop();
}


} // namespace imtrestdemo
