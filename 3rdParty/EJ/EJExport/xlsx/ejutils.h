#ifndef EJUTILS_H
#define EJUTILS_H
#include "ejtableblocks.h"
#include "documents/ejsheetdocument.h"
#include "documents/ejsharedstringsdocument.h"
#include "documents/ejstylesdocument.h"

class EjUtils
{
public:
	int static findMaxIndexColumnInDocumentByBlocks(QList<EjBlock*>);
	bool validateForTextFromTable(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
								  EjStylesDocument *styles_document);
    /*Проверка, остался ли недобавленный текст,
      он будет недобавленным в том случае, если текст был в конце документа и не было
      переноса строки, то есть блока 1, в моей логики текст вне таблицы добавляется в файл,
       только когда после него встречается блок 1*/
	void checkUnaddedText(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						  EjStylesDocument *styles_document);
	EjUtils();
};

#endif // EJUTILS_H
