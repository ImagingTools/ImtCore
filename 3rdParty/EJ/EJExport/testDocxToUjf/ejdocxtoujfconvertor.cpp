#include "ejdocxtoujfconvertor.h"
#include <QPair>
#include <QStandardPaths>


EjDocxToUjfConvertor::EjDocxToUjfConvertor():
    temp_folder(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + '/'), subfolder("ujf_test/")
{
    editors.insert("w:p", new ParagraphDocumentReader());
    editors.insert("w:tbl", new TableDocumentReader());
    editors.insert("w:t", new TextDocumentReader());
    editors.insert("w:pPr", new pPrDocumentReader());
    editors.insert("w:r", new RDocumentReader());
    editors.insert("w:rPr", new rPrDocumentReader());
    editors.insert("w:tblPr", new tblPrDocumentReader());
    editors.insert("w:tblGrid", new tblGridDocumentReader());
    editors.insert("w:trPr", new trPrDocumentReader());
    editors.insert("w:tcPr", new tcPrDocumentReader());
    editors.insert("w:tr", new TableRowDocumentReader());
    editors.insert("w:tc", new TableCellDocumentReader());
    editors.insert("w:tab", new TabDocumentReader());
}

bool EjDocxToUjfConvertor::readDoc(EjDocument* doc, QString fileName)
{
    qDebug() <<  "READ DOC";
    QString newName = fileName.left(fileName.indexOf('.')) + ".zip";
    QFile file(fileName);
    file.copy(newName);
    zipper.unzip(temp_folder + subfolder, newName);
    QFile xmlfile(temp_folder + subfolder + "/word/document.xml");
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
//     qDebug() <<  "----------";
      QDomNode node = dom.documentElement().firstChildElement("w:body").firstChild();
      while (!node.isNull()) {
//         qDebug() << node.toElement().tagName();
         WordDocumentReader *editor = editors.value(node.toElement().tagName());
         if(editor != nullptr){
             result &= editor->edit(document, node, &editors);
         }
         node = node.nextSibling();
      }
      file.close();
      xmlfile.close();
      QDir dir(temp_folder + subfolder);
      QFile filezip(newName);
      result &= dir.removeRecursively();
      result &= filezip.remove();
      return result;
}


