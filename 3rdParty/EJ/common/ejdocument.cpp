#include "ejdocument.h"
#include "ejtableblocks.h"
#include "labelblock.h"


EjDocument::EjDocument(QUuid key, QObject *parent) :
    QObject(parent)
{
	lPropBlocks = new QList<EjBlock*>;
	lBlocks = new QList<EjBlock*>;
    lStrings = new QList<JString*>;
    lPages = new QList<Page*>;
	lTables = new QList<EjTableBlock*>;
    lLabels = new QList<LabelBlock*>;
    lLabels_bak = new QList<LargeTextBlock*>;
	lStyles = new QList<EjBaseStyle *>;
    m_attrProp = nullptr;
    m_lPatches = nullptr;
}

EjDocument::~EjDocument()
{
    clear();
    testProps();
    delete lPropBlocks;
    delete lBlocks;
    delete lStrings;
    delete lPages;
    delete lTables;
    delete lStyles;
    delete lLabels;
    if(m_lPatches)
    {
        qDeleteAll(*m_lPatches);
        m_lPatches->clear();
    }
}

void EjDocument::clear()
{
    if(lBlocks) {
        qDeleteAll(*lBlocks);
        lBlocks->clear();
    }
    if(lStrings) {
        qDeleteAll(*lStrings);
        lStrings->clear();
    }
    if(lPages) {
        qDeleteAll(*lPages);
        lPages->clear();
    }
    if(lTables) {
        lTables->clear();
    }
    if(lLabels)
        lLabels->clear();
    if(lPropBlocks)
    {
        qDeleteAll(*lPropBlocks);
        lPropBlocks->clear();
    }
    m_attrProp = nullptr;
    if(lStyles)
        lStyles->clear();
}

EjAttrProp *EjDocument::attributes()
{
    if(!m_attrProp)
    {
		m_attrProp = new EjAttrProp(this);
        m_attrProp->m_doc = this;
        m_attrProp->createDefault(lPropBlocks,0);
    }
    return m_attrProp;
}

void EjDocument::copy(EjDocument *doc)
{
    doc->clear();
    *doc->lBlocks = *lBlocks;
    *doc->lPropBlocks = *lPropBlocks;
    *doc->lStyles = *lStyles;
    *doc->lStrings = *lStrings;
    *doc->lPages = *lPages;
    *doc->lTables = *lTables;
    *doc->lLabels_bak = *lLabels_bak;
    *doc->lLabels = *lLabels;

}

void EjDocument::copyData(EjDocument *doc)
{
    doc->clear();
    for(int i = 0; i < lBlocks->count(); i++)
    {
        doc->lBlocks->append(lBlocks->at(i)->makeCopy());
    }
    for(int i = 0; i < lPropBlocks->count(); i++)
    {
        doc->lPropBlocks->append(lPropBlocks->at(i)->makeCopy());
    }
}

void EjDocument::move(EjDocument *doc)
{
    copy(doc);
    lBlocks->clear();
    lPropBlocks->clear();
    lStyles->clear();
    lStrings->clear();
    lPages->clear();
    lTables->clear();
}

EjBaseStyle *EjDocument::getUndefinedStyle(int num)
{
	EjBaseStyle *baseStyle = nullptr;
    if(num > -1 && num < lStyles->count())
    {
        baseStyle = lStyles->at(num);
    }
    return baseStyle;
}

EjTextStyle *EjDocument::getTextStyle(int num)
{
	EjTextStyle *txtStyle = NULL;
	EjBaseStyle *baseStyle = getUndefinedStyle(num);
    if(baseStyle && baseStyle->m_vid == TEXT_STYLE)
		txtStyle = dynamic_cast<EjTextStyle*>(baseStyle);
    return txtStyle;
}

EjTextStyle *EjDocument::createDefaultTextStyle()
{
	EjTextStyle *txtStyle = new EjTextStyle;
    txtStyle->num = lStyles->count();
    appendStyle(txtStyle);

    return txtStyle;
}

