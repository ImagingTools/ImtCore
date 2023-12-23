#include <imtdesign/CDesignTokenCommonQrcGeneratorComp.h>


// Qt includes
#include <QtXml/QtXml>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdesign
{



int CDesignTokenCommonQrcGeneratorComp::Exec()
{
	if(!m_argumentParserAttrPtr->IsCommonGenerateResourceRequired()){
		return 0;
	}

	QStringList typesOfResources = {"Icons", "Styles"};

	QByteArray dirSeparator = QByteArray(1,QDir::separator().toLatin1());

	QByteArray outputDirPath = m_argumentParserAttrPtr->GetOutputDirectoryPath();
	QByteArray projectName = m_argumentParserAttrPtr->GetProjectName();
	QByteArray resourceDirPath = outputDirPath + dirSeparator + "Resources";
	QVector<QByteArray> styles;

	QDir::Filters filters = QDir::Filter::Dirs;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList dirs = QDir(resourceDirPath).entryInfoList(filters, sort);

	for (const QFileInfo& fileInFo: ::std::as_const(dirs)){
		if (typesOfResources.contains(fileInFo.fileName())){
			QFileInfoList iconsDirs = QDir(fileInFo.absoluteFilePath()).entryInfoList(filters, sort);
			for (const QFileInfo& iconsFileInFo: ::std::as_const(iconsDirs)){
				if (!iconsFileInFo.baseName().isEmpty()){
					styles << iconsFileInFo.fileName().toUtf8();
				}
			}
		}
	}

	QStringList dirsForTheme;
	for (const QByteArray& styleName: ::std::as_const(styles)){
		for(const QString& resType: ::std::as_const(typesOfResources)){
			 dirsForTheme << resourceDirPath + dirSeparator + resType + dirSeparator + styleName;
		}
	}

	if(!CreateQrcForDirs(dirsForTheme, QByteArray(outputDirPath) + dirSeparator + projectName + "Common" + ".qrc")){
		return -1;
	}

	return 0;
}


QByteArray CDesignTokenCommonQrcGeneratorComp::GetHelpString() const
{
	return "No help Yet :(";
}


bool CDesignTokenCommonQrcGeneratorComp::CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	QFile outputFile(outputFileName);
	outputFile.open(QFile::WriteOnly);
	QXmlStreamWriter xmlWriter(&outputFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file: ::std::as_const(files)){
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


bool CDesignTokenCommonQrcGeneratorComp::CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const
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

	for (const QFileInfo& dir: ::std::as_const(dirs)){
		CreateSingleEntry(QString('/') + dir.baseName(), dir.absolutePath(), GetPathOverFile(dir.absolutePath(), outputFileName), xmlWriter);
	}

	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenCommonQrcGeneratorComp::CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const
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

	for (const QString& dir : ::std::as_const(inputDirNames)){
		QString prefix = "/";
		QStringList dirParts = dir.split(QDir::separator());
		if (dirParts.size() > 2){
			prefix += dirParts[dirParts.size() - 2];
			prefix += "/";
			prefix += dirParts[dirParts.size() - 1];
		}
		CreateSingleEntry(prefix, dir, GetPathOverFile(dir, outputFileName), xmlWriter);
	}

	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenCommonQrcGeneratorComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return CreateQrc(prefix, path, files, outputFileName);
	}

	return false;
}


bool CDesignTokenCommonQrcGeneratorComp::CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	xmlWriter.writeStartElement("qresource");

	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file : ::std::as_const(files)){
		if(file.isFile()){
			xmlWriter.writeStartElement("file");
			xmlWriter.writeAttribute("alias", file.baseName());
			xmlWriter.writeCharacters(path + '/' + file.fileName());
			xmlWriter.writeEndElement();
		}
	}

	xmlWriter.writeEndElement();

	filters = QDir::Filter::Dirs | QDir::NoDotAndDotDot;
	files = QDir(inputDirPath).entryInfoList(filters, sort);
	for (const QFileInfo& file : ::std::as_const(files)){
		if(prefix.contains("/Icons") && file.baseName() == "Color"){
			QString theme = prefix.right(prefix.length() - 6);
			CreateSingleEntry("/ColorIcons" + theme,file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
		else{
			CreateSingleEntryInCurrentPrefix(file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
	}

	return true;
}


bool CDesignTokenCommonQrcGeneratorComp::CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Dirs | QDir::NoDotAndDotDot | QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	for (const QFileInfo& file : ::std::as_const(files)){
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

QString CDesignTokenCommonQrcGeneratorComp::GetPathOverFile(const QString& dir, const QString& file) const
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


