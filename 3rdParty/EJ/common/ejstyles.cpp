#include "../include/ejstyles.h"
#include "../include/ejdocument.h"
#include "ejtextcontrol.h"
#include <QFont>


EjBaseStyle *EjBaseStyle::getStyleByNum(int num, QList<EjBaseStyle *> *lStyles)
{
    EjBaseStyle *res = nullptr;
    if(num > -1 && num < lStyles->count())
        res = lStyles->at(num);
    return res;
}

void EjBaseStyle::createDefault(QList<EjBlock *> *lBlocks, int index)
{
    m_index = index;
    lBlocks->insert(index,this);
    index++;
	lBlocks->insert(index,new EjPropIntBlock(STYLE_NUM));
	((EjPropIntBlock*)lBlocks->at(index))->value = num;
    index++;
	lBlocks->insert(index,new EjBlock(END_GROUP,this));
    m_counts = 2;
}



EjTextStyle::EjTextStyle(QObject *parent):
    EjBaseStyle(parent),
    m_fontMetrics(m_font)
{
    m_vid = TEXT_STYLE;
    setDefault();
    m_underlineChange = false;
    m_sizeChange = false;
    m_boldChange = false;
    m_italicChange = false;
    m_strikeOutChange = false;
}

void EjTextStyle::setDefault()
{
    m_fontStyle = NORMAL;
    m_parentStyle = NULL;
    m_fontColor = QColor(Qt::black);
    m_brushColor = QColor(Qt::transparent);
    m_font = QFont("Helvetica [Cronyx]");
    m_font.setPixelSize(12);
    m_font.setBold(false);
    m_font.setUnderline(false);
    m_font.setItalic(false);
    m_fontMetrics = QFontMetrics(m_font);
}

EjBlock *EjTextStyle::makeCopy()
{
    EjTextStyle *res = new EjTextStyle();
    copy(res);
    return res;
}

EjTextStyle *EjTextStyle::makeTextStyle()
{
    return (EjTextStyle*) makeCopy();
}

void EjTextStyle::copy(EjTextStyle *style)
{
    style->type = type;
    style->num = num;
    style->m_font = m_font;
    style->m_fontColor = m_fontColor;
    style->m_brushColor = m_brushColor;
    style->m_fontStyle = m_fontStyle;
}

void EjTextStyle::copyChanges(EjTextStyle *style)
{
    style->m_boldChange = m_boldChange;
    style->m_sizeChange = m_sizeChange;
    style->m_underlineChange = m_underlineChange;
    style->m_strikeOutChange = m_strikeOutChange;
    style->m_italicChange = m_italicChange;
}


bool EjTextStyle::fullCompare(const EjBlock *other) const
{
    if(!EjBaseStyle::compare(*other)) {
        return false;
    }
    bool res = true;
    EjTextStyle *cur_textStyle = (EjTextStyle *)other;
    if(this->m_font != cur_textStyle->m_font || this->m_fontColor != cur_textStyle->m_fontColor
            || this->m_brushColor != cur_textStyle->m_brushColor || this->m_fontStyle != cur_textStyle->m_fontStyle)
        res = false;
    return res;
}

