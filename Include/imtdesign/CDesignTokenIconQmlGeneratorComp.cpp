#include <imtdesign/CDesignTokenIconQmlGeneratorComp.h>


// Qt includes
#include <QtXml/QtXml>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtdesign
{



int CDesignTokenIconQmlGeneratorComp::Exec()
{

	if(!m_argumentParserAttrPtr->IsQmlGenerateRequired()){
		return 0;
	}

	QStringList typesOfActive = {"On", "Off"};
	QStringList typesOfState = {"Active", "Disabled", "Normal", "Selected"};
	QStringList typesOfStyle = {"Light", "Dark"};
	QStringList properties;
	QByteArray dirSeparator = QByteArray(1,QDir::separator().toLatin1());;
	QDir::Filters filters = QDir::Filter::Files;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QByteArray outputFilePath = m_argumentParserAttrPtr->GetOutputFilePath();
	QByteArrayList argumentsDirs = m_argumentParserAttrPtr->GetImagesInputDirectoryMultiPath();
	QFile outputFile(outputFilePath);
	if (!outputFile.open(QFile::WriteOnly))
	{
		return 0;
	}
	QTextStream out(&outputFile);
	out.setCodec("UTF-8");
	out << "pragma Singleton" << endl;
	out << "import QtQuick 2.0" << endl;
	out << "Item {" << endl;


	for (const QByteArray& dir: ::qAsConst(argumentsDirs)){
		QFileInfoList filesDir = QDir(dir).entryInfoList(filters, sort);
		for (const QFileInfo& fileInfo: ::qAsConst(filesDir)){
			if (!fileInfo.baseName().isEmpty() && !properties.contains(fileInfo.baseName()))
			{
				for (const QString& typesActive: ::qAsConst(typesOfActive)){
					for (const QString& typesState: ::qAsConst(typesOfState)){
						QString propertyName = "icon_" + fileInfo.baseName() + '_' + typesActive + '_' + typesState;
						out << "property string " + propertyName + ";" << endl;
					}
				}
				properties.append(fileInfo.baseName());
			}
		}
	}
	out << "property var iconNames: [";

	for (int i = 0; i < properties.length(); i++){
		if (i > 0){
			out << ',';
		}
		out << "\""  << properties[i] << "\"";
	}
	out << "]" << endl;
	out << "}" << endl;
	outputFile.close();
	return 0;
}


QByteArray CDesignTokenIconQmlGeneratorComp::GetHelpString() const
{
	return "No help Yet :(";
}


bool CDesignTokenIconQmlGeneratorComp::CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
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


bool CDesignTokenIconQmlGeneratorComp::CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const
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


bool CDesignTokenIconQmlGeneratorComp::CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const
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
			prefix += dirParts[dirParts.size() - 2] + '/' + dirParts[dirParts.size() - 1];
		}
		CreateSingleEntry(prefix, dir, GetPathOverFile(dir, outputFileName), xmlWriter);
	}

	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();

	return true;
}


bool CDesignTokenIconQmlGeneratorComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return CreateQrc(prefix, path, files, outputFileName);
	}

	return false;
}


bool CDesignTokenIconQmlGeneratorComp::CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	xmlWriter.writeStartElement("qresource");

	xmlWriter.writeAttribute("prefix", prefix);

	for (const QFileInfo& file : ::qAsConst(files)){
		if(file.isFile()){
			xmlWriter.writeStartElement("file");
//			xmlWriter.writeAttribute("alias", file.baseName() + '.' + file.suffix());//FIX
			xmlWriter.writeAttribute("alias", file.fileName());
			xmlWriter.writeCharacters(path + '/' + file.fileName());
			xmlWriter.writeEndElement();
		}
	}

	xmlWriter.writeEndElement();

	filters = QDir::Filter::Dirs | QDir::NoDotAndDotDot;
	files = QDir(inputDirPath).entryInfoList(filters, sort);
	for (const QFileInfo& file : ::qAsConst(files)){
		if(prefix == "/Icons" && file.baseName() == "Color"){
			CreateSingleEntry("/ColorIcons",file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
		else{
			CreateSingleEntryInCurrentPrefix(file.absoluteFilePath(), path + QDir::separator().toLatin1() + file.fileName(),xmlWriter);
		}
	}

	return true;
}


bool CDesignTokenIconQmlGeneratorComp::CreateSingleEntryInCurrentPrefix(const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
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

QString CDesignTokenIconQmlGeneratorComp::GetPathOverFile(const QString& dir, const QString& file) const
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


