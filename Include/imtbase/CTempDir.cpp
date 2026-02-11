// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTempDir.h>


// Qt includes
#include <QtCore/QUuid>


namespace imtbase
{


CTempDir::CTempDir(const QString& tempSubDir, bool autoRemove)
{
	QString path = QDir::tempPath();
	if (!tempSubDir.isEmpty()){
		path += '/' + tempSubDir;

		m_basePath = path;
	}
	path += '/' + QUuid::createUuid().toString(QUuid::WithoutBraces);

	m_autoRemove = false;
	if (m_dir.mkpath(path)){
		m_dir.setPath(path);

		m_autoRemove = autoRemove;
	}
}


CTempDir::~CTempDir()
{
	if (m_autoRemove){
		m_dir.removeRecursively();
	}
}


QString CTempDir::GetPath() const
{
	if (m_dir.path() != "."){
		return m_dir.path();
	}

	return QString();
}


QString CTempDir::GetBasePath() const
{
	return m_basePath;
}


} // namespace imtbase


