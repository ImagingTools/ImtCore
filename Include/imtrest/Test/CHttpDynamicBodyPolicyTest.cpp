// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpDynamicBodyPolicyTest.h"


// STL includes
#include <limits>
#include <memory>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStorageInfo>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <ifile/CFileNameParamComp.h>
#include <ifile/CSystemLocationComp.h>

// ImtCore includes
#include <imtrest/CHttpDynamicBodyPolicyComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpTemporaryFileBodySink.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


typedef imtrest::IRequest IRequest;
typedef imtrest::IHttpRequestBodyPolicy IHttpRequestBodyPolicy;
typedef imtrest::CHttpDynamicBodyPolicyComp PolicyComp;
typedef icomp::TSimComponentWrap<PolicyComp> Policy;
typedef imtrest::CHttpTemporaryFileBodySink FileSink;


const int s_buffer = int(IHttpRequestBodyPolicy::BM_BUFFER);
const int s_stream = int(IHttpRequestBodyPolicy::BM_STREAM);
const int s_reject = int(IHttpRequestBodyPolicy::BM_REJECT);

const int s_payloadTooLarge = int(imtrest::IProtocolEngine::SC_PAYLOAD_TOO_LARGE);
const int s_internalServerError = int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR);

const qint64 s_mebibyte = qint64(1024) * 1024;
const qint64 s_defaultMemoryBodyLimit = 100 * s_mebibyte;

// The tests of the free space need a disk with some free space and a disk that is not endless
const qint64 s_minAvailableBytes = 16 * s_mebibyte;
const qint64 s_maxAvailableBytes = std::numeric_limits<qint64>::max() / 4;


class CRequestServletStub: virtual public imtrest::IRequestServlet
{
public:
	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& /*request*/, const QByteArray& /*subCommandId*/) const override
	{
		return {};
	}
};


/**
	Engine that creates the responses for the statuses of the refusals.
*/
class CEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	// reimplemented (imtrest::IProtocolEngine)
	QByteArray GetProtocolTypeId() const override
	{
		return QByteArrayLiteral("http");
	}

	const iser::IVersionInfo* GetProtocolVersion() const override
	{
		return nullptr;
	}

	bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override
	{
		switch (statusCode){
		case SC_PAYLOAD_TOO_LARGE:
			protocolStatusCode = 413;
			statusCodeLiteral = "Payload Too Large";
			return true;

		case SC_INTERNAL_SERVER_ERROR:
			protocolStatusCode = 500;
			statusCodeLiteral = "Internal Server Error";
			return true;

		default:
			return false;
		}
	}

	imtrest::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& /*requestHandler*/) const override
	{
		return nullptr;
	}

	imtrest::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& /*requestHandler*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}

	imtrest::IResponseUniquePtr CreateResponse(
				const imtrest::IRequest& /*request*/,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override
	{
		return new imtrest::CHttpResponse(statusCode, data, dataTypeId, *this);
	}
};


struct SPolicySettings
{
	int memoryBodyLimit = 100;
	int maxBodySize = 0;
	int reservePercent = 0;
	QString directory;

	// The reference to the directory is obligatory, a policy without it is a mistake of the settings
	bool isDirectoryMissing = false;
};


typedef icomp::TSimSharedComponentPtr<ifile::CFileNameParamComp> DirectoryParam;


/**
	The parameter of the directory as a component, it is what the container gives to the policy.
*/
DirectoryParam CreateDirectoryParam(const QString& path)
{
	DirectoryParam param;

	param->SetStringAttr("DefaultPath", path);
	param->SetIntAttr("PathType", ifile::IFileNameParam::PT_DIRECTORY);
	param->InitComponent();

	return param;
}


/**
	Create the policy as the container of the components does: the attributes and the references are set, then the component is created.
*/
std::unique_ptr<Policy> CreatePolicy(const SPolicySettings& settings)
{
	std::unique_ptr<Policy> policyPtr(new Policy);

	policyPtr->SetIntAttr("MemoryBodyLimit", settings.memoryBodyLimit);
	policyPtr->SetIntAttr("MaxBodySize", settings.maxBodySize);
	policyPtr->SetIntAttr("FreeSpaceReservePercent", settings.reservePercent);

	// The reference that is not set: ACF logs it (the critical message "Component BodyDirectory is defined, but definition component has no parent" of the test of the missing directory is expected)
	if (!settings.isDirectoryMissing){
		policyPtr->SetRef("BodyDirectory", CreateDirectoryParam(settings.directory));
	}

	policyPtr->InitComponent();

	return policyPtr;
}


