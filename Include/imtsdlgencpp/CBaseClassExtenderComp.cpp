// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CBaseClassExtenderComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IOptionsList.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
{

static QRegularExpression s_includeRemarkRegExp(QStringLiteral("\\/\\/(.*)includes"));
static QRegularExpression s_includeDirectiveRegExp(QStringLiteral("\\#\\s*include\\s*[<\"]{1}(.*)[>\"]{1}"));
static QRegularExpression s_publicSectionDirectiveRegExp(QStringLiteral("\\s*public\\s*\\:"));

// static help methods

QSet<QString> GetRemarks(const QMap<QString, QString>& direciveIncludeMap)
{
	QSet<QString> retVal;
	QMapIterator mapIterator(direciveIncludeMap);
	while(mapIterator.hasNext()){
		retVal.insert(mapIterator.next().value());
	}

	return retVal;
}


bool FindRemark(const QMap<QString, QString>& container, const QString& remark)
{
	QString remarkName = remark;
	QRegularExpressionMatch remarkMatch = s_includeRemarkRegExp.match(remark);
	if (remarkMatch.hasMatch()){
		remarkName = remarkMatch.capturedTexts().constLast();
	}

	for (auto iter = container.cbegin(); iter != container.cend(); ++iter){
		QRegularExpressionMatch match = s_includeRemarkRegExp.match(iter.value());
		if (match.hasMatch()){
			const QString containerRemarkName = match.capturedTexts().constLast();
			return remarkName == containerRemarkName;
		}
	}

	return false;
}


bool FindDirective(const QMap<QString, QString>& container, const QString& directive)
{
	QString directiveName = directive;
	QRegularExpressionMatch remarkMatch = s_includeDirectiveRegExp.match(directive);
	if (remarkMatch.hasMatch()){
		directiveName = remarkMatch.capturedTexts().constLast();
	}

	for (auto iter = container.cbegin(); iter != container.cend(); ++iter){
		QRegularExpressionMatch match = s_includeRemarkRegExp.match(iter.key());
		if (match.hasMatch()){
			const QString containerRemarkName = match.capturedTexts().constLast();
			return directiveName == containerRemarkName;
		}
	}

	return false;
}


QStringList GetDirectivesByRemark(const QMap<QString, QString>& direciveIncludeMap, const QString& remark)
{
	QString remarkName = remark;
	QRegularExpressionMatch remarkMatch = s_includeRemarkRegExp.match(remark);
	if (remarkMatch.hasMatch()){
		remarkName = remarkMatch.capturedTexts().constLast();
	}

	QStringList retVal;
	QMapIterator mapIterator(direciveIncludeMap);
	while(mapIterator.hasNext()){
		auto mapIndex = mapIterator.next();
		const QString& includeDirective = mapIndex.value();
		QRegularExpressionMatch match = s_includeRemarkRegExp.match(includeDirective);
		if(match.hasMatch()){
			const QString containerRemarkName = match.capturedTexts().constLast();
			if (remarkName == containerRemarkName){
				retVal << mapIndex.key();
			}
		}
	}

	return retVal;
}


void RemoveDirectivesByRemark(QMap<QString, QString>& direciveIncludeMap, const QString& remark)
{
	QString remarkName = remark;
	QRegularExpressionMatch remarkMatch = s_includeRemarkRegExp.match(remark);
	if (remarkMatch.hasMatch()){
		remarkName = remarkMatch.capturedTexts().constLast();
	}

	QMutableMapIterator mapIterator(direciveIncludeMap);
	while(mapIterator.hasNext()){
		QString includeDirective = mapIterator.next().value();
		QRegularExpressionMatch match = s_includeRemarkRegExp.match(includeDirective);
		if(match.hasMatch()){
			const QString containerRemarkName = match.capturedTexts().constLast();
			if (remarkName == containerRemarkName){
				mapIterator.remove();
			}
		}
	}
}


// public methods

//reimplemented(iproc::IProcessor)

iproc::IProcessor::TaskState CBaseClassExtenderComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	if (paramsPtr == nullptr){
		I_CRITICAL();

		return TS_INVALID;
	}

	const iprm::IOptionsList* baseClassOptionListPtr = dynamic_cast<const iprm::IOptionsList*>(inputPtr);
	if (baseClassOptionListPtr == nullptr){
		I_CRITICAL();

		return TS_INVALID;
	}

	imtbase::CTempDir tempDir;
	const QString tempPath = tempDir.GetPath();

	iprm::TParamsPtr<ifile::IFileNameParam> headerFilePathPtr(paramsPtr, QByteArrayLiteral("HeaderFile"));
	if (!headerFilePathPtr.IsValid()){
		I_CRITICAL();

		return TS_INVALID;
	}

	m_originalHeaderFilePtr.SetPtr(new QFile(headerFilePathPtr->GetPath()));
	m_headerFilePtr.SetPtr(new QFile(tempPath + QDir::separator() + QFileInfo(*m_originalHeaderFilePtr).fileName()));

	bool isOk = BeginClassFiles();
	if (!isOk){
		AbortCurrentProcessing();

		return TS_INVALID;
	}

	isOk = isOk && ProcessHeaderClassFile(*paramsPtr, *baseClassOptionListPtr);
	if (!isOk){
		AbortCurrentProcessing();

		return TS_INVALID;
	}

	isOk = isOk && EndClassFiles();
	if (!isOk){
		AbortCurrentProcessing();

		return TS_INVALID;
	}

	return TS_OK;
}


