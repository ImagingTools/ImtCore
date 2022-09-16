#include "ejworddocument.h"

#include <algorithm>
#include "standart_function.h"

/* Этих цветов у меня нет.
    {220,20,60,"crimson"},
    {250,128,114, "salmon"},
    {255,99,71, "tomato"},
    {175,216,230, "lightBlue"},
    {255,255,204, "lightYellow"},
    {225,255,255, "lightCyan"},
    {106,90,205, "slateBlue"},
    {128,128,128, "gray"},
    {192,192,192, "silver"},
*/
const int EjWordDocument::shift_relation = 4;
const int  EjWordDocument::number_system_color = 16;
const QColor EjWordDocument::system_color[number_system_color] = {
    QColor(255, 255, 255),
    QColor(255, 255, 0),
    QColor(255, 0, 255),
    QColor(255, 0, 0),
    QColor(211, 211, 211),
    QColor(204, 204, 0),
    QColor(169, 169, 169),
    QColor(139, 0, 139),
    QColor(139, 0, 0),
    QColor(0, 255, 255),
    QColor(0, 255, 0),
    QColor(0, 139, 139),
    QColor(0, 100, 0),
    QColor(0, 0, 255),
    QColor(0, 0, 139),
    QColor(0, 0, 0),
};
const QString EjWordDocument::system_color_name[number_system_color] = {
    "white",
    "yellow",
    "magenta",
    "red",
    "lightGray",
    "darkYellow",
    "darkGray",
    "darkMagenta",
    "darkRed",
    "cyan",
    "green",
    "darkCyan",
    "darkGreen",
    "blue",
    "darkBlue",
    "black",
};

void EjWordDocument::removeElementList() {
    blocks.removeLast();
    types.removeLast();
}

void EjWordDocument::addElementList(TypeElement const& type, QDomElement const& element) {
    blocks.append(element);
    types.append(type);
}

void EjWordDocument::createSectPr() {
    QDomElement w_sectPr = document.createElement("w:sectPr");
    blocks[0].appendChild(w_sectPr);

    QDomElement w_type = document.createElement("w:type");
    w_type.setAttribute("w:val", "nextPage");
    w_sectPr.appendChild(w_type);

    QDomElement w_pgSz = document.createElement("w:pgSz");
    if(landscape){
         w_pgSz.setAttribute("w:orient", "landscape");
         w_pgSz.setAttribute("w:w", height);
         w_pgSz.setAttribute("w:h", width);
    }else{
        w_pgSz.setAttribute("w:w", width);
        w_pgSz.setAttribute("w:h", height);
    }
    w_sectPr.appendChild(w_pgSz);


    QDomElement w_PgMar = document.createElement("w:pgMar");
    w_PgMar.setAttribute("w:left", left);
    w_PgMar.setAttribute("w:right", right);
    w_PgMar.setAttribute("w:header", header);
    w_PgMar.setAttribute("w:top", top);
    w_PgMar.setAttribute("w:footer",footer);
    w_PgMar.setAttribute("w:bottom", bottom);
    w_PgMar.setAttribute("w:gutter", gutter);
    w_sectPr.appendChild(w_PgMar);

    QDomElement w_pgNumType = document.createElement("w:pgNumType");
    w_pgNumType.setAttribute("w:fmt", "decimal");
    w_sectPr.appendChild(w_pgNumType);

    QDomElement w_formProt = document.createElement("w:formProt");
    w_formProt.setAttribute("w:val", "false");
    w_sectPr.appendChild(w_formProt);

    QDomElement w_textDirection = document.createElement("w:textDirection");
    w_textDirection.setAttribute("w:val", "lrTb");
    w_sectPr.appendChild(w_textDirection);

    QDomElement w_docGrid = document.createElement("w:docGrid");
    w_docGrid.setAttribute("w:type", "default");
    w_docGrid.setAttribute("w:linePitch", "240");
    w_docGrid.setAttribute("w:charSpace", "4294961151");
    w_sectPr.appendChild(w_docGrid);
}

bool EjWordDocument::createParagraph() {
    TypeElement last_type = types[types.size() - 1];

    bool result = last_type == type_p;
    if (paragraph_parent.contains(last_type) && !result) {
        QDomElement element = document.createElement("w:p");
        blocks[types.size()-1].appendChild(element);
        addElementList(type_p, element);
        QDomElement w_pPr = document.createElement("w:pPr");
        QDomElement w_jc = document.createElement("w:jc");
        w_jc.setAttribute("w:val", align);
        w_pPr.appendChild(w_jc);
        element.appendChild(w_pPr);
        result = true;
    }
    return result;
}

