#include <imtstyle/CDesignTokenQrcUpdaterComp.h>

// Qt includes
#include <QtXml/QtXml>

// Acf includes

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtstyle
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
		this->CreateQrcForDirs(dirsForTheme, QByteArray(outputDirName) + dirSeparator + projectName + styleName.toLower() + ".qrc");
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

	xmlWriter.writeStartDocument();{

		xmlWriter.writeStartElement("RCC");{

			xmlWriter.writeStartElement("qresource");{

				xmlWriter.writeAttribute("prefix", prefix);

				for (const QFileInfo& file: ::qAsConst(files)){

					xmlWriter.writeStartElement("file");{
						xmlWriter.writeAttribute("alias", file.baseName());
						xmlWriter.writeCharacters(path + '/' + file.fileName());
					}
					xmlWriter.writeEndElement();
				}
			}
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}
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

	xmlWriter.writeStartDocument();{

		xmlWriter.writeStartElement("RCC");{

			QDir::Filters filters = QDir::Filter::Dirs | QDir::Filter::Readable;
			QDir::SortFlags sort = QDir::SortFlag::Name;
			QFileInfoList dirs = QDir(inputDirName).entryInfoList(filters, sort);

			for (const QFileInfo& dir: ::qAsConst(dirs)){
				this->CreateSingleEntry(QString('/') + dir.baseName(), dir.absolutePath(), this->GetPathOverFile(dir.absolutePath(), outputFileName), xmlWriter);
			}
		}
	}
	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();
	return true;
}


bool CDesignTokenQrcUpdaterComp::CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const
{
	QFile outputFile(outputFileName);
	outputFile.open(QFile::WriteOnly);
	QXmlStreamWriter xmlWriter(&outputFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();{

		xmlWriter.writeStartElement("RCC");{

			for(const QString& dir: ::qAsConst(inputDirNames)){
				QString prefix = "/";
				QStringList dirParts = dir.split(QDir::separator());
				if(dirParts.size() > 2){
					prefix += dirParts[dirParts.size()-2];
				}
				this->CreateSingleEntry(prefix , dir, this->GetPathOverFile(dir, outputFileName), xmlWriter);

			}

		}
	}
	xmlWriter.writeEndDocument();

	outputFile.flush();
	outputFile.close();
	return true;
}

bool CDesignTokenQrcUpdaterComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return this->CreateQrc(prefix, path, files, outputFileName);
	}
	return true;
}


bool CDesignTokenQrcUpdaterComp::CreateSingleEntry(const QString& prefix, const QString& inputDirPath, const QString& path, QXmlStreamWriter& xmlWriter) const
{
	QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Readable;
	QDir::SortFlags sort = QDir::SortFlag::Name;
	QFileInfoList files = QDir(inputDirPath).entryInfoList(filters, sort);

	xmlWriter.writeStartElement("qresource");{

		xmlWriter.writeAttribute("prefix", prefix);

		for (const QFileInfo& file: ::qAsConst(files)){

			xmlWriter.writeStartElement("file");{
				xmlWriter.writeAttribute("alias", file.baseName());
				xmlWriter.writeCharacters(path + '/' + file.fileName());
			}
			xmlWriter.writeEndElement();
		}
	}
	xmlWriter.writeEndElement();
	return true;
}

QString CDesignTokenQrcUpdaterComp::GetPathOverFile(const QString& dir, const QString& file) const
{
	QString retval = QDir(dir).absolutePath();
	QFileInfo inputFileInfo(file);
	QString inputFileInfoPath = inputFileInfo.absolutePath();
	retval.replace(inputFileInfoPath, "");

	if(retval.startsWith(QDir::separator())){
		retval.remove(0,1);
	}
	return retval;
}


} // namespace imtstyle


//-F /Volumes/ALL_DATA/ZProjects/Acf/ITDevelopment/ImtCore/Include/imtstyle/light.theme -I /Volumes/ALL_DATA/ZProjects/Acf/ITDevelopment/ImtCore/Include/imtgui/Resources/Icons/Template -S /Volumes/ALL_DATA/ZProjects/Acf/ITDevelopment/ImtCore/Include/imtgui/Resources/Styles/Template -OD /Volumes/ALL_DATA/ZProjects/Acf/ITDevelopment/ImtCore/AuxInclude/Qt5_ClangOSX_64/GeneratedFiles/imtgui -P imtgui