void EjTextStyle::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
	EjPropInt8Block *curInt8;
	EjPropColorBlock *curColor;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == STYLE_NUM)
        {
            num = curInt->value;
        }
        else if(curInt->num == STYLE_PARENT_NUM)
        {
            if(m_doc && m_doc->lStyles)
            {
                m_parentStyle = dynamic_cast<EjTextStyle*>(getStyleByNum(curInt->value,m_doc->lStyles));
                if(m_parentStyle)
                {
                    EjTextStyle* parentStyle = dynamic_cast<EjTextStyle*>(m_parentStyle);
                    if(!parentStyle) break;
                    m_font = parentStyle->m_font;
                    m_fontMetrics = QFontMetrics(m_font);

                }
            }
        }
        else if(curInt->num == FONT_SIZE)
        {
            m_font.setPixelSize(curInt->value);
            m_fontMetrics = QFontMetrics(m_font);
        }
        else if(curInt->num == FONT_STYLE)
        {
            m_fontStyle = (e_fontStyle)curInt->value;
        }
        break;
    case PROP_INT8:
		curInt8 = (EjPropInt8Block *)child;
        if(curInt8->num == FONT_BOLD)
        {
            m_font.setBold(curInt8->value);
            m_fontMetrics = QFontMetrics(m_font);
        }
        else if(curInt8->num == FONT_UNDERLINE)
        {
            m_font.setUnderline(curInt8->value);
            m_fontMetrics = QFontMetrics(m_font);
        }
        else if(curInt8->num == FONT_STRIKE_OUT)
        {
            m_font.setStrikeOut(curInt8->value);
            m_fontMetrics = QFontMetrics(m_font);
        }
        else if(curInt8->num == FONT_ITALIC)
        {
            m_font.setItalic(curInt8->value);
            m_fontMetrics = QFontMetrics(m_font);
        }
        break;
    case PROP_COLOR:
		curColor = (EjPropColorBlock *)child;
        if(curColor->num == FONT_COLOR)
        {
            m_fontColor = curColor->color;
        }
        else if(curColor->num == FONT_BRUSH_COLOR)
        {
            m_brushColor = curColor->color;
        }
        break;
    default:
        break;
    }
}

bool EjTextStyle::underlineChange() const
{
    return m_underlineChange;
}

bool EjTextStyle::sizeChange() const
{
    return m_sizeChange;
}

bool EjTextStyle::boldChange() const
{
    return m_boldChange;
}

bool EjTextStyle::italicChange() const
{
    return m_italicChange;
}

bool EjTextStyle::strikeOutChange() const
{
    return m_strikeOutChange;
}

bool EjTextStyle::styleChange() const
{
    return m_styleChange;
}

bool EjTextStyle::familyChange() const
{
    return m_familyChange;
}

bool EjTextStyle::colorChange() const
{
    return m_colorChange;
}

bool EjTextStyle::brushChange() const
{
    return m_brushChange;
}

EjTextStyle::e_fontStyle EjTextStyle::fontStyle() const
{
    return m_fontStyle;
}

QString EjTextStyle::fontFamily() const
{
    return m_font.family();
}

bool EjTextStyle::fontBold() const
{
    return m_font.bold();
}

bool EjTextStyle::fontItalic() const
{
    return m_font.italic();
}

bool EjTextStyle::fontStrikeOut() const
{
    return m_font.strikeOut();
}

QColor EjTextStyle::fontColor() const
{
    return m_fontColor;
}

QColor EjTextStyle::brushColor() const
{
    return m_brushColor;
}

int EjTextStyle::fontSize() const
{
    return m_font.pixelSize();
}

bool EjTextStyle::fontUnderline() const
{
    return m_font.underline();
}

void EjTextStyle::beforeCalc(EjCalcParams *calcParams)
{
    if(calcParams->control->doc)
    {
        m_doc = calcParams->control->doc;
        m_doc->lStyles->append(this);
    }
}

void EjTextStyle::setParentStyle(EjTextStyle *parentStyle)
{
    if (!m_doc || m_parentStyle)
        return;
	EjBlock *curBlock = m_doc->lPropBlocks->at(m_index);
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, STYLE_PARENT_NUM);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(STYLE_PARENT_NUM);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = m_doc->lStyles->indexOf(parentStyle);

    m_parentStyle = parentStyle;
    if(m_font.family() != parentStyle->m_font.family())
        setFontFamily(m_font.family());
    if(m_font.bold() != parentStyle->m_font.bold())
        setFontBold(m_font.bold());
    if(m_font.italic() != parentStyle->m_font.italic())
        setFontItalic(m_font.italic());
    if(m_font.underline() != parentStyle->m_font.underline())
        setFontUnderline(m_font.underline());
    if(m_font.strikeOut() != parentStyle->m_font.strikeOut())
        setFontStrikeOut(m_font.strikeOut());
    if(m_font.pixelSize() != parentStyle->m_font.pixelSize())
        setFontSize(m_font.pixelSize());
    if(m_fontColor != parentStyle->m_fontColor)
        setFontColor(m_fontColor);
    if(m_brushColor != parentStyle->m_brushColor)
        setBrushColor(m_brushColor);
    if(m_fontStyle != parentStyle->m_fontStyle)
        setFontStyle(m_fontStyle);
}