EjTextStyle *EjDocument::fromTextStyles(EjTextStyle *inputStyle)
{
	EjTextStyle *curStyle = NULL;
    int count;
    int max_count = -1;
	foreach (EjBaseStyle *style, *lStyles) {
        if(style->m_vid == TEXT_STYLE)
        {
            count = 0;
			EjTextStyle *textStyle = (EjTextStyle*)style;
            if(textStyle->m_fontColor == inputStyle->m_fontColor)
                count++;
            if(textStyle->m_font.family() == inputStyle->m_font.family())
                count++;
            if(textStyle->m_fontStyle == inputStyle->m_fontStyle)
                count++;
            if(textStyle->m_font.bold() == inputStyle->m_font.bold())
                count++;
            if(textStyle->m_font.italic() == inputStyle->m_font.italic())
                count++;
            if(textStyle->m_font.underline() == inputStyle->m_font.underline())
                count++;
            if(textStyle->m_font.pixelSize() == inputStyle->m_font.pixelSize())
                count++;
            if(textStyle->m_font.strikeOut() == inputStyle->m_font.strikeOut())
                count++;
            if(textStyle->m_brushColor == inputStyle->m_brushColor)
                count++;
            if(count > max_count)
            {
				curStyle = (EjTextStyle*)style;
                max_count = count;
            }
        }
    }
    if(!curStyle)
    {
        curStyle = createDefaultTextStyle();
    }
    if(max_count < 9)
    {
		EjTextStyle *newTextStyle = new EjTextStyle();
        inputStyle->copy(newTextStyle);
        newTextStyle->num = lStyles->count();
        newTextStyle->m_doc = this;
        appendStyle(newTextStyle);
        newTextStyle->setParentStyle(curStyle);
        curStyle = newTextStyle;
        curStyle->m_fontMetrics = QFontMetrics(curStyle->m_font);
    }
    return curStyle;
}

EjParagraphStyle *EjDocument::createDefaultPrgStyle()
{
	EjParagraphStyle *prgStyle = new EjParagraphStyle;
    prgStyle->num = lStyles->count();
    appendStyle(prgStyle);
    return prgStyle;
}

EjParagraphStyle *EjDocument::getParagraphStyle(int num)
{
	EjParagraphStyle *prgStyle = nullptr;
	EjBaseStyle *baseStyle = getUndefinedStyle(num);
    if(baseStyle && baseStyle->m_vid == PARAGRAPH_STYLE)
		prgStyle = dynamic_cast<EjParagraphStyle*>(baseStyle);

    return prgStyle;
}

EjParagraphStyle *EjDocument::fromParagraphStyles(EjParagraphStyle *inputStyle)
{
	EjParagraphStyle *curStyle = NULL;
    int count;
    int max_count = -1;
	foreach (EjBaseStyle *style, *lStyles) {
        if(style->m_vid == PARAGRAPH_STYLE)
        {
            count = 0;
			EjParagraphStyle *prgStyle = (EjParagraphStyle*)style;
            if(prgStyle->m_align == inputStyle->m_align)
                count++;
            if(count > max_count)
            {
				curStyle = (EjParagraphStyle*)style;
                max_count = count;
            }
        }
    }
    if(max_count < 1)
    {
		EjParagraphStyle *newStyle = new EjParagraphStyle();
        inputStyle->copy(newStyle);
        newStyle->num = lStyles->count();
        newStyle->m_parentStyle = curStyle;
        newStyle->m_doc = this;
        appendStyle(newStyle);
        newStyle->setAlign(newStyle->m_align);
        curStyle = newStyle;
    }
    return curStyle;
}

EjCellStyle *EjDocument::createDefaultCellStyle()
{
	EjCellStyle *cellStyle = new EjCellStyle;
    cellStyle->num = lStyles->count();
    appendStyle(cellStyle);
    return cellStyle;
}

EjCellStyle *EjDocument::getCellStyle(int num)
{
	EjCellStyle *cellStyle = nullptr;
	EjBaseStyle *baseStyle = getUndefinedStyle(num);
    if(baseStyle && baseStyle->m_vid == CELL_STYLE)
		cellStyle = dynamic_cast<EjCellStyle*>(baseStyle);
    return cellStyle;
}

EjTextStyle *EjDocument::currentTextStyle(int block)
{
	EjTextStyle *curTextStyle = nullptr;
	EjBlock *cur_block;
    int i = block;

    if(i > lBlocks->count() - 1)
       i = lBlocks->count() - 1;

    while(i > -1 && !lBlocks->isEmpty())
    {
        cur_block = lBlocks->at(i);
        if(cur_block->type == NUM_STYLE)
        {
			if(((EjNumStyleBlock *)cur_block)->style)
            {
				if(((EjNumStyleBlock *)cur_block)->style->m_vid == TEXT_STYLE)
                {
					curTextStyle = (EjTextStyle*)((EjNumStyleBlock *)cur_block)->style;
                    break;
                }
            }
            else
            {
				curTextStyle = getTextStyle(((EjNumStyleBlock *)cur_block)->num);
                if(curTextStyle) {
					((EjNumStyleBlock *)cur_block)->style = curTextStyle;
                    break;
                }
            }
        }
        i--;
    }
    if(!curTextStyle) {
		foreach (EjBaseStyle *style, *lStyles) {
            if(style->m_vid == TEXT_STYLE)
            {
				curTextStyle = (EjTextStyle*)style;
            }
        }
        if(!curTextStyle)
            curTextStyle = createDefaultTextStyle();
    }
    return curTextStyle;

}

