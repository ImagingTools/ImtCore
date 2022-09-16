#include "ejcsvdocumenteditor.h"
#include "ejcsvdocument.h"

EjCSVDocumentWriter::~EjCSVDocumentWriter(){}

int TextWriterCSV::edit(EjCSVDocument& document, QList<EjBlock*> list, int index) {
    //передаем false, текст не будет записан в файл, а будет накапливаться пока не найдем конец строки
    if(document.getActiveTable()){
        return index;
    }
	document.addText(((EjTextBlock*)list.at(index))->text, false);
    return index;
}

int SpaceWriterCSV::edit(EjCSVDocument& document, QList<EjBlock*> list, int index) {
    document.addSpace();
    return index;
}

int EnterWriterCSV::edit(EjCSVDocument& document, QList<EjBlock*> list, int index) {
    //Конец строки найден, добавляем строку в файл
    document.newString();
    return index;
}

int TableWriterCSV::edit(EjCSVDocument& document, QList<EjBlock*> list, int index) {

    //Бывают ситуации когда перед таблицей нет переноса строки
    //условие ниже это исправляет
    if(index != 0 && list.at(index - 1)->type != e_typeBlocks::ENTER){
        document.newString();
    }

    EjTableBlock* block = (EjTableBlock*)list.at(index);
    document.setActiveTable(true);
    document.setCountStrFromCurrentTable(block->nRows());
    document.setCountColFromCurrentTable(block->nColums());
    return index;
}

int CellWriterCSV::edit(EjCSVDocument& document, QList<EjBlock*> list, int index) {
    EjCellBlock* block = (EjCellBlock*)list.at(index);
    document.addTextFromTable(block->getText());
    return index;
}

int MaxIndexColumnCSV::edit(EjCSVDocument &document, QList<EjBlock *> blocks, int index)
{
    document.findMaxIndexColumn(&blocks);
    return index;
}

int EndGroupWriterCSV::edit(EjCSVDocument &document, QList<EjBlock *> blocks, int index)
{
    document.setActiveTable(false);
    return index;
}

int EndEditorCSV::edit(EjCSVDocument &document, QList<EjBlock *> blocks, int index)
{
    if(document.getText() != ""){
        document.addText(document.getText(), true);
    }
    return index;
}
