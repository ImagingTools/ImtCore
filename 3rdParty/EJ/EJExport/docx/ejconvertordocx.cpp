#include "ejconvertordocx.h"

#include "imageblock.h"
#include "ejtableblocks.h"
#include "export_global.h"
#include "ejstoragehelper.h"

const QString EjConvertorDocx::format = "docx";

bool EjConvertorDocx::unzip() const {
	return zipper.unzip(temp_folder + subfolder, ":/formats/" + format + ".zip");
}

bool EjConvertorDocx::zip(QString const& file_name) const {
    qDebug() << "FileName to ZIP: " << file_name;
    return zipper.toZip(temp_folder + subfolder, file_name);
}

bool EjConvertorDocx::readDoc(EjDocument *doc) {
    document.setPageSettings(int(doc->attributes()->getDocLayout()->docWidth() * CM_TO_TWIP),
                                  int(doc->attributes()->getDocLayout()->docHeight() * CM_TO_TWIP),
                                  int(doc->attributes()->getDocMargings()->left() * CM_TO_TWIP),
                                  int(doc->attributes()->getDocMargings()->right() * CM_TO_TWIP),
                                  int(doc->attributes()->getDocMargings()->top() * CM_TO_TWIP),
                                  int(doc->attributes()->getDocMargings()->bottom() * CM_TO_TWIP),
                                  0, 0, 0);
    document.addOrientation(doc->attributes()->getDocLayout()->docOrientation());
    bool result = true;
	const QList<EjBlock*> &list = *doc->lBlocks;
    qDebug() << "Блоки:";
    for (int i = 0; i < list.size() && result; ++i) {
		EjBlock* block = list[i];
        qDebug() << block->type;
    }
    qDebug() << "-----";

    for (int i = 0; i < list.size() && result; ++i) {
		EjBlock* block = list[i];
        EjWordDocumentWriter *editor = editors.value(block->type, nullptr);
        if (editor != nullptr) {
            result = editor->edit(document, block);
        } else {
            qDebug() << block->type;
        }
    }
    document.finish();
    return result;
}

bool EjConvertorDocx::write() {
    QFile file_document(temp_folder + subfolder + "/word/document.xml");
    bool result = file_document.open(QIODevice::WriteOnly);
    if (result) {
        result = file_document.write(document.getDocument().toByteArray()) != -1;
        file_document.close();
    }
    QFile file_rels(temp_folder + subfolder + "/word/_rels/document.xml.rels");
    if ((result &= file_rels.open(QIODevice::WriteOnly))) {
        result = file_rels.write(document.getRels().toByteArray()) != -1;
    }
    QList<QString> names_file = document.getRelations();
    const QString path = EjStorageHelper::pathPic();
    QString newPath = temp_folder + subfolder + "/word/media/";
    for (int i = 0; i < names_file.size() && result; ++i) {
    }
    return result;
}

void EjConvertorDocx::clear() {
    QDir(temp_folder + subfolder).removeRecursively();
    document.reset();
}

EjConvertorDocx::EjConvertorDocx():
    temp_folder(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + '/'), subfolder(format + "_test/") {
    editors.insert(e_typeBlocks::TEXT, new TextWriterWord());
    editors.insert(e_typeBlocks::SPACE, new SpaceWriterWord());
    editors.insert(e_typeBlocks::ENTER, new EnterWriterWord());
    editors.insert(e_typeBlocks::EXT_TABLE, new TableWriterWord());
    editors.insert(e_typeBlocks::BASECELL, new CellWriterWord());
    editors.insert(e_typeBlocks::EXT_IMAGE, new ImageWriterWord());
    editors.insert(e_typeBlocks::PROP_BA, new ByteArrayWriterWord());
    editors.insert(e_typeBlocks::END_GROUP, new EndGroupWriterWord());
    editors.insert(e_typeBlocks::NUM_STYLE, new StyleWriterWord());
}

bool EjConvertorDocx::convert(EjDocument *doc, QString const& file_name) {
    bool result = unzip();
    result &= readDoc(doc);
    result &= write();
    result &= zip(file_name);
    clear();
    return result;
}

EjConvertorDocx::~EjConvertorDocx() {
    for (QMap<quint8, EjWordDocumentWriter*>::iterator i = editors.begin(); i != editors.end(); ++i) {
        delete *i;
    }
}
