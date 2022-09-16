#include "ejstylesdocument.h"

QDomDocument EjStylesDocument::getDocument() const
{
    return document;
}

void EjStylesDocument::setAlign(int value)
{
    align = value;
}

void EjStylesDocument::aligns_init()
{

    QDomElement align1 = document.createElement("alignment");
    align1.setAttribute("horizontal", "center");
    aligns.insert(EjParagraphStyle::AlignHCenter, align1);

    QDomElement align2 = document.createElement("alignment");
    align2.setAttribute("horizontal", "left");
    aligns.insert(EjParagraphStyle::EjParagraphStyle::AlignLeft, align2);

    QDomElement align3 = document.createElement("alignment");
    align3.setAttribute("horizontal", "right");
    aligns.insert(EjParagraphStyle::EjParagraphStyle::AlignRight, align3);

    QDomElement align4 = document.createElement("alignment");
    align4.setAttribute("horizontal", "center");
    align4.setAttribute("vertical", "center");
    aligns.insert(132, align4);

    QDomElement align5 = document.createElement("alignment");
    align5.setAttribute("horizontal", "center");
    align5.setAttribute("vertical", "top");
    aligns.insert(36, align5);

    QDomElement align6 = document.createElement("alignment");
    align6.setAttribute("horizontal", "center");
    align6.setAttribute("vertical", "bottom");
    aligns.insert(68, align6);

    QDomElement align7 = document.createElement("alignment");
    align7.setAttribute("horizontal", "left");
    align7.setAttribute("vertical", "center");
    aligns.insert(129, align7);

    QDomElement align8 = document.createElement("alignment");
    align8.setAttribute("horizontal", "left");
    align8.setAttribute("vertical", "top");
    aligns.insert(33, align8);

    QDomElement align9 = document.createElement("alignment");
    align9.setAttribute("horizontal", "left");
    align9.setAttribute("vertical", "bottom");
    aligns.insert(65, align9);

    QDomElement align10 = document.createElement("alignment");
    align10.setAttribute("horizontal", "right");
    align10.setAttribute("vertical", "bottom");
    aligns.insert(66, align10);

    QDomElement align11 = document.createElement("alignment");
    align11.setAttribute("horizontal", "right");
    align11.setAttribute("vertical", "center");
    aligns.insert(130, align11);

    QDomElement align12 = document.createElement("alignment");
    align12.setAttribute("horizontal", "right");
    align12.setAttribute("vertical", "top");
    aligns.insert(34, align12);

    QDomElement align13 = document.createElement("alignment");
    align13.setAttribute("vertical", "bottom");
    aligns.insert(EjParagraphStyle::AlignBottom, align13);

    QDomElement align14 = document.createElement("alignment");
    align14.setAttribute("vertical", "center");
    aligns.insert(EjParagraphStyle::AlignVCenter, align14);

    QDomElement align15 = document.createElement("alignment");
    align15.setAttribute("vertical", "top");
    aligns.insert(EjParagraphStyle::AlignTop, align15);
}

int EjStylesDocument::addBorders(int left, int right, int top, int bottom)
{
    QDomElement border = document.createElement("border");
    borders.appendChild(border);
    QDomElement l = document.createElement("left");
    border.appendChild(l);
    QDomElement r = document.createElement("right");
    border.appendChild(r);
    QDomElement t = document.createElement("top");
    border.appendChild(t);
    QDomElement b = document.createElement("bottom");
    border.appendChild(b);

    /*Пока нигде не использовалась*/
    QDomElement d = document.createElement("diagonal");
    border.appendChild(d);
    QString value;

    if((value = widthsBorders.value(left)) != nullptr){
        l.setAttribute("style", value);

       /*Пока что цвет только черный, в дальнейшем возможны изменения*/
        QDomElement color = document.createElement("color");
        color.setAttribute("auto", "1");
        l.appendChild(color);
    }
    if((value = widthsBorders.value(right)) != nullptr){
        r.setAttribute("style", value);
        QDomElement color = document.createElement("color");
        color.setAttribute("auto", "1");
        r.appendChild(color);
    }
    if((value = widthsBorders.value(top)) != nullptr){
        t.setAttribute("style", value);
        QDomElement color = document.createElement("color");
        color.setAttribute("auto", "1");
        t.appendChild(color);
    }
    if((value = widthsBorders.value(bottom)) != nullptr){
        b.setAttribute("style", value);
        QDomElement color = document.createElement("color");
        color.setAttribute("auto", "1");
        b.appendChild(color);
    }

    countBorders++;
    return countBorders;
}

int EjStylesDocument::addFill(QString color)
{
    QDomElement fill = document.createElement("fill");
    fills.appendChild(fill);

    QDomElement pf = document.createElement("patternFill");
    pf.setAttribute("patternType", "solid");
    fill.appendChild(pf);
    QDomElement fgColor = document.createElement("fgColor");
    fgColor.setAttribute("rgb", color);
    pf.appendChild(fgColor);
    QDomElement bgColor = document.createElement("bgColor");
    bgColor.setAttribute("indexed", "64");
    pf.appendChild(bgColor);
    countFills++;
    return countFills;
}

