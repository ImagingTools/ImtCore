#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imttest
{


//class ITestInfo;


/**
	Common interface for a test instance. A test instance is the result of the test installation by the customer.
	A test instance can have a subset of the defined test licenses and is therefore a provider of actually available licenses for the current instance.
	There is no license validation logic yet at this level. It is only an overview of the registered licenses in the specified test installation.
	\ingroup LicenseManagement
*/
class ITestInfo: virtual public iser::IObject
{
public:

	enum MetaInfoTypes
	{
		/**
			ID given as QByteArray.
		*/
		MIT_TEST_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Name given as QString.
		*/
		MIT_TEST_NAME,

		/**
			Description given as QString.
		*/
		MIT_TEST_DESCRIPTION
	};

	/**
		Get id of the test.
	*/
	virtual QByteArray GetTestId() const = 0;

	/**
		Get name of the test.
	*/
	virtual QString GetTestName() const = 0;

	/**
		Get description of the test.
	*/
	virtual QString GetTestDescription() const = 0;

	/**
		Set id of the test.
	*/
	virtual void SetTestId(QByteArray testId) = 0;

	/**
		Set name of the test.
	*/
	virtual void SetTestName(QString testName) = 0;

	/**
		Set description of the test.
	*/
	virtual void SetTestDescription(QString testDescription) = 0;
};


} // namespace imttest


