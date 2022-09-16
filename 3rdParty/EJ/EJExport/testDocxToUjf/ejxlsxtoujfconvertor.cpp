#include "ejxlsxtoujfconvertor.h"

EjXlsxToUjfConvertor::EjXlsxToUjfConvertor():
    temp_folder(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + '/'), subfolder("ujf_test/")
{
    editors.insert("fonts", new FontsStyleDocumentReader());
    editors.insert("fills", new FillsStyleDocumentReader());
    editors.insert("borders", new BordersStyleDocumentReader());
    editors.insert("cellXfs", new CellXfsStyleDocumentReader());

    editorsStrings.insert("si", new TagSiExcelDocumentReader());
    editorsStrings.insert("r", new TagRExcelDocumentReader());
    editorsStrings.insert("rPr", new TagRprExcelDocumentReader());
    editorsStrings.insert("t", new TagTExcelDocumentReader());

    editorsSheet.insert("dimension", new DimensionExcelDocumentReader());
    editorsSheet.insert("sheetData", new SheetDataExcelDocumentReader());
    editorsSheet.insert("row", new RowExcelDocumentReader());
    editorsSheet.insert("c", new CellExcelDocumentReader());
    editorsSheet.insert("v", new VExcelDocumentReader());
    editorsSheet.insert("mergeCells", new MergeCellsExcelDocumentReader());
}

bool EjXlsxToUjfConvertor::readDoc(EjDocument *doc, QString fileName)
{
    bool result = true;
    QString newName = fileName.left(fileName.indexOf('.')) + ".zip";
    QFile file(fileName);
    file.copy(newName);
    zipper.unzip(temp_folder + subfolder, newName);
    result &= readStylesDocument(temp_folder + subfolder + "/xl/styles.xml");
    result &= readStringsDocument(temp_folder + subfolder + "/xl/sharedStrings.xml");
    result &= readSheetDocument(doc, temp_folder + subfolder + "/xl/worksheets/sheet1.xml");
    file.close();
    return result;
}

bool EjXlsxToUjfConvertor::readStylesDocument(QString fileName)
{
    qDebug() <<  "READ STYLES XLSX DOC";
    QFile xmlfile(fileName);
    if(!xmlfile.exists()){
        return false;
    }
    QDomDocument dom;
    dom.setContent(&xmlfile);
    if(dom.isNull()){
        return false;
    }
    bool result = true;
    qDebug() <<  "----------";
      QDomNode node = dom.documentElement().firstChild();
      while (!node.isNull()) {
         qDebug() << node.toElement().tagName();
         StylesExcelDocumentReader *editor = editors.value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->read(stylesDocument, node);
         }
         node = node.nextSibling();
      }
      xmlfile.close();
    return result;
}

bool EjXlsxToUjfConvertor::readStringsDocument(QString fileName)
{
    qDebug() <<  "READ STRINGS XLSX DOC";
    QFile xmlfile(fileName);
    if(!xmlfile.exists()){
        return false;
    }
    QDomDocument dom;
    dom.setContent(&xmlfile);
    if(dom.isNull()){
        return false;
    }
    bool result = true;
      QDomNode node = dom.documentElement().firstChild();
      while (!node.isNull()) {
         qDebug() << node.toElement().tagName();
         StringsExcelDocumentReader *editor = editorsStrings.value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->read(sharedStrings, node, &editorsStrings);
         }
         node = node.nextSibling();
      }
      xmlfile.close();
    return result;
}

bool EjXlsxToUjfConvertor::readSheetDocument(EjDocument *doc, QString fileName)
{
    qDebug() <<  "READ SHEET XLSX DOC";
    QFile xmlfile(fileName);
    if(!xmlfile.exists()){
        return false;
    }
    QDomDocument dom;
    dom.setContent(&xmlfile);
    if(dom.isNull()){
        return false;
    }
    document.controlSetDocument(doc);
    bool result = true;
     QDomNode node = dom.documentElement().firstChild();
      while (!node.isNull()) {
         qDebug() << node.toElement().tagName();
         SheetExcelDocumentReader *editor = editorsSheet.value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->read(document, stylesDocument, sharedStrings, node, &editorsSheet);
         }
         node = node.nextSibling();
      }

    xmlfile.close();
    return result;
}