/**
	Request with the given length of the body (empty - no Content-Length) and other headers, without the body.
*/
QByteArray CreateHead(const QByteArray& contentLength, const QByteArray& otherHeaders = QByteArray(), const QByteArray& method = "PUT")
{
	QByteArray retVal(method + " /api/upload?x=1 HTTP/1.1\r\nHost: localhost:7380\r\n");

	if (!contentLength.isEmpty()){
		retVal += "Content-Length: " + contentLength + "\r\n";
	}

	retVal += otherHeaders;
	retVal += "\r\n";

	return retVal;
}


const QByteArray s_chunkedHeader("Transfer-Encoding: chunked\r\n");

const QByteArray s_chunkedBody(
			"5\r\n"
			"hello\r\n"
			"6\r\n"
			" world\r\n"
			"0\r\n"
			"\r\n");


QByteArray CreateHeadOfSize(qint64 size)
{
	return CreateHead(QByteArray::number(size));
}


/**
	The decision of the policy about the request with the head. The head is parsed by a request without the policy.
*/
IHttpRequestBodyPolicy::BodyDecision Decide(const IHttpRequestBodyPolicy& policy, const QByteArray& head)
{
	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.ExecuteHttpParser(head, nullptr);

	return policy.DecideBody(request);
}


std::shared_ptr<FileSink> GetFileSink(const IHttpRequestBodyPolicy::BodyDecision& decision)
{
	return std::dynamic_pointer_cast<FileSink>(decision.sinkPtr);
}


QStringList GetFileNames(const QTemporaryDir& dir)
{
	return QDir(dir.path()).entryList(QDir::Files | QDir::NoDotAndDotDot);
}


QByteArray ReadFile(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	return file.readAll();
}


qint64 GetAvailableBytes(const QString& path)
{
	const QStorageInfo storage(path);

	return storage.isValid() ? storage.bytesAvailable() : -1;
}


bool IsFinalResponse(const QByteArray& message, const QByteArray& statusLine)
{
	return message.startsWith("HTTP/1.1 " + statusLine + "\r\n")
				&& message.contains("\r\nConnection: close\r\n")
				&& message.endsWith("\r\n\r\n");
}


} // namespace


// private slots

void CHttpDynamicBodyPolicyTest::AnnouncedBodySizeTest_data()
{
	QTest::addColumn<QByteArray>("contentLength");
	QTest::addColumn<QByteArray>("transferEncoding");
	QTest::addColumn<qint64>("size");

	// A request without a body (RFC 9112, section 6.3)
	QTest::newRow("no headers") << QByteArray() << QByteArray() << qint64(0);
	QTest::newRow("zero") << QByteArray("0") << QByteArray() << qint64(0);
	QTest::newRow("empty headers") << QByteArray("  ") << QByteArray(" ") << qint64(0);

	QTest::newRow("small") << QByteArray("5") << QByteArray() << qint64(5);
	QTest::newRow("100 MiB") << QByteArray("104857600") << QByteArray() << qint64(104857600);
	QTest::newRow("leading zeros") << QByteArray("007") << QByteArray() << qint64(7);
	QTest::newRow("spaces around") << QByteArray(" 7 ") << QByteArray() << qint64(7);
	QTest::newRow("more than 4 GiB") << QByteArray("5000000000") << QByteArray() << qint64(5000000000);
	QTest::newRow("the biggest") << QByteArray("9223372036854775807") << QByteArray() << std::numeric_limits<qint64>::max();

	// The size is not known: it is not a number (RFC 9110, section 8.6: digits only)
	QTest::newRow("too big") << QByteArray("9223372036854775808") << QByteArray() << qint64(-1);
	QTest::newRow("much too big") << QByteArray("99999999999999999999") << QByteArray() << qint64(-1);
	QTest::newRow("negative") << QByteArray("-5") << QByteArray() << qint64(-1);
	QTest::newRow("sign") << QByteArray("+5") << QByteArray() << qint64(-1);
	QTest::newRow("list") << QByteArray("5, 5") << QByteArray() << qint64(-1);
	QTest::newRow("letters") << QByteArray("abc") << QByteArray() << qint64(-1);
	QTest::newRow("hexadecimal") << QByteArray("0x10") << QByteArray() << qint64(-1);
	QTest::newRow("fraction") << QByteArray("1.5") << QByteArray() << qint64(-1);

	// Transfer-Encoding overrides Content-Length (RFC 9112, section 6.3), the size of a chunked body is not known
	QTest::newRow("chunked") << QByteArray() << QByteArray("chunked") << qint64(-1);
	QTest::newRow("chunked in upper case") << QByteArray() << QByteArray("Chunked") << qint64(-1);
	QTest::newRow("chunked after another coding") << QByteArray() << QByteArray("gzip, chunked") << qint64(-1);
	QTest::newRow("chunked with length") << QByteArray("5") << QByteArray("chunked") << qint64(-1);
	QTest::newRow("another coding") << QByteArray("5") << QByteArray("gzip") << qint64(-1);
}


