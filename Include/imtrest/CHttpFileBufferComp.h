#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionComp.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtrepo/IFileObjectCollection.h>


namespace imtrest
{


/**
	Temporary storage for uploaded files to the server of files prepared to download to client
*/
class CHttpFileBufferComp: public CHttpServletCompBase
{


public:
	typedef CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileBufferComp);
		I_ASSIGN(m_tempFileCollectionCompPtr, "TempRequestCollection", "The object collection, used to store temp files paths", false, "TempRequestCollection");
		I_ASSIGN(m_tempDirectoryPathCompPtr, "TempDirectoryPath", "The path where temp files will be stored, while it is required. \nWarning: this path will be cleared on suutdown", true, "TempDirectoryPath");
		I_ASSIGN(m_fileObjectCollectionCompPtr, "FileCollection", "The file collection, used to direct access to files", false, "FileCollection");
	I_END_COMPONENT;


protected:
	ConstResponsePtr CreateDefaultErrorResponse(const QByteArray& errorString, imtrest::IProtocolEngine::StatusCode statusCode, const imtrest::CHttpRequest& request) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (CHttpServletCompBase)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

	virtual ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

	virtual ConstResponsePtr OnHead(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;



private:
	I_REF(imtbase::IObjectCollection, m_tempFileCollectionCompPtr);
	I_REF(ifile::IFileNameParam, m_tempDirectoryPathCompPtr);
	I_REF(imtrepo::IFileObjectCollection, m_fileObjectCollectionCompPtr);
};


} // namespace imtrest