void EjTextStyle::normalizeStyle(EjTextStyle *referenceStyle, bool force)
{
    if(force)
        referenceStyle->copy(this);
    else
    {
        if(referenceStyle->boldChange())
            setFontBold(referenceStyle->fontBold());
        if(referenceStyle->sizeChange())
            setFontSize(referenceStyle->fontSize());
        if(referenceStyle->underlineChange())
            setFontUnderline(referenceStyle->fontUnderline());
        if(referenceStyle->italicChange())
            setFontItalic(referenceStyle->fontItalic());
        if(referenceStyle->strikeOutChange())
            setFontStrikeOut(referenceStyle->fontStrikeOut());
        if(referenceStyle->styleChange())
            setFontStyle(referenceStyle->fontStyle());
        if(referenceStyle->familyChange())
            setFontFamily(referenceStyle->fontFamily());
        if(referenceStyle->colorChange())
            setFontColor(referenceStyle->fontColor());
        if(referenceStyle->brushChange())
            setBrushColor(referenceStyle->brushColor());
    }
    m_fontMetrics = QFontMetrics(m_font);
}

void EjTextStyle::setFontStyle(EjTextStyle::e_fontStyle fontStyle)
{
    m_fontStyle = fontStyle;
    setStyleChange(true);
    if (!m_doc) {
        emit fontStyleChanged(m_fontStyle);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, FONT_STYLE);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(FONT_STYLE);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontStyle;

    emit fontStyleChanged(m_fontStyle);
}

void EjTextStyle::setFontFamily(QString fontFamily)
{
    m_font.setFamily(fontFamily);
    setFamilyChange(true);
    if (!m_doc) {
        emit fontFamilyChanged(m_font.family());
        return;
    }
	EjPropTextBlock *propText = (EjPropTextBlock*)findProp(m_doc->lPropBlocks, PROP_TXT, FONT_FAMILY);
    if(!propText)
    {
		propText = new EjPropTextBlock(FONT_FAMILY);
        addProp(m_doc->lPropBlocks,propText);
    }
    propText->text = fontFamily;

    emit fontFamilyChanged(m_font.family());
}

void EjTextStyle::setFontBold(bool fontBold)
{
    m_font.setBold(fontBold);
    setBoldChange(true);
    if (!m_doc) {
        emit fontBoldChanged(fontBold);
        return;
    }
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, FONT_BOLD);
    if(!propInt)
    {
		propInt = new EjPropInt8Block(FONT_BOLD);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontBold;

    emit fontBoldChanged(fontBold);
}

void EjTextStyle::setFontItalic(bool fontItalic)
{
    m_font.setItalic(fontItalic);
    setItalicChange(true);
    if (!m_doc) {
        emit fontItalicChanged(fontItalic);
        return;
    }
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, FONT_ITALIC);
    if(!propInt)
    {
		propInt = new EjPropInt8Block(FONT_ITALIC);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontItalic;

    emit fontItalicChanged(fontItalic);
}

void EjTextStyle::setFontStrikeOut(bool fontStrikeOut)
{
    m_font.setStrikeOut(fontStrikeOut);
    setStrikeOutChange(true);
    if (!m_doc) {
        emit fontStrikeOutChanged(fontStrikeOut);
        return;
    }
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, FONT_STRIKE_OUT);
    if(!propInt)
    {
		propInt = new EjPropInt8Block(FONT_STRIKE_OUT);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontStrikeOut;

    emit fontStrikeOutChanged(fontStrikeOut);
}

void EjTextStyle::setFontColor(QColor fontColor)
{
    m_fontColor = fontColor;
    setColorChange(true);
    if (!m_doc) {
        emit fontColorChanged(m_fontColor);
        return;
    }
	EjPropColorBlock *propColor = (EjPropColorBlock*)findProp(m_doc->lPropBlocks, PROP_COLOR, FONT_COLOR);
    if(!propColor)
    {
		propColor = new EjPropColorBlock(FONT_COLOR);
        addProp(m_doc->lPropBlocks,propColor);
    }
    propColor->color = fontColor;

    m_fontColor = fontColor;
    emit fontColorChanged(m_fontColor);
}

