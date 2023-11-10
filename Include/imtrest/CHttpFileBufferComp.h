#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionComp.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>


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
		I_ASSIGN(m_tempFileCollectionCompPtr, "ObjectCollection", "The object collection, used to store temp files paths", true, "ObjectCollection");
		I_ASSIGN(m_tempDirectoryPathCompPtr, "TempDirectoryPath", "The path where temp files will be stored, while it is required. \nWarning: this path will be cleared on suutdown", true, "TempDirectoryPath");
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



private:
	I_REF(imtbase::IObjectCollection, m_tempFileCollectionCompPtr);
	I_REF(ifile::IFileNameParam, m_tempDirectoryPathCompPtr);
};


} // namespace imtrest