void CHttpDynamicBodyPolicyTest::AnnouncedBodySizeTest()
{
	QFETCH(QByteArray, contentLength);
	QFETCH(QByteArray, transferEncoding);
	QFETCH(qint64, size);

	QCOMPARE(PolicyComp::GetAnnouncedBodySize(contentLength, transferEncoding), size);
}


void CHttpDynamicBodyPolicyTest::FileBodyLimitTest_data()
{
	QTest::addColumn<qint64>("totalBytes");
	QTest::addColumn<qint64>("availableBytes");
	QTest::addColumn<int>("reservePercent");
	QTest::addColumn<qint64>("limit");

	// The share of the disk stays free after the body: the free space without the share
	QTest::newRow("10% of 1000 bytes") << qint64(1000) << qint64(500) << 10 << qint64(400);
	QTest::newRow("no reserve") << qint64(1000) << qint64(500) << 0 << qint64(500);
	QTest::newRow("the whole disk is reserved") << qint64(1000) << qint64(500) << 100 << qint64(0);
	QTest::newRow("half of the disk") << qint64(1000) << qint64(900) << 50 << qint64(400);

	// There is less free space than the reserve: nothing fits, the limit is not negative
	QTest::newRow("free space is the reserve") << qint64(1000) << qint64(100) << 10 << qint64(0);
	QTest::newRow("free space is less than the reserve") << qint64(1000) << qint64(50) << 10 << qint64(0);
	QTest::newRow("one byte over the reserve") << qint64(1000) << qint64(101) << 10 << qint64(1);

	// The share is rounded down
	QTest::newRow("rounded share") << qint64(1005) << qint64(500) << 10 << qint64(400);
	QTest::newRow("share of a small disk") << qint64(99) << qint64(50) << 10 << qint64(41);

	// A wrong percent is cut to the range from 0 to 100
	QTest::newRow("more than 100%") << qint64(1000) << qint64(500) << 150 << qint64(0);
	QTest::newRow("negative percent") << qint64(1000) << qint64(500) << -5 << qint64(500);

	// The state of the disk is not known or there is no space
	QTest::newRow("no free space") << qint64(1000) << qint64(0) << 10 << qint64(0);
	QTest::newRow("free space is not known") << qint64(1000) << qint64(-1) << 10 << qint64(0);
	QTest::newRow("size is not known") << qint64(-1) << qint64(500) << 10 << qint64(0);
	QTest::newRow("empty disk size") << qint64(0) << qint64(100) << 10 << qint64(100);
}


void CHttpDynamicBodyPolicyTest::FileBodyLimitTest()
{
	QFETCH(qint64, totalBytes);
	QFETCH(qint64, availableBytes);
	QFETCH(int, reservePercent);
	QFETCH(qint64, limit);

	QCOMPARE(PolicyComp::CalculateFileBodyLimit(totalBytes, availableBytes, reservePercent), limit);
}


void CHttpDynamicBodyPolicyTest::FileBodyLimitOfHugeDiskTest()
{
	// The product of the size of a disk of 8 EiB and the percent does not fit into 64 bits, the share is still right
	const qint64 totalBytes = std::numeric_limits<qint64>::max();
	const qint64 availableBytes = totalBytes / 2;

	QCOMPARE(PolicyComp::CalculateFileBodyLimit(totalBytes, availableBytes, 10), qint64(3689348814741910323));
	QCOMPARE(PolicyComp::CalculateFileBodyLimit(totalBytes, availableBytes, 0), availableBytes);
	QCOMPARE(PolicyComp::CalculateFileBodyLimit(totalBytes, totalBytes, 100), qint64(0));
	QCOMPARE(PolicyComp::CalculateFileBodyLimit(totalBytes, totalBytes, 50), totalBytes - totalBytes / 2);
}


void CHttpDynamicBodyPolicyTest::DefaultAttributesTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	// The attributes are not set: 100 MiB to the memory, the bigger body goes to a file, 10% of the disk stays free.
	// Only the directory is set: the reference to it is obligatory and has no default.
	Policy policy;
	policy.SetRef("BodyDirectory", CreateDirectoryParam(dir.path()));
	policy.InitComponent();

	{
		const IHttpRequestBodyPolicy::BodyDecision decision = Decide(policy, CreateHeadOfSize(s_defaultMemoryBodyLimit));

		QCOMPARE(int(decision.mode), s_buffer);
		QCOMPARE(decision.maxBodySize, s_defaultMemoryBodyLimit);
		QVERIFY(!decision.sinkPtr);
	}

	{
		const IHttpRequestBodyPolicy::BodyDecision decision = Decide(policy, CreateHeadOfSize(s_mebibyte));

		QCOMPARE(int(decision.mode), s_buffer);
	}

	// The limit by the free space is of the disk of the test, so only the choice between the memory and the file is checked
	{
		const IHttpRequestBodyPolicy::BodyDecision decision = Decide(policy, CreateHeadOfSize(s_defaultMemoryBodyLimit + 1));

		QVERIFY(int(decision.mode) != s_buffer);
	}
}