bool EjWordDocument::addText(QString const& text) {
    bool result = createParagraph();
    if (result) {
        //qDebug() << "Текст: " << text << "  Текущий алигн: " << align;
        QDomElement w_r = document.createElement("w:r");
        blocks[blocks.size() - 1].appendChild(w_r);
        QDomElement w_rPr = document.createElement("w:rPr");
        w_r.appendChild(w_rPr);
        QDomElement w_color = document.createElement("w:color");
        w_rPr.appendChild(w_color);
        w_color.setAttribute("w:val", convertColor(color_text));
        if (highlight_color_exist) {
            QDomElement w_highlight = document.createElement("w:highlight");
            w_rPr.appendChild(w_highlight);
            w_highlight.setAttribute("w:val", highlight_color);
        }
        QDomElement w_t = document.createElement("w:t");
        w_r.appendChild(w_t);
        w_t.appendChild(document.createTextNode(text));
        w_t.setAttribute("xml:space", "preserve");

        QDomElement w_rFonts = document.createElement("w:rFonts");
        w_rPr.appendChild(w_rFonts);
        w_rFonts.setAttribute("w:ascii", "Times New Roman");
        w_rFonts.setAttribute("w:hAnsi", "Times New Roman");

        QDomElement w_sz = document.createElement("w:sz");
        w_sz.setAttribute("w:val", text_size);
        w_rPr.appendChild(w_sz);
        QDomElement w_szCs = document.createElement("w:szCs");
        w_szCs.setAttribute("w:val", text_size);
        w_rPr.appendChild(w_szCs);

        if (is_bold) {
            w_rPr.appendChild(document.createElement("w:b"));
            w_rPr.appendChild(document.createElement("w:bCs"));
        }

        if (is_italic) {
            w_rPr.appendChild(document.createElement("w:i"));
            w_rPr.appendChild(document.createElement("w:iCs"));
        }

        if (is_under_line) {
            QDomElement  w_u = document.createElement("w:u");
            w_u.setAttribute("w:val","single");
            w_rPr.appendChild(w_u);
        }

        if (is_strike_out) {
            w_rPr.appendChild(document.createElement("w:strike"));
        }
    }
    return result;
}

bool EjWordDocument::createTableLine() {
    while (types.size() > 1 && types[types.size()-1] != type_tbl) {
        removeElementList();
    }
    bool result = types.size() > 1;
    if (result) {
        QDomElement w_tr = document.createElement("w:tr");
        blocks[blocks.size()-1].appendChild(w_tr);
        addElementList(type_tr, w_tr);
    }
    return result;
}

QDomElement EjWordDocument::createBorrier(QString const& name, int width) {
    QDomElement borrier = document.createElement(name);
    borrier.setAttribute("w:val", "single");
    borrier.setAttribute("w:color", "000000");
    borrier.setAttribute("w:space", "0");
    int value = width / 25;
    borrier.setAttribute("w:sz", value);
    return borrier;
}

void EjWordDocument::initializationDocument() {
    document.clear();
    types.clear();
    blocks.clear();
    color_text = QColor(0,0,0);
    highlight_color_exist = is_bold = is_italic = is_under_line = is_strike_out = false;
    text_size = 24;
    setPageSettings();
    setAilgn();
    skiped_cell_i.clear();
    skiped_cell_j.clear();
    continue_cell_i.clear();
    continue_cell_j.clear();
    value_continue.clear();
    widths_tables.clear();
    heights_tables.clear();
    i.clear();
    j.clear();

    QDomElement w_document = document.createElement("w:document");
    document.appendChild(w_document);
    w_document.setAttribute("xmlns:ve", "http://schemas.openxmlformats.org/markup-compatibility/2006");
    w_document.setAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    w_document.setAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
    w_document.setAttribute("xmlns:m", "http://schemas.openxmlformats.org/officeDocument/2006/math");
    w_document.setAttribute("xmlns:v", "urn:schemas-microsoft-com:vml");
    w_document.setAttribute("xmlns:wp", "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing");
    w_document.setAttribute("xmlns:w10", "urn:schemas-microsoft-com:office:word");
    w_document.setAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
    w_document.setAttribute("xmlns:wne", "http://schemas.microsoft.com/office/word/2006/wordml");
    w_document.setAttribute("xmlns:wp14", "http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing");
    addElementList(type_body, document.createElement("w:body"));
    w_document.appendChild(blocks[0]);
}