// protected methods


bool CBaseClassExtenderComp::ProcessHeaderClassFile(
			const iprm::IParamsSet& paramsPtr,
			const iprm::IOptionsList& baseClassList)
{
	QTextStream ofStream(m_headerFilePtr.GetPtr());
	QTextStream ifStream(m_originalHeaderFilePtr.GetPtr());

	iprm::TParamsPtr<iprm::IEnableableParam> addMacroParamPtr(&paramsPtr, QByteArrayLiteral("AddBaseComponentMacro"), false);
	const bool addCompMacro = addMacroParamPtr.IsValid() && addMacroParamPtr->IsEnabled();


	// fill include directives /remarks
	QMap<QString/*directive*/, QString/*remark*/> necessaryDireciveIncludeMap;
	QStringList classList;
	int baseListCount = baseClassList.GetOptionsCount();
	for (int i = 0; i < baseListCount; ++i){
		const QString includeDirective = QString("#include <%1>").arg(baseClassList.GetOptionName(i));
		QString classLib = QString(baseClassList.GetOptionId(i).split(':').constFirst());
		classLib = (QString("// %1 includes").arg(classLib));
		necessaryDireciveIncludeMap.insert(includeDirective, classLib);
		classList << QString(baseClassList.GetOptionId(i));
	}

	/// indicates if the class has a public section
	bool hasPublicSection = false;

	if (!necessaryDireciveIncludeMap.isEmpty()){
		while (!ifStream.atEnd()){
			const QString readLine = ifStream.readLine();
			if (!hasPublicSection && s_publicSectionDirectiveRegExp.match(readLine).hasMatch()){
				hasPublicSection = true;
			}

			// check if any of included directives already added
			if (s_includeDirectiveRegExp.match(readLine).hasMatch()){
				QMutableMapIterator mapIterator(necessaryDireciveIncludeMap);
				while (mapIterator.hasNext()){
					auto indexIter = mapIterator.next();
					QRegularExpressionMatch match = s_includeDirectiveRegExp.match(indexIter.key());
					if (match.hasMatch()){
						if (FindDirective(necessaryDireciveIncludeMap, readLine)){
							mapIterator.remove();
						}
					}
				}
			}
		}
	}

	// read file from start
	ifStream.seek(0);

	/// indicates if we inside class
	bool inClass = false;

	/// indicates if we inside public section of class
	bool inPublicSection = false;

	/// indicates if we added Component MACRO
	bool componentMacroAdded = false;

	QString className;

	while (!ifStream.atEnd()){
		const QString readLine = ifStream.readLine();
		QRegularExpressionMatch includeRemarkMatch = s_includeRemarkRegExp.match(readLine);
		bool addDirective = false;
		// if include remark, check if we have additionad directives for this remark
		if (includeRemarkMatch.hasMatch()){
			addDirective = FindRemark(necessaryDireciveIncludeMap, readLine);
		}

		if (addDirective){
			FeedStream(ofStream, 1, false);
			const QStringList directivesForRemark = GetDirectivesByRemark(necessaryDireciveIncludeMap, QString(readLine));
			for (const QString& directive: directivesForRemark){
				ofStream << directive;
				FeedStream(ofStream, 1, false);
			}
			RemoveDirectivesByRemark(necessaryDireciveIncludeMap, QString(readLine));
		}
		else if (!necessaryDireciveIncludeMap.isEmpty()){
			// add remaining includes
			static QRegularExpression namespaceRegExp(QStringLiteral("^\\s*namespace"));
			if (namespaceRegExp.match(readLine).hasMatch()){
				// remove extra line
				ofStream.seek(ofStream.pos() - 1);

				QSet<QString> remarks = GetRemarks(necessaryDireciveIncludeMap);
				for (const QString& remark: remarks){
					ofStream << remark;
					FeedStream(ofStream, 1, false);

					const QStringList directivesForRemark = GetDirectivesByRemark(necessaryDireciveIncludeMap, remark);
					for (const QString& directive: directivesForRemark){
						ofStream << directive;
						FeedStream(ofStream, 1, false);
					}
					FeedStream(ofStream, 1, false);
				}

				FeedStream(ofStream, 2, false);
			}
		}

		bool writeLine = true;
		if (!classList.isEmpty()){
			// add inheritance if we found a class
			static QRegularExpression classRegExp(QStringLiteral("^\\s*class\\s*([^\\s\\:]+)"));
			QRegularExpressionMatch classRegExpMatch = classRegExp.match(readLine);
			if (classRegExpMatch.hasMatch()){
				writeLine = false;
				ofStream << readLine;
				ofStream.flush();
				AddInheritance(ifStream, ofStream, classList);
				inClass = true;
				className = classRegExpMatch.capturedTexts().constLast();
			}
		}

		// if we found public section
		if (inClass && s_publicSectionDirectiveRegExp.match(readLine).hasMatch()){
			inPublicSection = true;
		}

		if (inPublicSection && inClass && addCompMacro && !componentMacroAdded){
			writeLine = false;
			ofStream << readLine;
			FeedStream(ofStream, 1, false);

			if (!hasPublicSection){
				ofStream << QStringLiteral("public:");
				FeedStream(ofStream, 1, false);
			}
			Q_ASSERT(!className.isEmpty());
			Q_ASSERT(baseListCount > 0);

			// define BaseClass
			FeedStreamHorizontally(ofStream, 1);
			ofStream << QStringLiteral("typedef ");
			ofStream << QString(baseClassList.GetOptionId(0));
			ofStream << QStringLiteral(" BaseClass;");

			// add Comp MACRO
			FeedStream(ofStream, 2, false);
			FeedStreamHorizontally(ofStream, 1);
			ofStream << QStringLiteral("I_BEGIN_BASE_COMPONENT(");
			ofStream << className;
			ofStream << ')';
			FeedStream(ofStream, 1, false);

			FeedStreamHorizontally(ofStream, 1);
			ofStream << QStringLiteral("I_END_COMPONENT");
			FeedStream(ofStream, 2, true);

			componentMacroAdded = true;
		}
		if (writeLine){
			ofStream << readLine;
			FeedStream(ofStream);
		}
	}

	return true;
}


