// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QTimer>


namespace imtrestdemo
{


/**
	Model of the bytes that were exchanged between the client and the server (see CWireCaptureProxy), for the debug preview.
	The consecutive bytes of one direction of a connection are one entry, a change of the direction or of the connection starts the
	next one, so a request and its response are the entries one after another. An entry keeps only the beginning of the
	bytes (the limit of the preview) and counts the rest: a gigabyte download does not fill the memory of the preview.
	The text of an entry shows the control characters: CR and LF are written as \c \\r and \c \\n (with the real line break after \c \\n),
	the other bytes that are not printable are written as \c \\xNN.
*/
class CWireLogModel: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
	enum Kind
	{
		K_CLIENT_TO_SERVER = 0,
		K_SERVER_TO_CLIENT = 1,
		K_EVENT = 2
	};

	enum Role
	{
		R_CONNECTION_ID = Qt::UserRole + 1,
		R_KIND,
		R_TIME,
		R_SIZE,
		R_TEXT,
		R_IS_TRUNCATED
	};

	static constexpr int s_previewLimit = 16 * 1024;

	explicit CWireLogModel(QObject* parentPtr = nullptr);

	/**
		Add the bytes of a direction of a connection.
	*/
	void AddData(int connectionId, int kind, const QByteArray& data);

	/**
		Add an event of a connection (opened, closed).
	*/
	void AddEvent(int connectionId, const QString& text);

	Q_INVOKABLE void clear();

	/**
		The texts are made when they are asked, so after the change of the language the views must ask for them again.
	*/
	void Retranslate();

	/**
		Get the text of all entries as it is shown, for the copying.
	*/
	Q_INVOKABLE QString getAllText() const;

	/**
		Write the bytes as the text that shows the control characters.
	*/
	static QString EscapeBytes(const QByteArray& data);

	// reimplemented (QAbstractItemModel)
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role) const override;
	virtual QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void countChanged();

private:
	struct Entry
	{
		int connectionId = 0;
		int kind = K_EVENT;
		QDateTime time;
		qint64 size = 0;
		QByteArray head;
		QString eventText;

		// The text of the head is made once: the entry of a big transfer is changed many times a second
		mutable QString escapedHead;
		mutable bool isEscapedHeadValid = false;
	};

	static QString GetEntryText(const Entry& entry);

	void FlushDirtyRow();

private:
	QList<Entry> m_entries;

	// The size of a growing entry is reported to the views at most a few times a second: the views lay out the text on every change
	QTimer m_flushTimer;
	int m_dirtyRow = -1;
};


} // namespace imtrestdemo


