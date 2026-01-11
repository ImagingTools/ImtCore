/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/

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


