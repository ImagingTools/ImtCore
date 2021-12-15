#include <imtbase/CTempDir.h>


// Qt includes
#include <QtCore/QUuid>


namespace imtbase
{


CTempDir::CTempDir(const QString& tempSubDir, bool autoRemove)
{
	QString path = QDir::tempPath() + "/" + tempSubDir + "/" + QUuid::createUuid().toString();

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


QString CTempDir::Path() const
{
	if (m_dir.path() != "."){
		return m_dir.path();
	}

	return QString();
}


} // namespace imtbase