void CHttpDynamicBodyPolicyTest::SmallBodyGoesToMemoryTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(50));

	QCOMPARE(int(decision.mode), s_buffer);
	QCOMPARE(decision.maxBodySize, qint64(100));
	QVERIFY(!decision.sinkPtr);

	// The body is in the memory: no file and nothing is held on the disk
	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::BodyEqualToMemoryLimitGoesToMemoryTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(100)).mode), s_buffer);
	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(101)).mode), s_stream);
}


void CHttpDynamicBodyPolicyTest::EmptyBodyGoesToMemoryTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// A request without a body (GET) and with Content-Length: 0 does not need a file even if the memory is not used for bodies
	const IHttpRequestBodyPolicy::BodyDecision withoutBody = Decide(*policyPtr, CreateHead(QByteArray(), QByteArray(), "GET"));

	QCOMPARE(int(withoutBody.mode), s_buffer);

	// For the request 0 is no limit, so the limit of the empty body is not 0
	QVERIFY(withoutBody.maxBodySize > 0);

	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(0)).mode), s_buffer);
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::BodyAboveMemoryLimitGoesToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(decision.mode), s_stream);
	QVERIFY(decision.sinkPtr);
	QVERIFY(decision.maxBodySize >= 101);

	// The file is created before the body is received, so the client does not send a body that cannot be saved
	const QString filePath = GetFileSink(decision)->GetFilePath();
	QVERIFY(QFileInfo::exists(filePath));
	QCOMPARE(QFileInfo(filePath).canonicalPath(), QFileInfo(dir.path()).canonicalFilePath());
	QCOMPARE(GetFileNames(dir).size(), 1);

	// The space of the body is held while it is received
	QCOMPARE(policyPtr->GetReservedSize(), qint64(101));

	// The body is not received: the receiver is released, the file is deleted and the space is free
	decision = IHttpRequestBodyPolicy::BodyDecision();

	QVERIFY(!QFileInfo::exists(filePath));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::ZeroMemoryLimitSendsEveryBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(1));

	QCOMPARE(int(decision.mode), s_stream);
	QVERIFY(GetFileSink(decision));
}


void CHttpDynamicBodyPolicyTest::ChunkedBodyGoesToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 1000;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// The size of the body is not known, so the body goes to a file whatever its real size is
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	QCOMPARE(int(decision.mode), s_stream);
	QVERIFY(GetFileSink(decision));
	QVERIFY(decision.maxBodySize > 0);
	QCOMPARE(GetFileNames(dir).size(), 1);

	// The size is not known, so the space is not held
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::ChunkedBodyLimitTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.maxBodySize = 1000;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// The request refuses the body that grows over the limit, so the limit is the smaller of the attribute and the free space
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	QCOMPARE(int(decision.mode), s_stream);
	QCOMPARE(decision.maxBodySize, qint64(1000));
}


void CHttpDynamicBodyPolicyTest::BodyAboveMaxBodySizeIsRefusedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 5000;
	settings.maxBodySize = 1000;
	settings.directory = dir.path();

	std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// The limit works for the bodies of the memory too
	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(1000)).mode), s_buffer);

	const IHttpRequestBodyPolicy::BodyDecision memoryDecision = Decide(*policyPtr, CreateHeadOfSize(1001));

	QCOMPARE(int(memoryDecision.mode), s_reject);
	QCOMPARE(memoryDecision.rejectStatusCode, s_payloadTooLarge);

	// And for the bodies that go to a file: they are refused before the file is created
	settings.memoryBodyLimit = 100;
	policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision fileDecision = Decide(*policyPtr, CreateHeadOfSize(2000));

	QCOMPARE(int(fileDecision.mode), s_reject);
	QCOMPARE(fileDecision.rejectStatusCode, s_payloadTooLarge);
	QVERIFY(!fileDecision.sinkPtr);
	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::MaxBodySizeLimitsFileBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.maxBodySize = 1000;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(500));

	QCOMPARE(int(decision.mode), s_stream);
	QCOMPARE(decision.maxBodySize, qint64(1000));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(500));
}


