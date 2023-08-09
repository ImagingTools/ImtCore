#include <imtdesign/CDesignTokenQrcUpdaterComp.h>


// Qt includes
#include <QtXml/QtXml>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdesign
{



int CDesignTokenQrcUpdaterComp::Exec()
{
	QStringList typesOfResources = {"Icons", "Styles"};

	QByteArray dirSeparator = QByteArray(1,QDir::separator().toLatin1());

	QByteArray outputDirName = m_argumentParserAttrPtr->GetOutputDirectoryPath();
	QByteArray projectName = m_argumentParserAttrPtr->GetProjectName();
	QByteArray designTokenFilePath = m_argumentParserAttrPtr->GetDesignTokenFilePath();
	m_designTokenFileParserAttrPtr->SetFile(designTokenFilePath);
	m_designTokenFileParserAttrPtr->ParseFile();
	QVector<QByteArray> styles = m_designTokenFileParserAttrPtr->GetDesignSchemaList().GetElementIds();

	for (const QByteArray& styleName: ::qAsConst(styles)){
		QStringList dirsForTheme;
		for(const QString& resType: ::qAsConst(typesOfResources)) {
			dirsForTheme << outputDirName  + dirSeparator + "Resources" + dirSeparator + resType + dirSeparator + styleName;
		}
		if(!CreateQrcForDirs(dirsForTheme, QByteArray(outputDirName) + dirSeparator + projectName + styleName.toLower() + ".qrc")){
			return -1;
		}
	}

	return 0;
}


QByteArray CDesignTokenQrcUpdaterComp::GetHelpString() const
{
	return "No help Yet :(";
}


bool CDesignTokenQrcUpdaterComp::CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
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


bool CDesignTokenQrcUpdaterComp::CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const
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


bool CDesignTokenQrcUpdaterComp::CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const
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


bool CDesignTokenQrcUpdaterComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return CreateQrc(prefix, path, files, outputFileName);
	}

	return false;
}


bool CDesignTokenQrcUpdaterComp::CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	xmlWriter.writeStartElement("qresource");

	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file : ::qAsConst(files)){
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
	for (const QFileInfo& file : ::qAsConst(files)){
		if(prefix.contains("/Icons") && file.baseName() == "Color"){
			QString theme = prefix.right(prefix.count() - 6);
			CreateSingleEntry("/ColorIcons" + theme,file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
		else{
			CreateSingleEntryInCurrentPrefix(file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
	}

	return true;
}


bool CDesignTokenQrcUpdaterComp::CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
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

QString CDesignTokenQrcUpdaterComp::GetPathOverFile(const QString& dir, const QString& file) const
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


