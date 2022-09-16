#include "ejworddocumenteditors.h"

#include "ejtableblocks.h"
#include "export_global.h"

EjWordDocumentWriter::~EjWordDocumentWriter() {}

StyleDocumentWriter::~StyleDocumentWriter() {}

bool TextWriterWord::edit(EjWordDocument& document, EjBlock* block) {
	return document.addText(((EjTextBlock*)block)->text);
}

bool SpaceWriterWord::edit(EjWordDocument& document, EjBlock*) {
    return document.addSpace();
}

bool EnterWriterWord::edit(EjWordDocument& document, EjBlock*) {
    return document.endParagraph();
}

bool TableWriterWord::edit(EjWordDocument& document, EjBlock* block) {
    EjTableBlock *table_block = (EjTableBlock*)block;
    QList<int> widths;
    for(int i=0; i < table_block->lColums.count();++i){
        widths.append(int(table_block->lColums.at(i)->sizeProp.current*CM_TO_TWIP));
    }
    return document.createTable(int(table_block->width*CM_TO_TWIP), 0, widths, table_block->nColums(), table_block->nRows());
}

bool CellWriterWord::edit(EjWordDocument& document, EjBlock* block) {
    EjCellBlock* cell_block = (EjCellBlock*)block;
    EjCellStyle* style = cell_block->cellStyle;

    return document.addCell(int(cell_block->width*CM_TO_TWIP), cell_block->mergeRows, cell_block->mergeColums, style->m_brushColor,
                            style->topBorder()->m_width, style->leftBorder()->m_width,
                            style->rightBorder()->m_width, style->bottomBorder()->m_width, cell_block->getText().isEmpty());
}

bool ImageWriterWord::edit(EjWordDocument& document, EjBlock* block) {
    ImageBlock* image_block = (ImageBlock*)block;
    return document.addPict(int(double(image_block->width)*IMAGE_RATIO), int(double(image_block->height())*IMAGE_RATIO));
}

bool ByteArrayWriterWord::edit(EjWordDocument& document, EjBlock* block) {
	return document.addImage(((EjPropByteArrayBlock*)block)->data.toHex());
}

bool EndGroupWriterWord::edit(EjWordDocument& document, EjBlock*) {
    return document.endGroup();
}

StyleWriterWord::StyleWriterWord() {
    style_editors.insert(e_PropDoc::PARAGRAPH_STYLE, new StyleParagraphWriterWord());
    style_editors.insert(e_PropDoc::TEXT_STYLE, new StyleTextWriterWord());
}

bool StyleWriterWord::edit(EjWordDocument& document, EjBlock* block) {
    EjNumStyleBlock *bloc_style = (EjNumStyleBlock*)block;
    bool result = true;
    StyleDocumentWriter* editor = style_editors.value(bloc_style->style->m_vid, nullptr);

    if (editor != nullptr) {
        result = editor->edit(document, bloc_style);
    }
    return result;
}

StyleWriterWord::~StyleWriterWord() {
    for (QMap<quint8, StyleDocumentWriter*>::iterator i = style_editors.begin(); i != style_editors.end(); ++i) {
        delete *i;
    }
}

bool StyleParagraphWriterWord::edit(EjWordDocument& document, EjNumStyleBlock *block) {
    qDebug() << "Алигн: " << ((EjParagraphStyle*)block->style)->m_align;
    return document.setAilgn(((EjParagraphStyle*)block->style)->m_align);
}

bool StyleTextWriterWord::edit(EjWordDocument& document, EjNumStyleBlock *block) {
    EjTextStyle *text_style = (EjTextStyle*)block->style;
    bool result = document.setColorText(text_style->m_fontColor);
    result &= document.setHighlightColor(text_style->m_brushColor);
    result &= document.setBold(text_style->m_font.bold());
    result &= document.setItalic(text_style->m_font.italic());
    result &= document.setUnderLine(text_style->m_font.underline());
    result &= document.setStrikeOut(text_style->m_font.strikeOut());
    //qDebug() << int(double(text_style->fontSize())*TEXT_ASPECT_RATIO);
    result &= document.setTextSize(int(double(text_style->fontSize())*TEXT_ASPECT_RATIO));
    return result;
}
