#include "ejexceldocumenteditors.h"

#include "ejtableblocks.h"
#include "export_global.h"
#include "standart_function.h"

EjExcelDocumentWriter::~EjExcelDocumentWriter() {}
int TextWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						  EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index)
{
	EjTextBlock *text_block =(EjTextBlock*) lBlocks->at(index);

    /*Если этот текст из таблицы, то здесь он добавлен не будет,
       так как текст из таблицы добавляется в CellEditor*/
    if(!sheet_document->getActiveTable()){
        sharedStrings_document->startNewString = true;
         sharedStrings_document->addText(text_block->text, false);
         qDebug() << text_block->text;
    }
    else{
          EjUtils utils;
          utils.validateForTextFromTable(sheet_document, sharedStrings_document, styles_document);
    }

    return index;
}

int TableWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						  EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index)
{
    EjTableBlock *table_block =(EjTableBlock*) lBlocks->at(index);
    sheet_document->setCountColsCurrentTable(table_block->nColums());
    sheet_document->setCountStrCurrentTable(table_block->nRows());
    sheet_document->setActiveTable(true);
    return index;
}

int CellWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						  EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index)
{
    EjCellBlock *cell_block = (EjCellBlock*) lBlocks->at(index);
    EjCellStyle* style = cell_block->cellStyle;

    sharedStrings_document->first = true;

    /*Проверяется индекс столбца*/
    sheet_document->validate();

    int borderId = styles_document->addBorders(style->leftBorder()->width(),
                                style->rightBorder()->width(),
                                style->topBorder()->width(),
                                style->bottomBorder()->width());
    int fillId = 0;
    if("FF" + convertColor(style->m_brushColor) != "FF000000"){
        fillId = styles_document->addFill("FF" + convertColor(style->m_brushColor));
    }
//    qDebug() << "ВЫСОТА ЯЧЕЙКИ - " << cell_block->height();
//    qDebug() << "ШИРИНА ЯЧЕЙКИ - " << cell_block->width;
    sheet_document->addWidthCell((cell_block->width / 100) * 0.5);
    sheet_document->addHeightCell(cell_block->height() / 35);

    /*В обычных клетках эти значения по нулям*/
    if(cell_block->mergeColums != 0 || cell_block->mergeRows != 0){
        sheet_document->addMergeCells(cell_block->mergeRows, cell_block->mergeColums);
    }

    sheet_document->setFillId(fillId);
    sheet_document->setBorderId(borderId);
    sharedStrings_document->setTempTextFromCell(cell_block->getText());
    qDebug() << cell_block->getText();
    if(cell_block->getText().isEmpty()){
        int styleId = styles_document->addXfToCellXfs(fillId,
                                                      borderId);
        sheet_document->addTextNumber(-1, styleId);
    }
//    sheet_document->addTextNumber(sharedStrings_document->getCountWords() - 1, styleId);
//    qDebug() <<"INDEX STRING - " <<sheet_document->getIndexString();
//    qDebug() <<"INDEX COLUMN - " <<sheet_document->getIndexColumn();
//    qDebug() << "Left width - " << style->leftBorder()->width();
//    qDebug() << "Right width - " << style->rightBorder()->width();
//    qDebug() << "Top width - " << style->topBorder()->width();
//    qDebug() << "Bottom width - " << style->bottomBorder()->width();
    sheet_document->newColumn();
    return index;
}

int SpaceWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						   EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index) {
    if(!sheet_document->getActiveTable()){
        sharedStrings_document->addSpase();
    }
    else{
        EjUtils utils;
        utils.validateForTextFromTable(sheet_document, sharedStrings_document, styles_document);
    }
    return index;
}

StyleWriterExcel::StyleWriterExcel()
{
    style_editors.insert(e_PropDoc::PARAGRAPH_STYLE, new StyleParagraphWriterExcel());
    style_editors.insert(e_PropDoc::TEXT_STYLE, new StyleTextWriterExcel());
}

int StyleWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *sharedStrings_document,
						   EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index){
    EjNumStyleBlock *block_style = (EjNumStyleBlock*)lBlocks->at(index);
    StyleExcelDocumentWriter* editor = style_editors.value(block_style->style->m_vid, nullptr);
    if (editor != nullptr) {
        editor->edit(sheet_document, sharedStrings_document, styles_document, block_style);
    }
    return index;
}

StyleWriterExcel::~StyleWriterExcel()
{

}

int StyleTextWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *shared_document,
								EjStylesDocument *styles_document, EjNumStyleBlock *block)
{
    EjTextStyle* style = (EjTextStyle*)block->style;
    shared_document->setBold(style->fontBold());
    shared_document->setItalic(style->fontItalic());
    shared_document->setStrikeOut(style->fontStrikeOut());
    shared_document->setUnderline(style->fontUnderline());
    shared_document->setColorText("FF" + convertColor(style->fontColor()));
    shared_document->setNameFont("Calibri");
    shared_document->setSizeText(style->m_font.pixelSize());
    return 0;
}

int StyleParagraphWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *shared_document,
									 EjStylesDocument *styles_document, EjNumStyleBlock *block)
{
    EjParagraphStyle* style = (EjParagraphStyle*)block->style;
    styles_document->setAlign(style->align());
    return 0;
}

int EnterWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *shared_document,
						   EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index)
{
    /*Проверка что ентер не идет сразу после таблицы, иначе последняя
      строчка таблицы забагуется, пока что так потом ИСПРАВИТЬ*/
    if(!sheet_document->getActiveTable()){
        if(index != 0 && lBlocks->at(index - 1)->type != e_typeBlocks::END_GROUP){
            shared_document->addText("", true);

            /*Применяем здесь стиль только ради alignment  */
            int styleId = styles_document->addXfToCellXfs(0, 0);
            sheet_document->addTextNumber(shared_document->getCountWords() - 1, styleId);
            shared_document->startNewString = false;
        }
        sheet_document->newString();
    }
    return 0;
}

int EndTableWriterExcel::edit(EjSheetDocument *sheet_document, EjSharedStringsDocument *shared_document,
							  EjStylesDocument *styles_document, QList<EjBlock *> *lBlocks, int index)
{
    sheet_document->setActiveTable(false);
     sheet_document->newString();
    sheet_document->setDefaultIndexColumn();
    return index;
}

StyleExcelDocumentWriter::~StyleExcelDocumentWriter()
{

}