void EjTextStyle::setBrushColor(QColor brushColor)
{
    m_brushColor = brushColor;
    setBrushChange(true);
    if (!m_doc) {
        emit brushColorChanged(m_brushColor);
        return;
    }
	EjPropColorBlock *propColor = (EjPropColorBlock*)findProp(m_doc->lPropBlocks, PROP_COLOR, FONT_BRUSH_COLOR);
    if(!propColor)
    {
		propColor = new EjPropColorBlock(FONT_BRUSH_COLOR);
        addProp(m_doc->lPropBlocks,propColor);
    }
    propColor->color = brushColor;

    emit brushColorChanged(m_brushColor);
}

void EjTextStyle::setFontSize(int fontSize)
{
    m_font.setPixelSize(fontSize);
    setSizeChange(true);
    if (!m_doc) {
        emit fontSizeChanged(fontSize);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT8, FONT_SIZE);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(FONT_SIZE);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontSize;

    emit fontSizeChanged(fontSize);
}

void EjTextStyle::setFontUnderline(bool fontUnderline)
{
    m_font.setUnderline(fontUnderline);
    setUnderlineChange(true);
    if (!m_doc) {
        emit fontUnderlineChanged(fontUnderline);
        return;
    }
	EjPropInt8Block *propInt = (EjPropInt8Block*)findProp(m_doc->lPropBlocks, PROP_INT8, FONT_UNDERLINE);
    if(!propInt)
    {
		propInt = new EjPropInt8Block(FONT_UNDERLINE);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = fontUnderline;

    emit fontUnderlineChanged(fontUnderline);
}

void EjTextStyle::setUnderlineChange(bool underlineChange)
{
    if (m_underlineChange == underlineChange)
        return;

    m_underlineChange = underlineChange;
    emit underlineChangeChanged(m_underlineChange);
}

void EjTextStyle::setSizeChange(bool sizeChange)
{
    if (m_sizeChange == sizeChange)
        return;

    m_sizeChange = sizeChange;
    emit sizeChangeChanged(m_sizeChange);
}

void EjTextStyle::setBoldChange(bool boldChange)
{
    if (m_boldChange == boldChange)
        return;

    m_boldChange = boldChange;
    emit boldChangeChanged(m_boldChange);
}

void EjTextStyle::setItalicChange(bool italicChange)
{
    if (m_italicChange == italicChange)
        return;

    m_italicChange = italicChange;
    emit italicChangeChanged(m_italicChange);
}

void EjTextStyle::setStrikeOutChange(bool strikeOutChange)
{
    if (m_strikeOutChange == strikeOutChange)
        return;

    m_strikeOutChange = strikeOutChange;
    emit strikeOutChangeChanged(m_strikeOutChange);
}

void EjTextStyle::setStyleChange(bool styleChange)
{
    if (m_styleChange == styleChange)
        return;

    m_styleChange = styleChange;
    emit styleChangeChanged(m_styleChange);
}

void EjTextStyle::setFamilyChange(bool familyChange)
{
    if (m_familyChange == familyChange)
        return;

    m_familyChange = familyChange;
    emit familyChangeChanged(m_familyChange);
}

void EjTextStyle::setColorChange(bool colorChange)
{
    if (m_colorChange == colorChange)
        return;

    m_colorChange = colorChange;
    emit colorChangeChanged(m_colorChange);
}

void EjTextStyle::setBrushChange(bool brushChange)
{
    if (m_brushChange == brushChange)
        return;

    m_brushChange = brushChange;
    emit brushChangeChanged(m_brushChange);
}


EjParagraphStyle::EjParagraphStyle(QObject *parent):EjBaseStyle(parent)
{
    m_vid = PARAGRAPH_STYLE;
    m_align = AlignLeft; // | AlignBaseline;
}

EjParagraphStyle *EjParagraphStyle::makePrgStyle()
{
    return (EjParagraphStyle*) makeCopy();
}

EjBlock *EjParagraphStyle::makeCopy()
{
    EjParagraphStyle *res = new EjParagraphStyle();
    res->type = type;
    res->num = num;
    res->m_align = m_align;
    return res;
}

void EjParagraphStyle::copy(EjParagraphStyle *style)
{
    style->type = type;
    style->num = num;
    style->m_align = m_align;
}

bool EjParagraphStyle::fullCompare(const EjBlock *other) const
{
    if(!EjBaseStyle::compare(*other)) {
        return false;
    }
    bool res = true;
    EjParagraphStyle *cur_paragraphStyle = (EjParagraphStyle *)other;
    if(this->m_align != cur_paragraphStyle->m_align)
        res = false;
    return res;
}

void EjParagraphStyle::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == STYLE_NUM)
        {
            num = curInt->value;
        }
        else if(curInt->num == PRG_ALIGN)
        {
            m_align = (Align)curInt->value;
        }
        break;
    default:
        break;
    }
}