void CHttpDynamicBodyPolicyTest::ReserveOfWholeDiskRefusesFileBodiesTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.reservePercent = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// Nothing can be written to the disk, if all of it is reserved: the bodies of the memory are accepted as before
	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(100)).mode), s_buffer);

	const IHttpRequestBodyPolicy::BodyDecision knownSize = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(knownSize.mode), s_reject);
	QCOMPARE(knownSize.rejectStatusCode, s_payloadTooLarge);

	// The limit 0 of the request means no limit, so the body of the unknown size is refused at once
	const IHttpRequestBodyPolicy::BodyDecision unknownSize = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	QCOMPARE(int(unknownSize.mode), s_reject);
	QCOMPARE(unknownSize.rejectStatusCode, s_payloadTooLarge);

	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::BodyThatDoesNotFitIsRefusedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const qint64 availableBytes = GetAvailableBytes(dir.path());
	if (availableBytes < s_minAvailableBytes || availableBytes > s_maxAvailableBytes){
		QSKIP("The free space of the disk is not suitable for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// The body is twice as big as the whole free space of the disk, whatever the other programs do while the test works
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(availableBytes * 2 + s_mebibyte));

	QCOMPARE(int(decision.mode), s_reject);
	QCOMPARE(decision.rejectStatusCode, s_payloadTooLarge);
	QVERIFY(!decision.sinkPtr);

	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::BodyThatFitsIsAcceptedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const qint64 availableBytes = GetAvailableBytes(dir.path());
	if (availableBytes < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// A body of a half of the free space of the disk is a big file that is accepted
	const qint64 size = availableBytes / 2;
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(size));

	QCOMPARE(int(decision.mode), s_stream);
	QVERIFY(GetFileSink(decision));
	QVERIFY(decision.maxBodySize >= size);
	QCOMPARE(policyPtr->GetReservedSize(), size);
}


void CHttpDynamicBodyPolicyTest::ReservePercentIsKeptTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const qint64 availableBytes = GetAvailableBytes(dir.path());
	if (availableBytes < s_minAvailableBytes || availableBytes > s_maxAvailableBytes){
		QSKIP("The free space of the disk is not suitable for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.reservePercent = 50;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// A half of the disk stays free, so 60% of the free space can never be taken: the free space is not bigger than the disk
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(availableBytes / 10 * 6));

	QCOMPARE(int(decision.mode), s_reject);
	QCOMPARE(decision.rejectStatusCode, s_payloadTooLarge);
}


void CHttpDynamicBodyPolicyTest::ParallelBodiesShareFreeSpaceTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const qint64 availableBytes = GetAvailableBytes(dir.path());
	if (availableBytes < s_minAvailableBytes || availableBytes > s_maxAvailableBytes){
		QSKIP("The free space of the disk is not suitable for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// Each body fits into the free space, but two of them do not
	const qint64 size = availableBytes / 10 * 6;

	IHttpRequestBodyPolicy::BodyDecision first = Decide(*policyPtr, CreateHeadOfSize(size));

	QCOMPARE(int(first.mode), s_stream);
	QCOMPARE(policyPtr->GetReservedSize(), size);

	const IHttpRequestBodyPolicy::BodyDecision second = Decide(*policyPtr, CreateHeadOfSize(size));

	QCOMPARE(int(second.mode), s_reject);
	QCOMPARE(second.rejectStatusCode, s_payloadTooLarge);
	QVERIFY(!second.sinkPtr);

	// The refused body holds nothing
	QCOMPARE(policyPtr->GetReservedSize(), size);
	QCOMPARE(GetFileNames(dir).size(), 1);

	// The body of an unknown size does not hold the space, but it does not use the space of the first body
	const IHttpRequestBodyPolicy::BodyDecision unknownSize = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	// 40% of the free space is left for it, the other programs can change the free space a little while the test works
	QCOMPARE(int(unknownSize.mode), s_stream);
	QVERIFY(unknownSize.maxBodySize > 0);
	QVERIFY(unknownSize.maxBodySize < availableBytes / 10 * 7);
	QCOMPARE(policyPtr->GetReservedSize(), size);

	// The first body is not received, its space is free again
	first = IHttpRequestBodyPolicy::BodyDecision();

	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));

	const IHttpRequestBodyPolicy::BodyDecision third = Decide(*policyPtr, CreateHeadOfSize(size));

	QCOMPARE(int(third.mode), s_stream);
}


