#include "ejstoragehelper.h"
#include "ejdocument.h"
#include <QIODevice>
#include <ejtableblocks.h>


EjBlock* newBlock(int tid)
{
	EjBlock *block = NULL;
	if(tid == -1 || tid == EXT_TABLE)
		block = new EjTableBlock(0);
	else if(tid == BASECELL)
		block = new EjCellBlock();
	return block;
}


template<>
struct COMMONSHARED_EXPORT StreamReader<EjBlock>
{
	static EjBlock* read(QDataStream &is)
	{
		quint8 tid;
		is >> tid;
		//        case EXT_MAP:  case EXT_DIAGRAM: case EXT_IMAGE: case EXT_TABLE:
		//        case BASECELL:
		if(tid == BASECELL || tid > PROP_BIG_TEXT)
		{

				EjBlock *obj = newBlock(tid);
				if(!obj)
					throw 1;
				obj->read(is);
				return obj;


		}
		switch (tid) {
		case BASE: case ENTER: case END_GROUP: {
				EjBlock *obj = new EjBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }
		case SPACE: {
				EjSpaceBlock *obj = new EjSpaceBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }
		case FRAGMENT: {
				EjFragmentBlock *obj = new EjFragmentBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }
		case NUM_STYLE: {
				EjNumStyleBlock *obj = new EjNumStyleBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }
		case TEXT: {
				EjTextBlock *obj = new EjTextBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }
		case PROP_DOC: {
				//            PropDoc *obj = new PropDoc();
				EjBlock *obj = nullptr;
				quint8 vid;
				is >> vid;
				if(vid == DOC_LINK)
					obj = new EjLinkProp();
				if(vid == DOC_ATTR)
					obj = new EjAttrProp();
				else if(vid == DOC_LAYOUT)
					obj = new EjDocLayout();
				else if(vid == DOC_MARGINGS)
					obj = new EjDocMargings();
				else if(vid == BASE_STYLE)
					obj = new EjBaseStyle();
				//            else if(vid == PAGE_STYLE)
				//                obj = new PageStyle();
				else if(vid == PARAGRAPH_STYLE)
					obj = new EjParagraphStyle();
				else if(vid == TEXT_STYLE)
					obj = new EjTextStyle();
				else if(vid == CELL_STYLE)
					obj = new EjCellStyle();
				else if(vid == BORDER_STYLE)
					obj = new EjBorderStyle();
				if(obj) {
					obj->type = tid;
					//                obj->read(is); ---- Block not read stream
				}
				return obj; }
			//        case EXT_TABLE: {
			//          TableBlockld *obj = neTableBlockld();
			//            obj->type = tid;
			//            obj->read(is);
			//            return obj; }
			//        case BASECELL: {
			//            BaseCellBlock *obj = new BaseCellBlock();
			//            obj->type = tid;
			//            obj->read(is);
			//            return obj; }
			//        case CHECK: {
			//            CheckBlock *obj = new CheckBlock();
			//            obj->type = tid;
			//            obj->read(is);
			//            return obj; }
		case VECTOR_POINT: {
				EjPointBlock *obj = new EjPointBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_KEY: {
				EjPropKeyBlock *obj = new EjPropKeyBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case MAP_LABEL: {
				EjMapLabelBlock *obj = new EjMapLabelBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_INT: {
				EjPropIntBlock *obj = new EjPropIntBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_INT8: {
				EjPropInt8Block *obj = new EjPropInt8Block();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_INT64: {
				EjPropInt64Block *obj = new EjPropInt64Block();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_BA: {
				EjPropByteArrayBlock *obj = new EjPropByteArrayBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_PNT: {
				EjPropPntBlock *obj = new EjPropPntBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_TXT: {
				EjPropTextBlock *obj = new EjPropTextBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_BIG_TEXT: {
				EjPropBigTextBlock *obj = new EjPropBigTextBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case PROP_COLOR: {
				EjPropColorBlock *obj = new EjPropColorBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		case ACCESS_RULES: {
				EjAccessBlock *obj = new EjAccessBlock();
				obj->type = tid;
				obj->read(is);
				return obj; }

		}
		//        return NULL;
		//        Block *obj = new Block();
		////        obj->type = tid;
		//        obj->type = ERROR_BLOCK;
		//        obj->read(is);
		throw 1;
		return NULL;
	}
};


class PatchDoc
{
public:
	PatchDoc(){
		m_patchKey = m_patchVer = m_patchTime = 0;
		m_prev = nullptr;
	}
	qint16 m_patchKey;
	quint16 m_patchVer;
	quint32 m_patchTime;
	QByteArray m_header;
	QByteArray m_body;
	QByteArray m_attributes;
	PatchDoc *m_prev;
	QString m_user;
	//    PatchDoc *getPatch(qint16 &key, quint16 &ver)
	//    {
	//        if(key == m_patchKey && ver == m_patchVer) {

	//            return this;
	//        }
	//        if(m_next) {
	//            return m_next->getPatch(key,ver);
	//        }
	//        else {
	//            return nullptr;
	//        }
	//    }
};


EjStorageHelper::EjStorageHelper()
{

}


EjStorageHelper *EjStorageHelper::getInstance()
{
    static EjStorageHelper instance;
    return &instance;
}


void EjStorageHelper::loadSmallImage(QImage *image, QByteArray &name, bool isSmall)
{

}


void EjStorageHelper::addImage(QString path, QByteArray &name)
{

}


void EjStorageHelper::addImage(QImage &image, QByteArray &name)
{

}


QString EjStorageHelper::pathPic()
{
	return QString();
}


QByteArray EjStorageHelper::getPatch(EjDocument* oldDoc, EjDocument* newDoc, quint16 newVer, QString user)
{
	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	if(!oldDoc || !newDoc || oldDoc->attributes()->key() != newDoc->attributes()->key())
	{
		return buffer;
	}
	//    Patch patch;
	EjAttrProp *oldAttr = oldDoc->attributes();
	//    AttrProp *newAttr = newDoc->getAttributes();
	QUuid uuid = QUuid::createUuid();
	qint16 key = uuid.data2;
	quint32 time = QDateTime::currentDateTime().toSecsSinceEpoch();
	QByteArray header;
	QByteArray body = diff(*oldDoc->lBlocks,*newDoc->lBlocks,false);
	QByteArray attributes;
	QList<EjBlock*> lEmpty;
	if(oldDoc->attributes()->m_patchVer == 0)
		attributes = diff(lEmpty,*newDoc->lPropBlocks,false);
	else
		attributes = diff(*oldDoc->lPropBlocks,*newDoc->lPropBlocks,false);
	quint8 countDop = 1;
	quint32 k = header.size();
	IntVarLen2 t(k);
	quint32 k1 = body.size();
	IntVarLen2 t1(k);
	quint32 k2 = attributes.size();
	IntVarLen2 t2(k2);
	bool bCompress = false;
	if(body.size() > 2 || attributes.size() > 2)
	{
		//        out.writeRawData("EJ1.0",5);
		if(body.size() < 3)
			body.clear();
		if(attributes.size() < 3)
			attributes.clear();
		out << oldAttr->m_patchKey << oldAttr->m_patchVer << key << newVer << time;
		writeBA(out,header);
		writeBA(out,body);
		writeBA(out,attributes);
		out << countDop;
		writeSmallString(out,user);
		if(buffer.size() > 100)
		{
			bCompress = true;
			buffer = qCompress(buffer);
		}
		buffer.prepend("EJ1.0_",6);
		buffer[5] = bCompress;
	}
	return buffer;
}


void EjStorageHelper::loadData(EjDocument* doc, QList<QByteArray*>& lData, bool isSaveHistory)
{
	qint16 oldPatchKey;
	quint16 oldPatchVer;
	QByteArray buffer;
	quint8 countDop = 0;
	PatchDoc *patchDoc;
	PatchDoc *headPatch = nullptr;
	PatchDoc *curPatch;
	PatchDoc *findPatch;
	PatchDoc *lastPatch = nullptr;
	bool bCompress;
	QList<PatchDoc*> *lPatches = new QList<PatchDoc*>;
	for(int i = 0; i < lData.count(); i++)
	{
		if(!lData.at(i))
			continue;
		buffer = *lData.at(i);
		if(buffer.size() < 17)
			continue;
		bCompress = buffer.at(5);
		buffer = buffer.right(buffer.size() - 6);
		if(bCompress)
			buffer = qUncompress(buffer);
		QDataStream in(&buffer, QIODevice::ReadOnly);
		//        in.readRawData(ver,5);
		patchDoc = new PatchDoc();
		in >> oldPatchKey >> oldPatchVer;
		in >> patchDoc->m_patchKey >> patchDoc->m_patchVer >> patchDoc->m_patchTime;
		readBA(in,patchDoc->m_header);
		readBA(in,patchDoc->m_body);
		readBA(in,patchDoc->m_attributes);
		in >> countDop;
		if(countDop == 1)
		{
			readSmallString(in,patchDoc->m_user);
		}

		//        in >> patchDoc->m_header >> patchDoc->m_body >> patchDoc->m_attributes >> countDop;
		lPatches->append(patchDoc);
		if(!headPatch) {
			headPatch = lastPatch = patchDoc;
		}
		else {
			findPatch = nullptr;
			for(int j = 0; j < lPatches->count(); j++)
			{
				curPatch = lPatches->at(j);
				if(curPatch->m_patchKey == oldPatchKey && curPatch->m_patchVer == oldPatchVer)
				{
					if(!findPatch)
						findPatch = curPatch;
					//                    if(curPatch && curPatch->m_patchTime < lPatches.at(j)->m_patchTime)
					if(findPatch->m_patchTime < curPatch->m_patchTime)
					{
						findPatch = curPatch;
					}
				}
			}
			if(findPatch) {
				patchDoc->m_prev = findPatch;
				if(lastPatch->m_patchVer < patchDoc->m_patchVer ||
					(lastPatch->m_patchVer == patchDoc->m_patchVer
					 && lastPatch->m_patchTime < patchDoc->m_patchTime) )
				{
					lastPatch = patchDoc;
				}
			}
			else {
				//                delete patchDoc;
				qDebug() << __FILE__ << __LINE__ << "Error Patch!!!";
			}

		}
	}

	doc->m_lPatches = lPatches;
	if(lastPatch)
		updateDoc(doc,lastPatch->m_patchVer, lastPatch->m_patchTime);

	if(!isSaveHistory)
	{
		qDeleteAll(*lPatches);
		lPatches->clear();
		doc->m_lPatches = nullptr;
	}
}


void EjStorageHelper::updateDoc(EjDocument* doc, quint16 patchVer, quint32 patchTime, bool isSetKey)
{
	PatchDoc *lastPatch = nullptr;
	if(doc && doc->m_lPatches)
	{
		foreach(PatchDoc *patch, *doc->m_lPatches)
		{
			if(patch->m_patchVer == patchVer && patch->m_patchTime == patchTime)
			{
				lastPatch = patch;
				break;
			}
		}
	}
	if(lastPatch)
	{
		QList<PatchDoc*>lFinish;
		QList<EjBlock*> src;
		QList<EjBlock*> patched;
		QList<EjBlock*> unused;
		QList<EjBlock*> src_prop;
		QList<EjBlock*> patched_prop;
		QList<EjBlock*> unused_prop;
		EjAttrProp *attr = nullptr;
		PatchDoc *curPatch;
		bool isError;
		QUuid uuid;


		curPatch = lastPatch;
		while(curPatch)
		{
			lFinish.prepend(curPatch);
			curPatch = curPatch->m_prev;
		}

		for(int i = 0; i < lFinish.count(); i++)
		{
			isError = false;
			curPatch = lFinish.at(i);

			try {
				//                patch(src_prop, curPatch->m_attributes);
				patched_prop  = patch2(src_prop, unused_prop, curPatch->m_attributes);
			}
			catch(...)
			{
				isError = true;
				qDebug() << "Error for load prop " << __FILE__ << __LINE__ ;
			}
			if(!isError)
				qDeleteAll(unused_prop);
			unused_prop.clear();
			src_prop = patched_prop;


			//            try {
			//                patched_prop  = patch(src_prop, curPatch->m_attributes);
			//            }
			//            catch(...)
			//            {
			//                isError = true;
			//                qDebug() << "Error for load prop " << __FILE__ << __LINE__ ;
			//            }

			//            unused_prop = (src_prop.toSet()-patched_prop.toSet()).toList();
			//            qDeleteAll(unused_prop);
			//            unused_prop.clear();
			//            src_prop = patched_prop;


			isError = false;
			try {

				patched  = patch2(src, unused, curPatch->m_body);
			}
			catch(...)
			{
				isError = true;
				qDebug() << "Error for load " << __FILE__ << __LINE__ ;

				//                break;
			}
			if(!isError)
				qDeleteAll(unused);
			unused.clear();
			src = patched;


			//            try {
			//                patched  = patch(src, curPatch->m_body);
			//            }
			//            catch(...)
			//            {
			//                isError = true;
			//                qDebug() << "Error for load data " << __FILE__ << __LINE__ ;
			//            }

			//            unused = (src.toSet()-patched.toSet()).toList();
			//            qDeleteAll(unused);
			//            unused.clear();
			//            src = patched;



		}
		qDeleteAll(*doc->lBlocks);
		doc->lBlocks->clear();
		*doc->lBlocks = src;

		if(doc->m_attrProp)
			uuid = doc->m_attrProp->keyUuid();
		//        qDeleteAll(*doc->lPropBlocks);
		//        doc->lPropBlocks->clear();
		//        doc->m_attrProp = nullptr;
		//        attr = doc->getAttributes();
		//        src_prop = *doc->lPropBlocks;

		qDeleteAll(*doc->lPropBlocks);
		doc->lPropBlocks->clear();
		*doc->lPropBlocks = src_prop;
		src_prop.clear();

		//        qDeleteAll(*doc->lStyles);
		doc->lStyles->clear();

		//        delete doc->m_attrProp;
		doc->m_attrProp = nullptr;
		//        if(doc->m_attrProp)
		//            doc->m_attrProp->m_counts = 0;

		//        doc->createDefaultTextStyle();
		//        doc->createDefaultPrgStyle();
		doc->calcProps();
		if(isSetKey)
		{
			attr = doc->attributes();
			attr->setKey(uuid);
			attr->m_patchKey = lastPatch->m_patchKey;
			attr->m_patchVer = lastPatch->m_patchVer;
			attr->m_patchTime = lastPatch->m_patchTime;
		}
	}

}