void EjParagraphStyle::beforeCalc(EjCalcParams *calcParams)
{
    if(calcParams->control->doc)
    {
        m_doc = calcParams->control->doc;
        m_doc->lStyles->append(this);
    }
}

int EjParagraphStyle::align() const
{
    return m_align;
}

void EjParagraphStyle::setAlign(int align)
{
//    qDebug() << __FILE__ << __LINE__ << align;
    m_align = align;
    if (!m_doc) {
        emit alignChanged(m_align);
        return;
    }
	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, PRG_ALIGN);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(PRG_ALIGN);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = align;

    emit alignChanged(m_align);
}


EjBorderStyle::EjBorderStyle(QObject *parent) : EjBaseStyle(parent)
{
     m_vid = BORDER_STYLE;
     m_width = -1; m_penType = SOLID; m_penColor = QColor(Qt::black);
}


bool EjBorderStyle::fullCompare(const EjBlock *other) const
{
    if(!EjBaseStyle::compare(*other)) {
        return false;
    }
    bool res = true;
    EjBorderStyle *cur_borderStyle = (EjBorderStyle *)other;
    if(this->m_penColor != cur_borderStyle->m_penColor || this->m_width != cur_borderStyle->m_width
            || this->m_penType != cur_borderStyle->m_penType)
        res = false;
    return res;
}

void EjBorderStyle::operator =(const EjBorderStyle &other)
{
    m_width = other.m_width;
    m_penType = other.m_penType;
    m_penColor = other.m_penColor;
}

void EjBorderStyle::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
	EjPropColorBlock *curColor;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == STYLE_NUM)
        {
            num = curInt->value;
        }
        else if(curInt->num == BORDER_WIDTH)
        {
            m_width = curInt->value;
        }
        break;
    case PROP_COLOR:
		curColor = (EjPropColorBlock *)child;
        if(curColor->num == BORDER_COLOR)
        {
            m_penColor = curColor->color;
        }
        break;
    default:
        break;
    }

}

EjBlock *EjBorderStyle::makeCopy()
{
    EjBorderStyle *res = new EjBorderStyle();
    copyData(res);
    res->m_vid = m_vid;
    res->m_counts = m_counts;
    res->num = num;
    return res;
}

int EjBorderStyle::width() const
{
    return m_width;
}

QColor EjBorderStyle::penColor() const
{
    return m_penColor;
}

void EjBorderStyle::setWidth(int width)
{
    if (m_doc)
    {
		EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, BORDER_WIDTH);
        if(!propInt)
        {
			propInt = new EjPropIntBlock(BORDER_WIDTH);
            addProp(m_doc->lPropBlocks,propInt);
        }
        propInt->value = width;
    }
    m_width = width;
    emit widthChanged(width);
}

void EjBorderStyle::setPenColor(QColor penColor)
{
    if (m_doc)
    {
		EjPropColorBlock *propColor = (EjPropColorBlock*)findProp(m_doc->lPropBlocks, PROP_INT, BORDER_COLOR);
        if(!propColor)
        {
			propColor = new EjPropColorBlock(BORDER_COLOR);
            addProp(m_doc->lPropBlocks,propColor);
        }
        propColor->color = penColor;
    }
    m_penColor = penColor;
    emit penColorChanged(m_penColor);
}