void EjWordDocument::initializationRels() {
    rels.clear();
    relations.clear();
    rels_relationships = rels.createElement("Relationships");
    rels.appendChild(rels_relationships);
    rels_relationships.setAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");

    QDomElement relationship = rels.createElement("Relationship");
    relationship.setAttribute("Id", "rId1");
    relationship.setAttribute("Target", "styles.xml");
    relationship.setAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles");
    rels_relationships.appendChild(relationship);

    relationship = rels.createElement("Relationship");
    relationship.setAttribute("Id", "rId2");
    relationship.setAttribute("Target", "fontTable.xml");
    relationship.setAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/fontTable");
    rels_relationships.appendChild(relationship);

    relationship = rels.createElement("Relationship");
    relationship.setAttribute("Id", "rId3");
    relationship.setAttribute("Target", "settings.xml");
    relationship.setAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings");
    rels_relationships.appendChild(relationship);
}

void EjWordDocument::addOrientation(bool value)
{
   landscape = value;
}

EjWordDocument::EjWordDocument() {
    aligns.insert(EjParagraphStyle::AlignLeft, "left");
    aligns.insert(EjParagraphStyle::AlignRight, "right");
    aligns.insert(EjParagraphStyle::AlignHCenter, "center");

    vAligns.insert(EjParagraphStyle::AlignTop, "top");
    vAligns.insert(EjParagraphStyle::AlignBottom, "bottom");
    vAligns.insert(EjParagraphStyle::AlignVCenter, "center");

    paragraph_parent.insert(type_body);
    paragraph_parent.insert(type_tc);

    groups.insert(type_tbl);
    groups.insert(type_pict);

    reset();
}

void EjWordDocument::reset() {
    initializationDocument();
    initializationRels();
}

void EjWordDocument::setPageSettings(int new_width, int new_height, int new_left, int new_right,
                                   int new_top,int new_bottom, int new_header, int new_footer, int new_gutter) {
    width = new_width;
    height = new_height;
    left = new_left;
    right = new_right;
    top = new_top;
    bottom = new_bottom;
    header = new_header;
    footer = new_footer;
    gutter = new_gutter;
}

bool EjWordDocument::setTextSize(int value) {
    bool result = value > 0;
    if (result) {
        text_size = value;
    }
    return result;
}

bool EjWordDocument::setAilgn(int style) {

    switch(style)
    {
        case 1:
            align = "left";
            vAlign = "";
        break;

        case 2:
            align = "right";
            vAlign = "";
        break;

        case 4:
            align = "center";
            vAlign = "";
        break;

        case 33:
            align = "left";
            vAlign = "top";
        break;

        case 36:
            align = "center";
            vAlign = "top";
        break;

        case 34:
            align = "right";
            vAlign = "top";
        break;

        case 129:
            align = "left";
            vAlign = "center";
        break;

        case 132:
            align = "center";
            vAlign = "center";
        break;

        case 130:
            align = "right";
            vAlign = "center";
        break;

        case 65:
            align = "left";
            vAlign = "bottom";
        break;

        case 68:
            align = "center";
            vAlign = "bottom";
        break;

        case 66:
            align = "right";
            vAlign = "bottom";
        break;
    }
    return true;
}

bool EjWordDocument::setColorText(QColor const& color) {
    color_text = color;
    return true;
}

bool EjWordDocument::setHighlightColor(QColor const &color) {
    if ((highlight_color_exist = color.alpha() > 0)) {
        highlight_color = convertColorName(color, number_system_color, system_color, system_color_name);
    }
    return true;
}

bool EjWordDocument::setBold(bool value) {
    is_bold = value;
    return true;
}

bool EjWordDocument::setItalic(bool value) {
    is_italic = value;
    return true;
}

bool EjWordDocument::setUnderLine(bool value) {
    is_under_line = value;
    return true;
}

bool EjWordDocument::setStrikeOut(bool value) {
    is_strike_out = value;
    return true;
}

bool EjWordDocument::addSpace() {
    return addText(" ");
}

bool EjWordDocument::endParagraph() {
    bool result = createParagraph();
    if (result) {
        removeElementList();
    }
    return result;
}