void CHttpDynamicBodyPolicyTest::WrittenBytesAreTakenOffReservedSizeTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(4096));

	QCOMPARE(int(decision.mode), s_stream);
	QCOMPARE(policyPtr->GetReservedSize(), qint64(4096));

	// The written bytes are not free on the disk any more, the disk counts them: they are not counted twice
	const QByteArray data(4096, 'x');

	QVERIFY(decision.sinkPtr->Write(data.constData(), 1000));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(3096));

	QVERIFY(decision.sinkPtr->Write(data.constData(), 3096));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));

	// A chunk that is bigger than it was announced (it is refused by the request) does not make the counter negative
	QVERIFY(decision.sinkPtr->Write(data.constData(), 100));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::ReservedSizeIsReleasedByFinishTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(4096));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(4096));

	// The body that is shorter than it was announced is not a body that holds the space
	const QByteArray data(100, 'x');
	QVERIFY(decision.sinkPtr->Write(data.constData(), data.size()));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(3996));

	QVERIFY(decision.sinkPtr->Finish());

	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));

	// The file stays for the handler of the request
	QCOMPARE(ReadFile(GetFileSink(decision)->GetFilePath()), data);
}


void CHttpDynamicBodyPolicyTest::TakenFileHoldsNoReservedSizeTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const QByteArray data(100, 'x');
	std::unique_ptr<QTemporaryFile> filePtr;

	{
		const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(4096));
		QCOMPARE(policyPtr->GetReservedSize(), qint64(4096));

		QVERIFY(decision.sinkPtr->Write(data.constData(), data.size()));
		QVERIFY(decision.sinkPtr->Finish());

		filePtr = GetFileSink(decision)->TakeFile();
		QVERIFY(filePtr != nullptr);
	}

	// The receiver is destroyed: the file that it has given away stays with its owner, and the space is not held any more
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
	QCOMPARE(ReadFile(filePtr->fileName()), data);
	QCOMPARE(GetFileNames(dir).size(), 1);

	filePtr.reset();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::ReservedSizeIsReleasedByAbortTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(4096));
	QCOMPARE(policyPtr->GetReservedSize(), qint64(4096));

	const QByteArray data(100, 'x');
	QVERIFY(decision.sinkPtr->Write(data.constData(), data.size()));

	decision.sinkPtr->Abort();

	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
	QVERIFY(GetFileNames(dir).isEmpty());

	// The second call does not release the space twice
	decision.sinkPtr->Abort();

	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::ReservedSizeIsReleasedByDestructionTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	{
		const IHttpRequestBodyPolicy::BodyDecision first = Decide(*policyPtr, CreateHeadOfSize(1000));
		const IHttpRequestBodyPolicy::BodyDecision second = Decide(*policyPtr, CreateHeadOfSize(2000));

		QCOMPARE(policyPtr->GetReservedSize(), qint64(3000));
		QCOMPARE(GetFileNames(dir).size(), 2);
	}

	// The receivers are destroyed without Finish() and Abort()
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::ReservedSizeOutlivesPolicyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.directory = dir.path();

	std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(4096));
	QCOMPARE(int(decision.mode), s_stream);

	// The handler of the request can keep the receiver longer than the container keeps the policy
	policyPtr.reset();

	const QByteArray data(100, 'x');
	QVERIFY(decision.sinkPtr->Write(data.constData(), data.size()));
	QVERIFY(decision.sinkPtr->Finish());

	decision = IHttpRequestBodyPolicy::BodyDecision();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::MissingDirectoryIsCreatedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString directory = dir.filePath("bodies/of/requests");

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = directory;

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// A mistake of the settings is seen when the component is created, not when the first big body comes
	QVERIFY(QFileInfo(directory).isDir());

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(decision.mode), s_stream);

	const std::shared_ptr<FileSink> sinkPtr = GetFileSink(decision);
	QVERIFY(sinkPtr);
	QCOMPARE(QFileInfo(sinkPtr->GetFilePath()).canonicalPath(), QFileInfo(directory).canonicalFilePath());
}


void CHttpDynamicBodyPolicyTest::UnusableDirectoryRefusesFileBodiesTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	// A directory cannot be created under a file
	QFile file(dir.filePath("file.txt"));
	QVERIFY(file.open(QIODevice::WriteOnly));
	file.close();

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.filePath("file.txt/bodies");

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// It is the mistake of the server, not of the client
	const IHttpRequestBodyPolicy::BodyDecision knownSize = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(knownSize.mode), s_reject);
	QCOMPARE(knownSize.rejectStatusCode, s_internalServerError);

	const IHttpRequestBodyPolicy::BodyDecision unknownSize = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	QCOMPARE(int(unknownSize.mode), s_reject);
	QCOMPARE(unknownSize.rejectStatusCode, s_internalServerError);

	// The bodies of the memory do not need the directory
	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(100)).mode), s_buffer);
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::EmptyDirectoryRefusesFileBodiesTest()
{
	SPolicySettings settings;
	settings.memoryBodyLimit = 100;

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	// The directory is set explicitly: an empty path is not the system temporary directory and is not the current directory
	const IHttpRequestBodyPolicy::BodyDecision knownSize = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(knownSize.mode), s_reject);
	QCOMPARE(knownSize.rejectStatusCode, s_internalServerError);
	QVERIFY(!knownSize.sinkPtr);

	const IHttpRequestBodyPolicy::BodyDecision unknownSize = Decide(*policyPtr, CreateHead(QByteArray(), s_chunkedHeader));

	QCOMPARE(int(unknownSize.mode), s_reject);
	QCOMPARE(unknownSize.rejectStatusCode, s_internalServerError);

	// The bodies of the memory do not need the directory
	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(100)).mode), s_buffer);
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::MissingDirectoryParameterRefusesFileBodiesTest()
{
	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.isDirectoryMissing = true;

	// The reference is obligatory, but the component is created anyway: the container does not stop the server
	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*policyPtr, CreateHeadOfSize(101));

	QCOMPARE(int(decision.mode), s_reject);
	QCOMPARE(decision.rejectStatusCode, s_internalServerError);
	QVERIFY(!decision.sinkPtr);

	QCOMPARE(int(Decide(*policyPtr, CreateHeadOfSize(100)).mode), s_buffer);
}