EjCellStyle::EjCellStyle(QObject *parent) : EjBaseStyle(parent),
    m_topBorder_st(CELL_TOP),
    m_bottomBorder_st(CELL_BOTTOM),
    m_leftBorder_st(CELL_LEFT),
    m_rightBorder_st(CELL_RIGHT),
    m_horisontalLine_st(CELL_HORISONTAL),
    m_verticalLine_st(CELL_VERTICAL)
{
    m_vid = CELL_STYLE;
    m_brushColor = QColor(Qt::transparent);
    m_topBorder = &m_topBorder_st;
    m_bottomBorder = &m_bottomBorder_st;
    m_leftBorder = &m_leftBorder_st;
    m_rightBorder = &m_rightBorder_st;
    m_horisontalLine = &m_horisontalLine_st;
    m_verticalLine = &m_verticalLine_st;
}

EjCellStyle::~EjCellStyle()
{

}


bool EjCellStyle::fullCompare(const EjBlock *other) const
{
    if(!EjBaseStyle::compare(*other)) {
        return false;
    }
    bool res = true;
    EjCellStyle *cur_cellStyle = (EjCellStyle *)other;
    if(! this->m_topBorder->fullCompare(cur_cellStyle->m_topBorder) || ! this->m_bottomBorder->fullCompare(cur_cellStyle->m_bottomBorder)
            || ! this->m_leftBorder->fullCompare(cur_cellStyle->m_leftBorder) || ! this->m_rightBorder->fullCompare(cur_cellStyle->m_rightBorder)
//            || *this->m_verticalLine != *cur_cellStyle->m_verticalLine || *this->m_horisontalLine != *cur_cellStyle->m_horisontalLine
            || this->m_brushColor != cur_cellStyle->m_brushColor)
        res = false;
    return res;
}

void EjCellStyle::operator =(const EjCellStyle &other)
{
    m_brushColor = other.m_brushColor;
    *m_topBorder = *other.m_topBorder;
    *m_bottomBorder = *other.m_bottomBorder;
    *m_leftBorder = *other.m_leftBorder;
    *m_rightBorder = *other.m_rightBorder;
    *m_horisontalLine = *other.m_horisontalLine;
    *m_verticalLine = *other.m_verticalLine;
}

void EjCellStyle::beforeCalc(EjCalcParams *calcParams)
{
    if(calcParams->control->doc)
    {
        m_doc = calcParams->control->doc;
        m_doc->lStyles->append(this);
    }
}


void EjCellStyle::childCalc(EjBlock *child, EjCalcParams *calcParams)
{
    Q_UNUSED(calcParams)
	EjPropIntBlock *curInt;
	EjPropColorBlock *curColor;
    EjBorderStyle *borderStyle;
    switch (child->type) {
    case PROP_INT:
		curInt = (EjPropIntBlock *)child;
        if(curInt->num == STYLE_NUM)
        {
            num = curInt->value;
        }
        else if(curInt->num == STYLE_PARENT_NUM)
        {
            if(m_doc && m_doc->lStyles)
            {
                m_parentStyle = getStyleByNum(curInt->value,m_doc->lStyles);
                if(m_parentStyle)
                {
                    EjCellStyle* parentStyle = dynamic_cast<EjCellStyle*>(m_parentStyle);
                    if(!parentStyle) break;
                    *this = *parentStyle;
                }
            }
        }
        break;
    case PROP_DOC:
		if(((EjPropDoc*)child)->m_vid == BORDER_STYLE)
        {
           borderStyle = (EjBorderStyle *)child;
           if(borderStyle->num == CELL_TOP)
               m_topBorder = borderStyle;
           else if(borderStyle->num == CELL_BOTTOM)
               m_bottomBorder = borderStyle;
           else if(borderStyle->num == CELL_LEFT)
               m_leftBorder = borderStyle;
           else if(borderStyle->num == CELL_RIGHT)
               m_rightBorder = borderStyle;
        }
        break;
    case PROP_COLOR:
		curColor = (EjPropColorBlock *)child;
        if(curColor->num == CELL_BRUSH_COLOR)
        {
            m_brushColor = curColor->color;
        }
        break;
    default:
        break;
    }
}

EjBlock *EjCellStyle::makeCopy()
{
    EjCellStyle *res = new EjCellStyle();
    copyData(res);
    res->m_vid = m_vid;
    res->m_counts = m_counts;
    res->num = num;
    return res;
}

