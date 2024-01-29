#include "CSdlClassTreeModelModificatorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

//Acf includes
#include <istd/CSystem.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


int CSdlClassTreeModelModificatorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());

	int retVal = iproc::IProcessor::TS_OK;
	if (!m_argumentParserCompPtr->IsModificatorEnabled(*m_processorModificatorName)){
		// nothing todo if our modificator is not exsits
		return retVal;
	}

	if (!m_argumentParserCompPtr->IsModificatorEnabled(*m_processorModificatorName)){
		SendErrorMessage(1, QString("Variant map modificator is not enabled. Use -M%1 argument to enable it").arg(s_variantMapModificatorArgumentName));

		return iproc::IProcessor::TS_INVALID;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	imtbase::CTempDir tempDir;
	const QString tempPath = tempDir.Path();

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
	for (const CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".cpp"));
		m_originalHeaderFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_originalSourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		bool isOk = BeginClassFiles(sdlType);
		if (!isOk){
			AbortCurrentProcessing();

			return iproc::IProcessor::TS_INVALID;
		}

		isOk = isOk && EndClassFiles(sdlType);
		if (!isOk){
			AbortCurrentProcessing();

			return iproc::IProcessor::TS_INVALID;
		}
	}

	return retVal;
}


// private methods



bool CSdlClassTreeModelModificatorComp::BeginClassFiles(const CSdlType& sdlType)
{
	// Create new files in temp path
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	// Open original files to read data
	if (!m_originalHeaderFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalHeaderFilePtr->fileName(), m_originalHeaderFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_originalSourceFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalSourceFilePtr->fileName(), m_originalSourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}


	bool retVal = true;
	retVal = retVal && BeginHeaderClassFile(sdlType);
	retVal = retVal && BeginSourceClassFile(sdlType);

	return retVal;
}


bool CSdlClassTreeModelModificatorComp::BeginHeaderClassFile(const CSdlType& sdlType)
{
	QTextStream ofStream(m_headerFilePtr.GetPtr());
	QTextStream ifStream(m_originalHeaderFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		const QString readLine = ifStream.readLine();

		// check if we reached end of include declaration (namespace begin)
		static QRegularExpression namespaceRegExp(QStringLiteral("\\s*namespace"));
		if (namespaceRegExp.match(readLine).hasMatch()){
			ofStream.seek(ofStream.pos() - 1); // remove extra new line
			ofStream << QStringLiteral("// imtbase includes");
			FeedStream(ofStream, 1, false);
			ofStream << QStringLiteral("#include <imtbase/CTreeItemModel.h>");
			FeedStream(ofStream, 3, false);
		}
		ofStream << readLine;
		FeedStream(ofStream);
	}

	// add method definitions
	ofStream << QStringLiteral("\tbool AddMeToModel(imtbase::CTreeItemModel& model, int modelIndex, const QList<QString>& requiredFields = QList<QString>()) const;");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\tstatic bool ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtbase::CTreeItemModel& model, int modelIndex);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassTreeModelModificatorComp::BeginSourceClassFile(const CSdlType& sdlType)
{
	QTextStream ofStream(m_sourceFilePtr.GetPtr());
	QTextStream ifStream(m_originalSourceFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		ofStream << ifStream.readLine();
		FeedStream(ofStream);
	}


	// add method implementations
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::AddMeToModel(imtbase::CTreeItemModel& model, int modelIndex, const QList<QString>& requiredFields) const\n{\n");

	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToModelCode(ofStream, field);
	}
	ofStream << QStringLiteral("\n\treturn true;\n}");

	FeedStream(ofStream, 3, false);

	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtbase::CTreeItemModel& model, int modelIndex)\n{\n");
	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromModelCode(ofStream, field);
	}
	ofStream << QStringLiteral("\n\treturn true;\n}");

	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassTreeModelModificatorComp::EndClassFiles(const CSdlType& sdlType)
{
	return CloseFiles();
}


bool CSdlClassTreeModelModificatorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	Q_ASSERT(retVal);

	retVal = m_sourceFilePtr->flush() && retVal;
	Q_ASSERT(retVal);

	// Close files
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	m_originalHeaderFilePtr->close();
	m_originalSourceFilePtr->close();

	// Then move modificated files to original location
	const QString originalPath = QFileInfo(*m_originalHeaderFilePtr).absolutePath();
	retVal = retVal && istd::CSystem::FileMove(m_headerFilePtr->fileName(), originalPath, true);
	Q_ASSERT(retVal);

	retVal = retVal && istd::CSystem::FileMove(m_sourceFilePtr->fileName(), originalPath, true);
	Q_ASSERT(retVal);

	return retVal;
}



void CSdlClassTreeModelModificatorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


void CSdlClassTreeModelModificatorComp::AddFieldWriteToModelCode(QTextStream& stream, const CSdlField& field)
{

}

void CSdlClassTreeModelModificatorComp::AddFieldReadFromModelCode(QTextStream& stream, const CSdlField& field)
{
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = model.GetData(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	if (field.IsRequired()){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral(".isNull(){");
		FeedStream(stream, 1, false);

		/// \todo add errorString here
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
		stream << GetFromVariantConversionString(field) << QStringLiteral(");");
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral(".isNull(){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
		stream << GetFromVariantConversionString(field) << QStringLiteral(");");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}

}



} // namespace imtsdl

