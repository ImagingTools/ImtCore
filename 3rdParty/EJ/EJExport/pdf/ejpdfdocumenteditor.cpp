#include "ejpdfdocumenteditor.h"
#include "ejtableblocks.h"
#include "export_global.h"
#include "standart_function.h"
#include <QDebug>

EjPdfDocumentWriter::~EjPdfDocumentWriter() {}

void TextWriterPdf::edit(EjPdfDocument& document, EjBlock* block, QPainter* painter)
{
	EjTextBlock* text_block = (EjTextBlock*)block;
	int x = (int)text_block->x / 27;
	int y = (int)text_block->y / 27;
    document.drawFillRect(painter, x, y - 13, (int)(text_block->width / 32), (int)(text_block->height() / 32));
    document.addText(painter, text_block->text, x, y);
}

void CellWriterPdf::edit(EjPdfDocument& document, EjBlock* block, QPainter* painter)
{
    EjCellBlock* cell_block = (EjCellBlock*)block;
    EjCellStyle* style = cell_block->cellStyle;
    int dy = 20, dx = 5;
    int width = (int)(cell_block->width / 27), height = (int)(cell_block->height() / 27);
    int x = (int)(cell_block->x / 27), y = (int)(cell_block->y / 27);

    document.setColorBackground(style->m_brushColor);
    document.drawFillRect(painter, x - dx, y - dy, width, height);
    if(cell_block->visible)
    {
        document.drawBorder(painter, x - dx, y - dy, width, height,
                            style->leftBorder()->width(), style->rightBorder()->width(),
                            style->topBorder()->width(), style->bottomBorder()->width());
    }
}

void StyleWriterPdf::edit(EjPdfDocument &document, EjBlock *block, QPainter* painter)
{
    EjNumStyleBlock *block_style = (EjNumStyleBlock*)block;
    if(block_style->style->m_vid == e_PropDoc::TEXT_STYLE)
    {
       document.m_fontStyleExist = true;
       EjTextStyle* text_style = (EjTextStyle*)block_style->style;
       document.setFont(painter, text_style->fontColor(), text_style->m_font);
       document.setColorBackground(text_style->brushColor());
    }
}

void ImageWriterPdf::edit(EjPdfDocument &document, EjBlock *block, QPainter *painter)
{
    ImageBlock *image_block = (ImageBlock*)block;
    int x = (int)image_block->x / 27;
    int y = (int)image_block->y / 27;
    document.drawImage(painter, image_block->m_smallImage, x, y);
}

void ByteArrayWriterPdf::edit(EjPdfDocument& document, EjBlock* block, QPainter *painter) {
	EjPropByteArrayBlock *prop = ((EjPropByteArrayBlock*)block);
}

void SpaceWriterPdf::edit(EjPdfDocument &document, EjBlock *block, QPainter *painter)
{
}
