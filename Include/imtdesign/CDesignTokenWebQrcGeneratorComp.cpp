#include <imtdesign/CDesignTokenWebQrcGeneratorComp.h>


// Qt includes
#include <QtXml/QtXml>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdesign
{


int CDesignTokenWebQrcGeneratorComp::Exec()
{
	if(!m_argumentParserAttrPtr->IsWebGenerateResourceRequired()){
		return 0;
	}

	QStringList typesOfResources = {"Icons"};
	QByteArray dirSeparator = QByteArray(1,QDir::separator().toLatin1());

	QByteArray outputDirPath = m_argumentParserAttrPtr->GetOutputDirectoryPath();
	QByteArray resourceDirPath = outputDirPath + dirSeparator + "Resources";
	QByteArray projectName = m_argumentParserAttrPtr->GetProjectName();
	QVector<QByteArray> styles;

	QDir::Filters filters = QDir::Filter::Dirs;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList dirs = QDir(resourceDirPath).entryInfoList(filters, sort);

	for (const QFileInfo& fileInFo: ::qAsConst(dirs)){
		if (typesOfResources.contains(fileInFo.fileName())){
			QFileInfoList iconsDirs = QDir(fileInFo.absoluteFilePath()).entryInfoList(filters, sort);
			for (const QFileInfo& iconsFileInFo: ::qAsConst(iconsDirs)){
				if (!iconsFileInFo.baseName().isEmpty()){
					styles << iconsFileInFo.fileName().toUtf8();
				}
			}
		}
	}

	QStringList dirsForTheme;
	for (const QByteArray& styleName: ::qAsConst(styles)){
		for(const QString& resType: ::qAsConst(typesOfResources)){
			 dirsForTheme << resourceDirPath + dirSeparator + resType + dirSeparator + styleName;
		}
	}

	if(!CreateQrcForDirs(dirsForTheme, QByteArray(outputDirPath) + dirSeparator + projectName + "Theme" + ".qrc")){
		return -1;
	}

	return 0;
}


QByteArray CDesignTokenWebQrcGeneratorComp::GetHelpString() const
{
	return "No help Yet :(";
}


bool CDesignTokenWebQrcGeneratorComp::CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	QFile outputFile(outputFileName);
	outputFile.open(QFile::WriteOnly);
	QXmlStreamWriter xmlWriter(&outputFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file: ::qAsConst(files)){
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", file.baseName());
		xmlWriter.writeCharacters(path + '/' + file.fileName());
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();
	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenWebQrcGeneratorComp::CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const
{
	QFile outputFile(outputFileName);
	outputFile.open(QFile::WriteOnly);
	QXmlStreamWriter xmlWriter(&outputFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");

	QDir::Filters filters = QDir::Filter::Dirs | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList dirs = QDir(inputDirName).entryInfoList(filters, sort);

	for (const QFileInfo& dir: ::qAsConst(dirs)){
		CreateSingleEntry(QString('/') + dir.baseName(), dir.absolutePath(), GetPathOverFile(dir.absolutePath(), outputFileName), xmlWriter);
	}

	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenWebQrcGeneratorComp::CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const
{
	QFile outputFile(outputFileName);
	bool openResourceFile = outputFile.open(QFile::WriteOnly);
	if(!openResourceFile){
		qCritical() << "Cannot open resource file" << outputFileName;
	}
	QXmlStreamWriter xmlWriter(&outputFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();

	xmlWriter.writeStartElement("RCC");

	for (const QString& dir : ::qAsConst(inputDirNames)){
		QString prefix = "/";
		QStringList dirParts = dir.split(QDir::separator());
		if (dirParts.size() > 2){
			prefix += dirParts[dirParts.size() - 1];
			prefix += "/";
			prefix += dirParts[dirParts.size() - 2];
		}
		CreateSingleEntry(prefix, dir, GetPathOverFile(dir, outputFileName), xmlWriter);
	}

	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenWebQrcGeneratorComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return CreateQrc(prefix, path, files, outputFileName);
	}

	return false;
}


bool CDesignTokenWebQrcGeneratorComp::CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	xmlWriter.writeStartElement("qresource");

	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file : ::qAsConst(files)){
		if(file.isFile()){
			xmlWriter.writeStartElement("file");
			xmlWriter.writeAttribute("alias", file.fileName());
			xmlWriter.writeCharacters(path + '/' + file.fileName());
			xmlWriter.writeEndElement();
		}
	}

	xmlWriter.writeEndElement();
	return true;
}


bool CDesignTokenWebQrcGeneratorComp::CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Dirs | QDir::NoDotAndDotDot | QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	for (const QFileInfo& file : ::qAsConst(files)){
		if(file.isFile()){
			xmlWriter.writeStartElement("file");
			xmlWriter.writeAttribute("alias", file.baseName());
			xmlWriter.writeCharacters(path + '/' + file.fileName());
			xmlWriter.writeEndElement();
		}
		else{
			CreateSingleEntryInCurrentPrefix(file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
	}
	return true;
}

QString CDesignTokenWebQrcGeneratorComp::GetPathOverFile(const QString& dir, const QString& file) const
{
	QString retval = QDir(dir).absolutePath();
	QFileInfo inputFileInfo(file);
	QString inputFileInfoPath = inputFileInfo.absolutePath();
	retval.replace(inputFileInfoPath, "");

	while(retval.startsWith(QDir::separator())){
		retval.remove(0,1);
	}

	while(retval.startsWith('/')){
		retval.remove(0,1);
	}

	while(retval.startsWith('\\')){
		retval.remove(0,1);
	}

	return retval;
}


} // namespace imtdesign


