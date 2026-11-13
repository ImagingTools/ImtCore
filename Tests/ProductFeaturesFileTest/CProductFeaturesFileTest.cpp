// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CProductFeaturesFileTest.h"


// Qt includes
#include <QtCore/QTemporaryDir>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CProductInfo.h>
#include <imtlicgql/CProductInfoFileGeneratorComp.h>


namespace
{


const QByteArray s_rootUuid = "7a1d3c52-9f0e-4b8a-a6d2-3e5c1f0b9d47";


imtlic::IFeatureInfo::FeatureInfoPtr CreateFeature(const QByteArray& featureId, const QString& name, const QByteArrayList& requirements = QByteArrayList())
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo();
	featureInfoPtr->SetFeatureId(featureId);
	featureInfoPtr->SetFeatureName(name);
	featureInfoPtr->SetFeatureDescription(QStringLiteral("Allows ") + name);
	featureInfoPtr->SetIsPermission(true);
	featureInfoPtr->SetRequirements(requirements);

	return imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr);
}


/**
	/SensorManagement
	  View
	  Edit -> /SensorManagement/View
	    Add
	    Change: SerialNumber
	  Revisions: View -> /SensorManagement/View
	  Export (optional, not a permission) -> /SensorManagement/Revisions/View
*/
void SetupProduct(imtlic::CIdentifiableProductInfo& productInfo, const QString& rootName = QStringLiteral("Sensor Management"))
{
	productInfo.SetProductId("ProLife");
	productInfo.SetName(QStringLiteral("ProLife"));
	productInfo.SetProductDescription(QStringLiteral("Sensors and licenses"));
	productInfo.SetCategoryId("Software");

	imtlic::IFeatureInfo::FeatureInfoPtr changePtr = CreateFeature("Change", QStringLiteral("Change Sensor"));
	changePtr->InsertSubFeature(CreateFeature("SerialNumber", QStringLiteral("Change Serial Number")));

	imtlic::IFeatureInfo::FeatureInfoPtr editPtr = CreateFeature("Edit", QStringLiteral("Edit Sensor"), {"/SensorManagement/View"});
	editPtr->InsertSubFeature(CreateFeature("Add", QStringLiteral("Add Sensor")));
	editPtr->InsertSubFeature(changePtr);

	imtlic::CFeatureInfo* exportPtr = new imtlic::CFeatureInfo();
	exportPtr->SetFeatureId("Export");
	exportPtr->SetFeatureName(QStringLiteral("Export"));
	exportPtr->SetOptional(true);
	exportPtr->SetIsPermission(false);
	exportPtr->SetRequirements({"/SensorManagement/Revisions/View"});

	imtlic::IFeatureInfo::FeatureInfoPtr revisionsPtr = CreateFeature("Revisions", QStringLiteral("Sensor Revisions"));
	revisionsPtr->InsertSubFeature(CreateFeature("View", QStringLiteral("View Sensor Revisions"), {"/SensorManagement/View"}));

	imtlic::CIdentifiableFeatureInfo root;
	root.SetObjectUuid(s_rootUuid);
	root.SetFeatureId("SensorManagement");
	root.SetFeatureName(rootName);
	root.SetIsPermission(true);
	root.InsertSubFeature(CreateFeature("View", QStringLiteral("View Sensors")));
	root.InsertSubFeature(editPtr);
	root.InsertSubFeature(revisionsPtr);
	root.InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(exportPtr));

	QVERIFY(productInfo.AddFeature(s_rootUuid, root));
}


// One line per node: path, name, description, flags and requirements, so trees compare as text.
void DescribeFeature(const imtlic::IFeatureInfo& featureInfo, QStringList& lines)
{
	lines << QStringLiteral("%1|%2|%3|%4|%5|%6")
				.arg(QString::fromUtf8(imtlic::CalculateFeaturePath(featureInfo)), featureInfo.GetFeatureName(), featureInfo.GetFeatureDescription())
				.arg(featureInfo.IsOptional())
				.arg(featureInfo.IsPermission())
				.arg(QString::fromUtf8(featureInfo.GetRequirements().join(';')));

	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : featureInfo.GetSubFeatures()){
		if (subFeaturePtr.IsValid()){
			DescribeFeature(*subFeaturePtr, lines);
		}
	}
}


QStringList DescribeFeatures(imtlic::IProductInfo& productInfo)
{
	QStringList lines;
	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	for (const imtbase::ICollectionInfo::Id& featureId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (featureCollectionPtr->GetObjectData(featureId, dataPtr)){
			lines << QStringLiteral("element %1").arg(QString::fromUtf8(featureId));
			DescribeFeature(*dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr()), lines);
		}
	}

	return lines;
}


QString FilePath(const QTemporaryDir& directory)
{
	return directory.path() + QStringLiteral("/ProLifeFeatures.h");
}


QString Write(const QTemporaryDir& directory, imtlic::IProductInfo& productInfo)
{
	imtlicgql::CProductInfoFileGeneratorComp generator;
	if (generator.SaveToFile(productInfo, FilePath(directory)) != ifile::IFilePersistence::OS_OK){
		return QString();
	}

	QFile file(FilePath(directory));
	file.open(QIODevice::ReadOnly);

	return QString::fromUtf8(file.readAll());
}


ifile::IFilePersistence::OperationState Read(const QTemporaryDir& directory, const QString& text, imtlic::IProductInfo& productInfo)
{
	QFile file(FilePath(directory));
	file.open(QIODevice::WriteOnly);
	file.write(text.toUtf8());
	file.close();

	imtlicgql::CProductInfoFileGeneratorComp generator;

	return generator.LoadFromFile(productInfo, FilePath(directory));
}


