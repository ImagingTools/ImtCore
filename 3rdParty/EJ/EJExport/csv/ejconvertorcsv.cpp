#include "ejconvertorcsv.h"
#include "imageblock.h"
#include "ejtableblocks.h"
#include "export_global.h"
#include "ejstoragehelper.h"

const QString EjConvertorCSV::format = "csv";

EjConvertorCSV::EjConvertorCSV()
{
    editors.insert(e_typeBlocks::TEXT, new TextWriterCSV());
    editors.insert(e_typeBlocks::SPACE, new SpaceWriterCSV());
    editors.insert(e_typeBlocks::ENTER, new EnterWriterCSV());
    editors.insert(e_typeBlocks::EXT_TABLE, new TableWriterCSV());
    editors.insert(e_typeBlocks::BASECELL, new CellWriterCSV());
    editors.insert(e_typeBlocks::END_GROUP, new EndGroupWriterCSV());
}

EjConvertorCSV::~EjConvertorCSV() {
	for (QMap<quint8, EjCSVDocumentWriter*>::iterator i = editors.begin(); i != editors.end(); ++i) {
        delete *i;
    }
}

bool EjConvertorCSV::write(EjDocument *doc) {
   QList<EjBlock*> *list = doc->lBlocks;
   MaxIndexColumnCSV* maxindex = new MaxIndexColumnCSV();
   maxindex->edit(document, *list, 0);
   int i = 0;
   while (i < list->size()) {
	   EjBlock *block = list->at(i);
       qDebug() << block->type;
	   EjCSVDocumentWriter *editor = editors.value(block->type, nullptr);
       if (editor != nullptr) {
            i = editor->edit(document, *list, i);
       }
       i++;
   }

   //Нужно для того чтобы добавить текст который остался в конце,
    //Так как в текущей реализации текст добавляется только при конце строки
   //поэтому последняя строка текста не добавится без этого
   EndEditorCSV* end = new EndEditorCSV();
   end->edit(document, *list, 0);
   return true;
}

bool EjConvertorCSV::convert(EjDocument *doc, QString const& file_name){
    bool result = true;
    document.setFileName(file_name);
    result &= write(doc);
    return result;
}