EjBorderStyle *EjCellStyle::topBorder()
{
    return m_topBorder;
}

EjBorderStyle *EjCellStyle::bottomBorder()
{
    return m_bottomBorder;
}

EjBorderStyle *EjCellStyle::leftBorder()
{
    return m_leftBorder;
}

EjBorderStyle *EjCellStyle::rightBorder()
{
    return m_rightBorder;
}

EjBorderStyle *EjCellStyle::horisontalLine()
{
    return m_horisontalLine;
}

EjBorderStyle *EjCellStyle::verticalLine()
{
    return m_verticalLine;
}

void EjCellStyle::setTopBorder(EjBorderStyle *border)
{
    m_topBorder = border;
}

void EjCellStyle::setRightBorder(EjBorderStyle *border)
{
    m_rightBorder = border;
}

void EjCellStyle::setBottomBorder(EjBorderStyle *border)
{
    m_bottomBorder = border;
}

void EjCellStyle::setLeftBorder(EjBorderStyle *border)
{
    m_leftBorder = border;
}

void EjCellStyle::setBorderStyle(EjBorderStyle *borderStyle)
{
    borderStyle->m_doc = m_doc;
    borderStyle->createDefault(m_doc->lPropBlocks,m_index + m_counts);
    borderStyle->setPenColor(borderStyle->m_penColor);
    borderStyle->setWidth(borderStyle->m_width);
    m_counts += borderStyle->m_counts + 1;

}

void EjCellStyle::setBrushColor(QColor brushColor)
{
    if (m_brushColor == brushColor)
        return;
    if(m_doc)
    {
		EjPropColorBlock *propColor = (EjPropColorBlock*)findProp(m_doc->lPropBlocks, PROP_COLOR, CELL_BRUSH_COLOR);
        if(!propColor)
        {
			propColor = new EjPropColorBlock(CELL_BRUSH_COLOR);
            addProp(m_doc->lPropBlocks,propColor);
        }
        propColor->color = brushColor;
    }
    m_brushColor = brushColor;
    emit brushColorChanged();
}

void EjCellStyle::setParentStyle(EjCellStyle *parentStyle)
{
    if (!m_doc || m_parentStyle)
        return;

	EjPropIntBlock *propInt = (EjPropIntBlock*)findProp(m_doc->lPropBlocks, PROP_INT, STYLE_PARENT_NUM);
    if(!propInt)
    {
		propInt = new EjPropIntBlock(STYLE_PARENT_NUM);
        addProp(m_doc->lPropBlocks,propInt);
    }
    propInt->value = m_doc->lStyles->indexOf(parentStyle);

    m_parentStyle = parentStyle;

    if(!m_topBorder->fullCompare(parentStyle->m_topBorder))
    {
        m_topBorder = new EjBorderStyle(CELL_TOP);
        *m_topBorder = m_topBorder_st;
        setBorderStyle(m_topBorder);
    }
    m_topBorder->m_doc = m_doc;

    if(!m_bottomBorder->fullCompare(parentStyle->m_bottomBorder))
    {
        m_bottomBorder = new EjBorderStyle(CELL_BOTTOM);
        *m_bottomBorder = m_bottomBorder_st;
        setBorderStyle(m_bottomBorder);
    }
    m_bottomBorder->m_doc = m_doc;

    if(!m_leftBorder->fullCompare(parentStyle->m_leftBorder))
    {
        m_leftBorder = new EjBorderStyle(CELL_LEFT);
        *m_leftBorder = m_leftBorder_st;
        setBorderStyle(m_leftBorder);
    }
    m_leftBorder->m_doc = m_doc;

    if(!m_rightBorder->fullCompare(parentStyle->m_rightBorder))
    {
        m_rightBorder = new EjBorderStyle(CELL_RIGHT);
        *m_rightBorder = m_rightBorder_st;
        setBorderStyle(m_rightBorder);
    }
    m_rightBorder->m_doc = m_doc;

    if(m_brushColor != parentStyle->m_brushColor)
    {
		EjPropColorBlock  *propColor = new EjPropColorBlock(CELL_BRUSH_COLOR);
        propColor->color = m_brushColor;
        addProp(m_doc->lPropBlocks,propColor);
    }

}
