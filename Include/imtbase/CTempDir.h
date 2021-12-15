#pragma once


// Qt includes
#include <QtCore/QDir>


namespace imtbase
{


class CTempDir
{
public:
	CTempDir(const QString& tempSubDir = QString(), bool autoRemove = true);
	~CTempDir();

	QString Path() const;

private:
	QDir m_dir;
	bool m_autoRemove;
};


} // namespace imtbase