int EjStylesDocument::addXfToCellXfs(int fillId, int borderId)
{
     QDomElement xf = document.createElement("xf");
     xf.setAttribute("numFmtId", 0);
     xf.setAttribute("fontId", 0);
     xf.setAttribute("fillId", fillId);
     if(fillId != 0){
         xf.setAttribute("applyFill", 1);
     }

     xf.setAttribute("borderId", borderId);
     if(borderId != 0){
         xf.setAttribute("applyBorder", 1);
     }
     xf.setAttribute("xfId", 0);


     QDomElement alignElem = aligns.value(align);
     if(!alignElem.isNull()){
         QDomElement newAlign = document.createElement(alignElem.tagName());
         xf.appendChild(newAlign);
         auto var = alignElem.attributes();
         for ( int i = 0; i < var.count(); i++)
         {
             QDomAttr attribute = var.item(i).toAttr();
             newAlign.setAttribute(attribute.name(),attribute.value());
         }
         newAlign.setAttribute("wrapText", "1");
         xf.setAttribute("applyAlignment", 1);
     }


     cellXfs.appendChild(xf);
     countCellXfs++;
     return countCellXfs;
}

EjStylesDocument::EjStylesDocument()
{
    QDomElement styleSheet = document.createElement("styleSheet");
    document.appendChild(styleSheet);
    styleSheet.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    fonts = document.createElement("fonts");
    fills = document.createElement("fills");
    borders = document.createElement("borders");
    cellStyleXfs = document.createElement("cellStyleXfs");
    cellXfs = document.createElement("cellXfs");
    cellStyles = document.createElement("cellStyles");
    dxfs = document.createElement("dxfs");
    tableStyles = document.createElement("tableStyles");
    styleSheet.appendChild(fonts);
    styleSheet.appendChild(fills);
    styleSheet.appendChild(borders);
    styleSheet.appendChild(cellStyleXfs);
    styleSheet.appendChild(cellXfs);
    styleSheet.appendChild(cellStyles);
    styleSheet.appendChild(dxfs);
    styleSheet.appendChild(tableStyles);

    QDomElement font = document.createElement("font");
    fonts.appendChild(font);
    QDomElement sz = document.createElement("sz");
    font.appendChild(sz);
    sz.setAttribute("val", "11");
    QDomElement color = document.createElement("color");
    font.appendChild(color);
    color.setAttribute("theme", "1");
    QDomElement name = document.createElement("name");
    font.appendChild(name);
    name.setAttribute("val", "Calibri");
    QDomElement family = document.createElement("family");
    font.appendChild(family);
    family.setAttribute("val", "2");
    QDomElement charset = document.createElement("charset");
    font.appendChild(charset);
    charset.setAttribute("val", "204");
    QDomElement scheme = document.createElement("scheme");
    font.appendChild(scheme);
    scheme.setAttribute("val", "minor");
 //---------------------------------------------------------
    QDomElement fill1 = document.createElement("fill");
    fills.appendChild(fill1);
    QDomElement patternFill = document.createElement("patternFill");
    fill1.appendChild(patternFill);
    patternFill.setAttribute("patternType", "none");
    QDomElement fill2 = document.createElement("fill");
    fills.appendChild(fill2);
    QDomElement patternFill2 = document.createElement("patternFill");
    fill2.appendChild(patternFill2);
    patternFill2.setAttribute("patternType", "gray125");
 //---------------------------------------------------------
    QDomElement border = document.createElement("border");
    borders.appendChild(border);
    QDomElement left = document.createElement("left");
    QDomElement right = document.createElement("right");
    QDomElement top = document.createElement("top");
    QDomElement bottom = document.createElement("bottom");
    QDomElement diagonal = document.createElement("diagonal");
    border.appendChild(left);
    border.appendChild(right);
    border.appendChild(top);
    border.appendChild(bottom);
    border.appendChild(diagonal);
  //---------------------------------------------------------
    QDomElement xf = document.createElement("xf");
    cellStyleXfs.appendChild(xf);
    xf.setAttribute("numFmtId", 0);
    xf.setAttribute("fontId", 0);
    xf.setAttribute("fillId", 0);
    xf.setAttribute("borderId", 0);
  //---------------------------------------------------------

    QDomElement xfFromCellXfs = document.createElement("xf");
    cellXfs.appendChild(xfFromCellXfs);
    xfFromCellXfs.setAttribute("numFmtId", 0);
    xfFromCellXfs.setAttribute("fontId", 0);
    xfFromCellXfs.setAttribute("fillId", 0);
    xfFromCellXfs.setAttribute("borderId", 0);
    xfFromCellXfs.setAttribute("xfId", 0);
  //---------------------------------------------------------
    QDomElement cellStyle = document.createElement("cellStyle");
    cellStyles.appendChild(cellStyle);
    cellStyle.setAttribute("builtinId", 0);
    cellStyle.setAttribute("xfId", 0);
    cellStyle.setAttribute("name", "Обычный");
  //---------------------------------------------------------
    tableStyles.setAttribute("defaultPivotStyle", "PivotStyleLight16");
    tableStyles.setAttribute("defaultTableStyle", "TableStyleMedium9");

    widthsBorders.insert(100, "thin");
    widthsBorders.insert(200, "medium");
    widthsBorders.insert(300, "thick");
    aligns_init();
}