// The header of the test product with one line replaced.
QString WrittenWith(const QTemporaryDir& directory, const QString& from, const QString& to)
{
	imtlic::CIdentifiableProductInfo productInfo;
	SetupProduct(productInfo);

	QString text = Write(directory, productInfo);
	text.replace(from, to);

	return text;
}


} // anonymous namespace


void CProductFeaturesFileTest::testProductSurvivesRoundTrip()
{
	QTemporaryDir directory;
	imtlic::CIdentifiableProductInfo written;
	SetupProduct(written);

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, Write(directory, written), read), ifile::IFilePersistence::OS_OK);

	QCOMPARE(read.GetProductId(), QByteArrayLiteral("ProLife"));
	QCOMPARE(read.GetName(), QStringLiteral("ProLife"));
	QCOMPARE(read.GetProductDescription(), QStringLiteral("Sensors and licenses"));
	QCOMPARE(read.GetCategoryId(), QByteArrayLiteral("Software"));
}


void CProductFeaturesFileTest::testFeatureTreeSurvivesRoundTrip()
{
	QTemporaryDir directory;
	imtlic::CIdentifiableProductInfo written;
	SetupProduct(written);

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, Write(directory, written), read), ifile::IFilePersistence::OS_OK);

	const QStringList readFeatures = DescribeFeatures(read);
	QCOMPARE(readFeatures, DescribeFeatures(written));
	QVERIFY(readFeatures.contains(QStringLiteral("/SensorManagement/Edit/Change/SerialNumber|Change Serial Number|Allows Change Serial Number|0|1|")));
	QVERIFY(readFeatures.contains(QStringLiteral("/SensorManagement/Edit|Edit Sensor|Allows Edit Sensor|0|1|/SensorManagement/View")));
	QVERIFY(readFeatures.contains(QStringLiteral("/SensorManagement/Export|Export||1|0|/SensorManagement/Revisions/View")));
	QVERIFY(readFeatures.contains(QStringLiteral("/SensorManagement/Revisions/View|View Sensor Revisions|Allows View Sensor Revisions|0|1|/SensorManagement/View")));

	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(read.GetFeatures()->GetObjectData(s_rootUuid, dataPtr));
	const imtlic::CIdentifiableFeatureInfo* rootPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(dataPtr.GetPtr());
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetObjectUuid(), s_rootUuid);
}


void CProductFeaturesFileTest::testQuotesAndBackslashesSurviveRoundTrip()
{
	QTemporaryDir directory;
	const QString name = QStringLiteral(R"(Sensor "Pro" C:\Sensors)");
	imtlic::CIdentifiableProductInfo written;
	SetupProduct(written, name);

	const QString text = Write(directory, written);
	const QString escapedName = QStringLiteral(R"x(QT_TRANSLATE_NOOP("Feature", "Sensor \"Pro\" C:\\Sensors"))x");
	QVERIFY(text.contains(escapedName));

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_OK);
	QCOMPARE(DescribeFeatures(read), DescribeFeatures(written));
}


void CProductFeaturesFileTest::testReadProductIsNew()
{
	QTemporaryDir directory;
	imtlic::CIdentifiableProductInfo written;
	SetupProduct(written);
	written.SetObjectUuid("0b6f2d1e-58a4-4c3e-9d7a-1f2e3d4c5b6a");
	const QString text = Write(directory, written);

	imtlic::CIdentifiableProductInfo first;
	imtlic::CIdentifiableProductInfo second;
	QCOMPARE(Read(directory, text, first), ifile::IFilePersistence::OS_OK);
	QCOMPARE(Read(directory, text, second), ifile::IFilePersistence::OS_OK);

	QVERIFY(!first.GetObjectUuid().isEmpty());
	QVERIFY(first.GetObjectUuid() != written.GetObjectUuid());
	QVERIFY(first.GetObjectUuid() != second.GetObjectUuid());
}


void CProductFeaturesFileTest::testUnknownStatementIsRefused()
{
	QTemporaryDir directory;
	const QString text = WrittenWith(directory, QStringLiteral("productInfo.SetCategoryId(\"Software\");"), QStringLiteral("productInfo.SetCategoryId(\"Software\");\n\tproductInfo.SetFamily(\"Sensors\");"));

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_FAILED);
}


void CProductFeaturesFileTest::testFileWithoutProductIdIsRefused()
{
	QTemporaryDir directory;
	const QString text = WrittenWith(directory, QStringLiteral("productInfo.SetProductId(\"ProLife\");"), QString());

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_FAILED);
}


void CProductFeaturesFileTest::testFeatureNeverAddedIsRefused()
{
	QTemporaryDir directory;
	const QString text = WrittenWith(directory, QStringLiteral("productInfo.AddFeature("), QStringLiteral("// productInfo.AddFeature("));

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_FAILED);
}


void CProductFeaturesFileTest::testUnknownFeatureVariableIsRefused()
{
	QTemporaryDir directory;
	const QString text = WrittenWith(directory, QStringLiteral("sensorManagement_ViewFeatureInfo->SetFeatureId"), QStringLiteral("sensorManagement_LookFeatureInfo->SetFeatureId"));

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_FAILED);
}


void CProductFeaturesFileTest::testRootFeatureAddedUnderAnotherUuidIsRefused()
{
	QTemporaryDir directory;
	const QString text = WrittenWith(directory, QStringLiteral("productInfo.AddFeature(\"%1\"").arg(QString::fromUtf8(s_rootUuid)), QStringLiteral("productInfo.AddFeature(\"another-uuid\""));

	imtlic::CIdentifiableProductInfo read;
	QCOMPARE(Read(directory, text, read), ifile::IFilePersistence::OS_FAILED);
}


I_ADD_TEST(CProductFeaturesFileTest);


