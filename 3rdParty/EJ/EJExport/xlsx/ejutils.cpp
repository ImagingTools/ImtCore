#include "ejutils.h"

int EjUtils::findMaxIndexColumnInDocumentByBlocks(QList<EjBlock *> blocks)
{
    /*Функция вернет -1 если в документе нет таблиц*/
    int max_index = -1;
	for(EjBlock* block : blocks){
        if(block->type == e_typeBlocks::EXT_TABLE){
            EjTableBlock* table = (EjTableBlock*)block;
            if(table->nColums() > max_index){
                max_index = table->nColums();
            }
        }
    }
    return max_index;
}

bool EjUtils::validateForTextFromTable(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document, EjStylesDocument *styles_document)
{
    if(sharedStrings_document->first){
        int styleId = styles_document->addXfToCellXfs(sheet_document->getFillId(),
                                                      sheet_document->getBorderId());
        sharedStrings_document->addTextIntoTable(sharedStrings_document->getTempTextFromCell());
        sheet_document->addTextNumberIntoTable(sharedStrings_document->getCountWords() - 1,
                                      styleId);
        sharedStrings_document->first = false;
      }
    return sharedStrings_document->first;
}

void EjUtils::checkUnaddedText(EjSheetDocument *sheet_document, EjSharedStringsDocument *shared_document,
							 EjStylesDocument *styles_document)
{
    if(shared_document->startNewString){
        shared_document->addText("", true);
        int styleId = styles_document->addXfToCellXfs(0, 0);
        sheet_document->addTextNumber(shared_document->getCountWords() - 1, styleId);
        shared_document->startNewString = false;
    }
}

EjUtils::EjUtils()
{

}