bool CBaseClassExtenderComp::BeginClassFiles()
{
	// Create new files in temp path
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	// Open original files to read data
	if (!m_originalHeaderFilePtr->open(QIODevice::ReadOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalHeaderFilePtr->fileName(), m_originalHeaderFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CBaseClassExtenderComp::EndClassFiles()
{
	return CloseFiles();
}


bool CBaseClassExtenderComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();

	m_headerFilePtr->close();

	m_originalHeaderFilePtr->close();

	const QString originalHeaderFilePath = QFileInfo(*m_originalHeaderFilePtr).absolutePath();
	const QString sourceFile = m_headerFilePtr->fileName();
	bool isMoved = istd::CSystem::FileMove(sourceFile, originalHeaderFilePath, true);
	if (!isMoved){
		SendErrorMessage(0, QString("Unable to move file '%1' to '%2'").arg(sourceFile, originalHeaderFilePath));
	}

	retVal = retVal && isMoved;

	return retVal;
}


void CBaseClassExtenderComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
}


void CBaseClassExtenderComp::AddInheritance(QTextStream& ifStream, QTextStream& ofStream, const QStringList& classNameList)
{
	QStringList buffer;

	// find end of class declaration and potential inheritances
	bool addColon = true;
	while(!ifStream.atEnd()){
		QString bufferLine = ifStream.readLine();

		// we found existing inheritance(s) no colon needed
		if (bufferLine.contains(':')){
			addColon = false;
		}

		// we found end of class declaration. Add requered inheritance(s)
		if (bufferLine.startsWith('{')){
			WriteBufferToStream(ofStream, buffer);

			if (addColon){
				ofStream << ':';
			}
			else {
				ofStream << ',';
			}
			FeedStream(ofStream, 1, false);
			qsizetype classNameSize = classNameList.size();
			for (int i = 0; i < classNameSize; ++i){
				const QString& className = classNameList[i];
				FeedStreamHorizontally(ofStream, 3);
				ofStream << QStringLiteral("virtual public ");
				ofStream << className;

				if (i < classNameSize - 1){
					ofStream << ',';
				}
				FeedStream(ofStream, 1, false);
			}

			buffer << bufferLine;
			break;
		}
	}

	WriteBufferToStream(ofStream, buffer);
}

void CBaseClassExtenderComp::WriteBufferToStream(QTextStream& stream, QStringList& buffer)
{
	while (!buffer.isEmpty()){
		const QString& bufferLine = buffer.takeFirst();
		stream << bufferLine;
		FeedStream(stream, 1, true);
	}
}


} // namespace imtsdlgencpp