EjParagraphStyle *EjDocument::currentParagraphStyle(int block)
{
	EjParagraphStyle *curPrgStyle = NULL;
	EjBlock *cur_block;
    int i = block;
    bool bFind = false;
    if(lBlocks->isEmpty()) {
        return curPrgStyle;
    }
    while(i > -1 && !lBlocks->isEmpty())
    {
        cur_block = lBlocks->at(i);
        if(cur_block->type == NUM_STYLE)
        {
			if(((EjNumStyleBlock *)cur_block)->style)
            {
				if(((EjNumStyleBlock *)cur_block)->style->m_vid == PARAGRAPH_STYLE)
                {
					curPrgStyle = (EjParagraphStyle*)((EjNumStyleBlock *)cur_block)->style;
                    break;
                }
            }
            else
            {
				curPrgStyle = getParagraphStyle(((EjNumStyleBlock *)cur_block)->num);
                if(curPrgStyle) {
					((EjNumStyleBlock *)cur_block)->style = curPrgStyle;
                    break;
                }
            }

        }
        i--;
    }
    if(!curPrgStyle) {
		foreach (EjBaseStyle *style, *lStyles) {
            if(style->m_vid == PARAGRAPH_STYLE)
            {
				curPrgStyle = (EjParagraphStyle*)style;
                break;
            }
        }
        if(!curPrgStyle)
            curPrgStyle = createDefaultPrgStyle();
    }
    return curPrgStyle;

}

EjCellStyle *EjDocument::currentCellStyle(int block)
{
	EjCellStyle *curCellStyle = nullptr;
	EjBlock *cur_block;
    int i = block;

    if(i > lBlocks->count() - 1)
       i = lBlocks->count() - 1;

    while(i > -1 && !lBlocks->isEmpty())
    {
        cur_block = lBlocks->at(i);
        if(cur_block->type == NUM_STYLE)
        {
			if(((EjNumStyleBlock *)cur_block)->style)
            {
				if(((EjNumStyleBlock *)cur_block)->style->m_vid == CELL_STYLE)
                {
					curCellStyle = (EjCellStyle*)((EjNumStyleBlock *)cur_block)->style;
                    break;
                }
            }
            else
            {
				curCellStyle = getCellStyle(((EjNumStyleBlock *)cur_block)->num);
                if(curCellStyle) {
					((EjNumStyleBlock *)cur_block)->style = curCellStyle;
                    break;
                }
            }
        }
        i--;
    }
    if(!curCellStyle) {
		foreach (EjBaseStyle *style, *lStyles) {
            if(style->m_vid == TEXT_STYLE)
            {
				curCellStyle = (EjCellStyle*)style;
                break;
            }
        }
        if(!curCellStyle)
            curCellStyle = createDefaultCellStyle();
    }
    return curCellStyle;

}

int EjDocument::nextTableKey()
{
    int key = 0;
	EjTableBlock *table;
    for (int i = 0; i < lTables->count(); i++) {
        table = lTables->at(i);
        if(key < table->key)
            key = table->key;
    }
    key++;
    return key;
}

int EjDocument::nextLabelKey()
{
    int key = 0;
    LabelBlock *label;
    for (int i = 0; i < lLabels->count(); i++) {
        label = lLabels->at(i);
        if(key < label->key)
            key = label->key;
    }
    key++;
    return key;

}

void EjDocument::appendStyle(EjBaseStyle *style)
{
    lStyles->append(style);
    int index = lPropBlocks->size();
    if(index < 0)
        index = 0;
    style->createDefault(lPropBlocks, index);
}

void EjDocument::calcProps()
{
    lStyles->clear();
	EjTextControl control;
    EjCalcParams params;
	QList<EjBlock*> *lBlocks_tmp = lBlocks;
    lBlocks = lPropBlocks;
    control.doc = this;
    params.control = &control;
    params.force = true;

    for(int i = 0; i < lPropBlocks->size(); i++)
    {
        lPropBlocks->at(i)->calcBlock(i,&params);
    }
    lBlocks = lBlocks_tmp;
}

bool EjDocument::testProps()
{
    for(int i = 0; i < lPropBlocks->count(); i++)
    {
		EjBlock *block = lPropBlocks->at(i);
        for(int j = i + 1; j < lPropBlocks->count(); j++ )
        {
            if(block == lPropBlocks->at(j))
            {
                qDebug() << __FILE__ << __LINE__ << "Dublicate Props!!!!!!!!!!!!!!!!!!!!!!!!!!";
                return false;
            }
        }
    }
    return true;
}