void CHttpDynamicBodyPolicyTest::SystemLocationIsUsedAsDirectoryTest()
{
	// The system location of the temporary directory is a ready parameter of a directory, the partitura of the demo server uses it
	icomp::TSimSharedComponentPtr<ifile::CSystemLocationComp> location;
	location->SetIntAttr("LocationType", 7);
	location->InitComponent();

	QVERIFY(!location->GetPath().isEmpty());

	Policy policy;
	policy.SetIntAttr("MemoryBodyLimit", 100);
	policy.SetIntAttr("FreeSpaceReservePercent", 0);
	policy.SetRef("BodyDirectory", location);
	policy.InitComponent();

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(policy, CreateHeadOfSize(101));

	QCOMPARE(int(decision.mode), s_stream);

	// The file is deleted with the receiver, so nothing is left in the temporary directory of the system
	const std::shared_ptr<FileSink> sinkPtr = GetFileSink(decision);
	QVERIFY(sinkPtr);
	QCOMPARE(QFileInfo(sinkPtr->GetFilePath()).canonicalPath(), QFileInfo(QDir::tempPath()).canonicalFilePath());
}


void CHttpDynamicBodyPolicyTest::DirectoryIsReadWhenComponentIsCreatedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString directory = dir.filePath("first");
	const QString otherDirectory = dir.filePath("second");

	DirectoryParam param = CreateDirectoryParam(directory);

	Policy policy;
	policy.SetIntAttr("MemoryBodyLimit", 100);
	policy.SetIntAttr("FreeSpaceReservePercent", 0);
	policy.SetRef("BodyDirectory", param);
	policy.InitComponent();

	// The policy is used from many threads, so it does not follow the changes of the parameter: the directory cannot change under a body
	param->SetPath(otherDirectory);

	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(policy, CreateHeadOfSize(101));

	QCOMPARE(int(decision.mode), s_stream);

	const std::shared_ptr<FileSink> sinkPtr = GetFileSink(decision);
	QVERIFY(sinkPtr);
	QCOMPARE(QFileInfo(sinkPtr->GetFilePath()).canonicalPath(), QFileInfo(directory).canonicalFilePath());
	QVERIFY(!QFileInfo::exists(otherDirectory));
}


void CHttpDynamicBodyPolicyTest::RequestReceivesSmallBodyToMemoryTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);
	request.SetBodyPolicy(*policyPtr);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello"));
	QVERIFY(!request.GetBodySink());
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::RequestReceivesBigBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 10;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	QByteArray body;
	for (int i = 0; i < 1000; ++i){
		body.append(char('a' + (i % 26)));
	}

	CRequestServletStub servlet;
	CEngineStub engine;

	std::shared_ptr<FileSink> sinkPtr;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(*policyPtr);

		QVERIFY(request.ExecuteHttpParser(CreateHeadOfSize(body.size()), nullptr));

		// The head is received: the file is created and the space of the body is held
		QCOMPARE(GetFileNames(dir).size(), 1);
		QCOMPARE(policyPtr->GetReservedSize(), qint64(1000));

		QVERIFY(request.ExecuteHttpParser(body.left(300), nullptr));
		QVERIFY(request.ExecuteHttpParser(body.mid(300, 400), nullptr));

		QCOMPARE(policyPtr->GetReservedSize(), qint64(300));

		QVERIFY(request.ExecuteHttpParser(body.mid(700), nullptr));

		QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);

		// The body is not in the memory of the request, the handler gets it from the receiver
		QVERIFY(request.GetBody().isEmpty());

		sinkPtr = std::dynamic_pointer_cast<FileSink>(request.GetBodySink());
		QVERIFY(sinkPtr);
		QVERIFY(sinkPtr->IsFinished());
		QCOMPARE(sinkPtr->GetReceivedSize(), qint64(1000));
		QCOMPARE(ReadFile(sinkPtr->GetFilePath()), body);

		QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
	}

	// The request is destroyed, the receiver is held by the test only
	QCOMPARE(GetFileNames(dir).size(), 1);

	sinkPtr.reset();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::RequestReceivesChunkedBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 1000;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);
	request.SetBodyPolicy(*policyPtr);

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_chunkedHeader) + s_chunkedBody, nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(request.GetBody().isEmpty());

	const std::shared_ptr<FileSink> sinkPtr = std::dynamic_pointer_cast<FileSink>(request.GetBodySink());
	QVERIFY(sinkPtr);
	QVERIFY(sinkPtr->IsFinished());
	QCOMPARE(ReadFile(sinkPtr->GetFilePath()), QByteArray("hello world"));
}