bool EjWordDocument::createTable(int width, int tbl_ind, QList<int> const& widths, int width_table, int) {
    QDomElement w_tbl = document.createElement("w:tbl");
    blocks[blocks.size() - 1].appendChild(w_tbl);
    addElementList(type_tbl, w_tbl);

    QDomElement w_tblPr = document.createElement("w:tblPr");
    w_tbl.appendChild(w_tblPr);

    QDomElement w_tblW = document.createElement("w:tblW");
    w_tblPr.appendChild(w_tblW);
    w_tblW.setAttribute("w:w", width);
    w_tblW.setAttribute("w:type", "auto");

    QDomElement w_tblStyle = document.createElement("w:tblStyle");
    w_tblPr.appendChild(w_tblStyle);
    w_tblStyle.setAttribute("w:val", "a3");

    QDomElement w_tblInd = document.createElement("w:tblInd");
    w_tblPr.appendChild(w_tblInd);
    w_tblInd.setAttribute("w:w", tbl_ind);
    w_tblInd.setAttribute("w:type","dxa");

    QDomElement w_tblGrid = document.createElement("w:tblGrid");
    w_tbl.appendChild(w_tblGrid);

    for (int i = 0; i < widths.size(); ++i) {
        QDomElement w_gridCol = document.createElement("w:gridCol");
        w_tblGrid.appendChild(w_gridCol);
        w_gridCol.setAttribute("w:w", widths[i]);
    }

    widths_tables.append(width_table);
    i.append(0);
    j.append(0);
    skiped_cell_i.append(QList<int>());
    skiped_cell_j.append(QList<int>());
    continue_cell_i.append(QList<int>());
    continue_cell_j.append(QList<int>());
    value_continue.append(QList<int>());

    return true;
}

bool EjWordDocument::addCell(int width, int merge_rows, int merge_colums, QColor const& background,
                           int top_barrier_width, int left_barrier_width, int right_barrier_width, int bottom_barrier_width, bool isEmpty) {
    QList<int>& current_skiped_cell_j = skiped_cell_j[skiped_cell_j.size()-1];
    QList<int>& current_skiped_cell_i = skiped_cell_i[skiped_cell_i.size()-1];

    QList<int>& current_continue_cell_j = continue_cell_j[continue_cell_j.size()-1];
    QList<int>& current_continue_cell_i = continue_cell_i[continue_cell_i.size()-1];
    QList<int>& current_value_continue = value_continue[value_continue.size()-1];

    int current_i = i[i.size() - 1];
    int current_j = j[j.size() - 1];

    bool flag = true;
    int index;
    for (index = 0; index < current_skiped_cell_j.size() && flag; ++index) {
        flag = !(current_skiped_cell_j[index] == current_j && current_skiped_cell_i[index] == current_i);
    }
    bool result = true;
    if (flag) {
        if (0 == current_j) {
            result = createTableLine();
        }
        while (result && types.size() > 1 && types[types.size()-1] != type_tr) {
            removeElementList();
        }
        result &= types.size() > 1;
        if (result) {
            QDomElement w_tc = document.createElement("w:tc");
            blocks[blocks.size()-1].appendChild(w_tc);
            addElementList(type_tc, w_tc);

            QDomElement w_tcPr = document.createElement("w:tcPr");
            w_tc.appendChild(w_tcPr);

            QDomElement w_tcW = document.createElement("w:tcW");
            w_tcPr.appendChild(w_tcW);

            w_tcW.setAttribute("w:w", width);
            w_tcW.setAttribute("w:type", "dxa");

            if (!vAlign.isEmpty())
            {
                QDomElement w_vAlign = document.createElement("w:vAlign");
                w_tcPr.appendChild(w_vAlign);
                w_vAlign.setAttribute("w:val", vAlign);
            }

            QDomElement w_tcBorders = document.createElement("w:tcBorders");
            w_tcPr.appendChild(w_tcBorders);

            w_tcBorders.appendChild(createBorrier("w:top", top_barrier_width));
            w_tcBorders.appendChild(createBorrier("w:left", left_barrier_width));
            w_tcBorders.appendChild(createBorrier("w:right", right_barrier_width));
            w_tcBorders.appendChild(createBorrier("w:bottom", bottom_barrier_width));

            for (index = 0; index < current_continue_cell_j.size() && flag; ++index) {
                flag = !(current_continue_cell_j[index] == current_j && current_continue_cell_i[index] == current_i);
            }
            if (flag) {

                QDomElement w_shd = document.createElement("w:shd");
                w_tcPr.appendChild(w_shd);
                QString fill = "auto";
                if (background.alpha() > 0) {
                    fill = convertColor(background);
                }
                w_shd.setAttribute("w:fill", fill);
                w_shd.setAttribute("w:val","clear");

                bool merge = false;
                if ((merge |= merge_colums > 1)) {
                    QDomElement w_gridSpan = document.createElement("w:gridSpan");
                    w_tcPr.appendChild(w_gridSpan);
                    w_gridSpan.setAttribute("w:val", merge_colums);
                }
                if ((merge |= merge_rows > 1)) {
                    QDomElement w_vMerge = document.createElement("w:vMerge");
                    w_tcPr.appendChild(w_vMerge);
                    w_vMerge.setAttribute("w:val", "restart");
                }
                for (int y = 0; y < merge_rows && merge; ++y) {
                    for (int x = 1; x < merge_colums; ++x) {
                       current_skiped_cell_j.append(current_j+x);
                       current_skiped_cell_i.append(current_i+y);
                    }
                }
                for (int y = 1; y < merge_rows && merge; ++y) {
                    current_continue_cell_j.append(current_j);
                    current_continue_cell_i.append(current_i+y);
                    current_value_continue.append(merge_colums);
                }
            } else {
                --index;
                current_continue_cell_j.removeAt(index);
                current_continue_cell_i.removeAt(index);
                merge_colums = current_value_continue[index];
                current_value_continue.removeAt(index);
                QDomElement w_vMerge = document.createElement("w:vMerge");
                w_tcPr.appendChild(w_vMerge);
                if (merge_colums > 1) {
                    QDomElement w_gridSpan = document.createElement("w:gridSpan");
                    w_tcPr.appendChild(w_gridSpan);
                    w_gridSpan.setAttribute("w:val", merge_colums);
                }
            }

            /*Если в клетке нет текста, то добавим ей новый параграф
                Если текст есть, то параграф добавится вместе с ним
                */
            if(isEmpty){
                result &= createParagraph();
            }

        }
    } else {
        --index;
        current_skiped_cell_j.removeAt(index);
        current_skiped_cell_i.removeAt(index);
    }

    if (++j[j.size() - 1] >= widths_tables[widths_tables.size()-1]) {
        j[j.size() - 1] = 0;
        ++i[i.size() - 1];
    }
    return result;
}

