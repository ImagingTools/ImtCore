#include "ejsharedstringsdocument.h"

int EjSharedStringsDocument::getCountWords() const
{
    return count_words;
}

QDomDocument EjSharedStringsDocument::getDocument() const
{
    return document;
}

void EjSharedStringsDocument::createSi() {
    /*Будет принята ранее созданная si,
    и создастся новая*/
    sst.appendChild(si);
    si = document.createElement("si");
}

void EjSharedStringsDocument::insertIntoSi(QString str)
{
    /*Для переданной строки будет создана такая структура
            <r>
                <rPr>
                     <sz val="16"/>
                     <color rgb="FFFF0000"/>
                     <rFont val="Calibri"/>
                     <family val="2"/>
                     <charset val="204"/>
                     <scheme val="minor"/>
                </rPr>
                <t>HELLO</t>
            </r>
*/
    QDomElement r = document.createElement("r");
    QDomElement rPr = document.createElement("rPr");
    if(bold){
        QDomElement b = document.createElement("b");
        rPr.appendChild(b);
    }

    if(italic){
        QDomElement i = document.createElement("i");
        rPr.appendChild(i);
    }

    if(underline){
        QDomElement u = document.createElement("u");
        rPr.appendChild(u);
    }

    if(strikeOut){
        QDomElement s = document.createElement("strike");
        rPr.appendChild(s);
    }

    QDomElement sz = document.createElement("sz");
    sz.setAttribute("val", sizeText);

    QDomElement color = document.createElement("color");
    if(str == " "){
        color.setAttribute("theme", 1);
    }else{
        color.setAttribute("rgb", colorText);
    }

    QDomElement rFont = document.createElement("rFont");
    rFont.setAttribute("val", nameFont);
    QDomElement family = document.createElement("family");
    family.setAttribute("val", 2);
    QDomElement charset = document.createElement("charset");
    charset.setAttribute("val", 204);
    QDomElement scheme = document.createElement("scheme");
    scheme.setAttribute("val", "minor");

    rPr.appendChild(sz);
    rPr.appendChild(color);
    rPr.appendChild(rFont);
    rPr.appendChild(family);
    rPr.appendChild(charset);
    rPr.appendChild(scheme);
    r.appendChild(rPr);
    QDomElement t = document.createElement("t");
    if(str == " "){
        t.setAttribute("xml:space", "preserve");
    }
    t.appendChild(document.createTextNode(str));
    r.appendChild(t);
    si.appendChild(r);
}

QString EjSharedStringsDocument::getTempTextFromCell() const
{
    return tempTextFromCell;
}

void EjSharedStringsDocument::setTempTextFromCell(const QString &value)
{
    tempTextFromCell = value;
}

void EjSharedStringsDocument::setSizeText(int value)
{
    sizeText = value;
}

void EjSharedStringsDocument::setBold(bool value)
{
    bold = value;
}

void EjSharedStringsDocument::setUnderline(bool value)
{
    underline = value;
}

void EjSharedStringsDocument::setItalic(bool value)
{
    italic = value;
}

void EjSharedStringsDocument::setStrikeOut(bool value)
{
    strikeOut = value;
}

void EjSharedStringsDocument::setColorText(const QString &value)
{
    colorText = value;
}

void EjSharedStringsDocument::setNameFont(const QString &value)
{
    nameFont = value;
}

void EjSharedStringsDocument::setStylesToDefault()
{
    nameFont = "Calibri";
    colorText = "FF000000";
    sizeText = 12;
    bold = false;
    underline = false;
    italic = false;
    strikeOut = false;
}

EjSharedStringsDocument::EjSharedStringsDocument() {
    sst = document.createElement("sst");
    si = document.createElement("si");
    document.appendChild(sst);
    sst.setAttribute("xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
}

bool EjSharedStringsDocument::addSpase() {
    return addText(" ", false);
}

bool EjSharedStringsDocument::addText(QString text, bool isNew) {
    /* Если isNew будет передан как true, то текущий тег si будет вставлен в документ
       Для текста вне таблицы ожидается, что isNew = true когда будет встречен блок
       переноса строки */
    if(isNew){
        createSi();
        count_words++;
        return true;
    }
    insertIntoSi(text);
    return true;
}

bool EjSharedStringsDocument::addTextIntoTable(QString new_text)
{
    insertIntoSi(new_text);
    count_words++;
    createSi();
    return true;
}
