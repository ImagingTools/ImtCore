// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/CCollectionDocumentManager.h>


namespace imtdoc
{


class CCollectionDocumentManagerComp: public ilog::CLoggerComponentBase, public imtdoc::CCollectionDocumentManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imtdoc::CCollectionDocumentManager BaseClass2;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentManager)
		I_ASSIGN_MULTI_0(m_handlerCompPtr, "DocumentManagerEventHandler", "Document manager event handler", false);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIdList", "List of the object type-IDs according to the list of factories ('ObjectFactories')", true);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Factory of the undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_objectFactListCompPtr, "ObjectFactories", "List of object factories related to registered type-IDs ('ObjectTypeIdList')", true);
	I_END_COMPONENT

protected:
	// reimplemented (imtdoc::CCollectionDocumentManager)
	virtual QList<imtdoc::IDocumentManagerEventHandler*> GetDocumentManagerEventHandlers() const override;
	virtual imtbase::IObjectCollection* GetCollection() const override;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const override;

private:
	int GetObjectFactoryIndex(const QByteArray& typeId) const;

private:
	I_MULTIREF(imtdoc::IDocumentManagerEventHandler, m_handlerCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactListCompPtr);
};


} // namespace imtdoc


