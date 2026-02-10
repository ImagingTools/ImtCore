// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtservergql
{


#define COLLECTION_DOCUMENT_MANAGER_DEFS(ns, DefsName) \
	struct DefsName \
	{ \
		typedef ns::CGetOpenedDocumentListGqlRequest GetOpenedDocumentListGqlRequest; \
		typedef ns::CCreateNewDocumentGqlRequest CreateNewDocumentGqlRequest; \
		typedef ns::COpenDocumentGqlRequest OpenDocumentGqlRequest; \
		typedef ns::CSaveDocumentGqlRequest SaveDocumentGqlRequest; \
		typedef ns::CCloseDocumentGqlRequest CloseDocumentGqlRequest; \
		typedef ns::CGetUndoInfoGqlRequest GetUndoInfoGqlRequest; \
		typedef ns::CDoUndoGqlRequest DoUndoGqlRequest; \
		typedef ns::CDoRedoGqlRequest DoRedoGqlRequest; \
		typedef ns::CResetUndoGqlRequest ResetUndoGqlRequest; \
	}

} // namespace imtservergql