bool EjWordDocument::addPict(int width, int height) {
    bool result = createParagraph();
    if (result) {
        QDomElement w_r = document.createElement("w:r");
        blocks[blocks.size() - 1].appendChild(w_r);
        QDomElement w_pict = document.createElement("w:pict");
        w_r.appendChild(w_pict);
        QDomElement v_shape = document.createElement("v:shape");
        v_shape.setAttribute("style", "width:"+QString::number(width)+"pt;height:"+QString::number(height)+"pt");
        w_pict.appendChild(v_shape);
        addElementList(type_pict, v_shape);
    }
    return result;
}

bool EjWordDocument::addImage(QString const& name) {
    bool result = types.size() > 1 && types[types.size()-1] == type_pict;
    if (result) {
        if (!relations.contains(name)) {
            relations.insert(name, relations.size() + shift_relation);
            QDomElement relationship = rels.createElement("Relationship");
            relationship.setAttribute("Id", "rId" + QString::number(relations.size()+shift_relation-1));
            relationship.setAttribute("Target", "media/" + name + ".jpeg");
            relationship.setAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image");
            rels_relationships.appendChild(relationship);
        }
        QDomElement v_imagedata = rels.createElement("v:imagedata");
        v_imagedata.setAttribute("r:id", "rId" + QString::number(relations[name]));
        blocks[blocks.size()-1].appendChild(v_imagedata);
    }
    qDebug() << name;

    return true;
}

bool EjWordDocument::endGroup() {
    while (types.size() > 1 && !groups.contains(types[types.size()-1])) {
        removeElementList();
    }
    bool result = types.size() > 1;
    if (result) {
        if (types[types.size()-1] == type_tbl) {
            widths_tables.removeLast();
            i.removeLast();
            j.removeLast();
            skiped_cell_i.removeLast();
            skiped_cell_j.removeLast();
            continue_cell_i.removeLast();
            continue_cell_j.removeLast();
            value_continue.removeLast();
        }
        removeElementList();
    }
    return result;
}

void  EjWordDocument::finish() {
    createSectPr();
}

const QDomDocument& EjWordDocument::getDocument() const {
    return document;
}

const QDomDocument& EjWordDocument::getRels() const {
    return rels;
}

QList<QString> EjWordDocument::getRelations() const {
    return relations.keys();
}
