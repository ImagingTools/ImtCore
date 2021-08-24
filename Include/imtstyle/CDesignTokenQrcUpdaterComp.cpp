#include <imtstyle/CDesignTokenQrcUpdaterComp.h>

// Qt includes
#include <QtXml/QtXml>

// Acf includes

//ImtCore includes


namespace imtstyle
{

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

bool CDesignTokenQrcUpdaterComp::ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const
{
	if(!QFile::exists(outputFileName)){
		return this->CreateQrc(prefix, path, files, outputFileName);
	}
	return true;
}



} // namespace imtstyle


