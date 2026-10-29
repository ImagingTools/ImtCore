// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCxxSourceTailPostProcessorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>


namespace imtsdlgencpp
{


// reimplemented (ICxxFilePostProcessor)

bool CCxxSourceTailPostProcessorComp::ProcessFiles(const CxxEntryFileInfo& files, const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());

	if (!files.sourceFile){
		return true;
	}

	QTextStream stream(files.sourceFile.get());

	// the objects are optional (see '--no-objects'), the schema without them has only the main source file
	if (files.objectSourceFile){
		if (!files.objectSourceFile->seek(0)){
			SendErrorMessage(0, QStringLiteral("Unable to read objects source file '%1'").arg(files.objectSourceFile->fileName()));

			return false;
		}

		FeedStream(stream, 1, true);
		if (CopyData(*files.objectSourceFile, *files.sourceFile) < 0){
			SendErrorMessage(0, QStringLiteral("Unable to append the objects source file '%1' to '%2'").arg(files.objectSourceFile->fileName(), files.sourceFile->fileName()));

			return false;
		}
	}

	if (m_argumentParserCompPtr->IsMocIncludeEnabled()){
		// the moc output of the main header (namespace and enums meta objects) is always required
		const QFileInfo sourceFileInfo(*files.sourceFile);
		FeedStream(stream, 2, false);
		stream << QStringLiteral("#include \"moc_") << sourceFileInfo.fileName() << '"';
		FeedStream(stream, 1, false);

		if (files.objectSourceFile){
			const QFileInfo objectSourceFileInfo(*files.objectSourceFile);
			if (sourceFileInfo.dir() != objectSourceFileInfo.dir()){
				SendWarningMessage(0, QStringLiteral("Source file and object source file are in different directories"));
			}

			stream << QStringLiteral("#include \"moc_") << objectSourceFileInfo.fileName() << '"';
			FeedStream(stream, 1, false);
		}
	}

	return true;
}


} // namespace imtsdlgencpp
