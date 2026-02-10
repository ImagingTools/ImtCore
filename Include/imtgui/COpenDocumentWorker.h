// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>


namespace imtgui
{


class CDocumentCollectionViewDelegateComp;


struct OpenDocumentResult
{
	QByteArray objectId;
	QByteArray typeId;
	QByteArray viewTypeId;
	QString objectName;
	QString tempPath;
	QString tempFilePath;
	bool success = false;
};


/**
	Worker that performs the potentially slow open file operation.
*/
class COpenDocumentWorker: public QObject
{
	Q_OBJECT
public:
	COpenDocumentWorker(
				const CDocumentCollectionViewDelegateComp* parent,
				const QByteArray& objectId,
				const QByteArray& viewTypeId);

public Q_SLOTS:
	void Run();

Q_SIGNALS:
	void Finished(const OpenDocumentResult& result);

private:
	const CDocumentCollectionViewDelegateComp* m_parent = nullptr;
	QByteArray m_objectId;
	QByteArray m_viewTypeId;
};


} // namespace imtgui


