// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>
#include <imtfile/IFileDownloadLinkProvider.h>
#include <imtservergql/CFileSystemControllerComp.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>


class CMockFileSystemProvider: virtual public imtfile::IFileSystemStructureProvider
{
public:
	bool failNext = false;
	QString failMessage = QStringLiteral("mock failure");
	FileSystemListing nextListing;
	mutable QString lastPath;

	virtual bool GetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const override
	{
		lastPath = query.path;
		if (failNext){
			error = failMessage;
			result = FileSystemListing();

			return false;
		}

		result = nextListing;

		return true;
	}

	virtual bool Stat(
				const QString& /*path*/,
				FileSystemEntry& /*entry*/,
				QString& error) const override
	{
		error = QStringLiteral("not implemented in mock");

		return false;
	}
};


class CMockDownloadLinkProvider: virtual public imtfile::IFileDownloadLinkProvider
{
public:
	bool failNext = false;
	QString failMessage = QStringLiteral("mock failure");
	DownloadLink nextLink;
	mutable QString lastPath;

	virtual bool GetDownloadLink(
				const QString& path,
				DownloadLink& link,
				QString& error) const override
	{
		lastPath = path;
		if (failNext){
			error = failMessage;

			return false;
		}

		link = nextLink;

		return true;
	}
};


class CTestableFileSystemController: public imtservergql::CFileSystemControllerComp
{
public:
	void SetProvider(imtfile::IFileSystemStructureProvider* providerPtr)
	{
		m_providerPtr = providerPtr;
	}

	void SetDownloadLinkProvider(imtfile::IFileDownloadLinkProvider* providerPtr)
	{
		m_downloadLinkProviderPtr = providerPtr;
	}

	void SetPermissionsAllowed(bool allowed)
	{
		m_permissionsAllowed = allowed;
	}

	// Expose the command handlers for direct unit testing
	using BaseClass::OnGetFileSystemEntries;
	using BaseClass::OnGetDownloadLink;

	// reimplemented (CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(
				const imtgql::CGqlRequest& /*gqlRequest*/,
				QString& /*errorMessage*/) const override
	{
		return m_permissionsAllowed;
	}

protected:
	virtual imtfile::IFileSystemStructureProvider* GetFileSystemProvider() const override
	{
		return m_providerPtr;
	}

	virtual imtfile::IFileDownloadLinkProvider* GetDownloadLinkProvider() const override
	{
		return m_downloadLinkProviderPtr;
	}

private:
	imtfile::IFileSystemStructureProvider* m_providerPtr = nullptr;
	imtfile::IFileDownloadLinkProvider* m_downloadLinkProviderPtr = nullptr;
	bool m_permissionsAllowed = true;
};


class CFileSystemControllerTest: public QObject
{
	Q_OBJECT

private slots:
	void MissingProviderFailsTest();
	void ProviderErrorIsPropagatedTest();
	void HappyPathMapsPayloadTest();
	void LargeSizesAreNotTruncatedTest();
	void PermissionDeniedCreateResponseTest();
	void DownloadLinkWithoutProviderIsRefusedTest();
	void DownloadLinkMapsAllFieldsTest();
	void DownloadLinkOmitsUnknownFieldsTest();
	void DownloadLinkProviderErrorIsPropagatedTest();
	void DownloadLinkFailureWithoutMessageIsAnErrorTest();
};