void CHttpDynamicBodyPolicyTest::RequestRefusesBodyAboveMaxBodySizeBeforeBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 50;
	settings.maxBodySize = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);
	request.SetBodyPolicy(*policyPtr);

	// The client has not sent the body yet
	QVERIFY(request.ExecuteHttpParser(CreateHeadOfSize(101), nullptr));

	QByteArray message;
	QCOMPARE(int(request.TakeHeadAction(message)), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());

	QCOMPARE(request.GetState(), IRequest::RS_HEADERS_COMPLETE);
	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::RequestRefusesBodyThatDoesNotFitBeforeBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 10;
	settings.reservePercent = 100;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);
	request.SetBodyPolicy(*policyPtr);

	QVERIFY(request.ExecuteHttpParser(CreateHeadOfSize(11), nullptr));

	QByteArray message;
	QCOMPARE(int(request.TakeHeadAction(message)), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpDynamicBodyPolicyTest::RequestRefusesChunkedBodyAboveLimitWhileReceivedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	if (GetAvailableBytes(dir.path()) < s_minAvailableBytes){
		QSKIP("There is not enough free space on the disk for the test");
	}

	SPolicySettings settings;
	settings.memoryBodyLimit = 0;
	settings.maxBodySize = 8;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);
	request.SetBodyPolicy(*policyPtr);

	// The size of the body is not known in advance: the first chunk (5 bytes) is received, the second one is over the limit
	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_chunkedHeader) + s_chunkedBody, nullptr));

	QByteArray message;
	QCOMPARE(int(request.TakeHeadAction(message)), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());
	QVERIFY(request.GetState() != IRequest::RS_MESSAGE_COMPLETE);

	// The part of the body that is received is not needed
	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::ExpectContinueIsAnsweredByDecisionTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 10;
	settings.maxBodySize = 2000;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	const QByteArray expect("Expect: 100-continue\r\n");

	// The big body that fits is accepted by the file: the client is allowed to send it
	{
		CRequestServletStub servlet;
		CEngineStub engine;
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(*policyPtr);

		QVERIFY(request.ExecuteHttpParser(CreateHead("1000", expect), nullptr));

		QByteArray message;
		QCOMPARE(int(request.TakeHeadAction(message)), int(IRequest::HA_SEND_INTERIM));
		QCOMPARE(message, QByteArray("HTTP/1.1 100 Continue\r\n\r\n"));
	}

	// The body that is bigger than the limit is refused with the final status, the client does not send it
	{
		CRequestServletStub servlet;
		CEngineStub engine;
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(*policyPtr);

		QVERIFY(request.ExecuteHttpParser(CreateHead("2001", expect), nullptr));

		QByteArray message;
		QCOMPARE(int(request.TakeHeadAction(message)), int(IRequest::HA_REJECT));
		QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());
		QVERIFY(!message.contains("100 Continue"));
	}

	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


void CHttpDynamicBodyPolicyTest::BrokenRequestLeavesNoFileAndNoReservedSizeTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SPolicySettings settings;
	settings.memoryBodyLimit = 10;
	settings.directory = dir.path();

	const std::unique_ptr<Policy> policyPtr = CreatePolicy(settings);

	CRequestServletStub servlet;
	CEngineStub engine;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(*policyPtr);

		// The connection is broken in the middle of the body
		QVERIFY(request.ExecuteHttpParser(CreateHeadOfSize(1000) + QByteArray(400, 'x'), nullptr));

		QCOMPARE(GetFileNames(dir).size(), 1);
		QCOMPARE(policyPtr->GetReservedSize(), qint64(600));
	}

	// Nobody holds the receiver: the request has aborted it and has released it
	QVERIFY(GetFileNames(dir).isEmpty());
	QCOMPARE(policyPtr->GetReservedSize(), qint64(0));
}


I_ADD_TEST(CHttpDynamicBodyPolicyTest);


