// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtpermtest/CPermissionConsistencyTestBase.h>


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>
#include <QtCore/QXmlStreamReader>


static const QSet<QByteArray> s_permissionValueTypes = {QByteArrayLiteral("Id"), QByteArrayLiteral("Id[]"), QByteArrayLiteral("String"), QByteArrayLiteral("String[]")};
static const QByteArrayList s_handlerAttributeIds = {QByteArrayLiteral("CommandPermissions"), QByteArrayLiteral("PermissionChecker")};

static const QRegularExpression s_numberExpression(QStringLiteral(R"((\d+))"));
static const QRegularExpression s_conversionExpression(QStringLiteral(R"(\(\s*'([^']*)'\s*,\s*(?:'(/[^']*)'|NULL)\s*\))"));
static const QRegularExpression s_qmlExpression(QStringLiteral(R"(checkPermission\(\s*["']([^"']*)["']\s*\)|permissionId\s*:\s*["']([^"']*)["'])"));


namespace imtpermtest
{


void CPermissionConsistencyTestBase::initTestCase()
{
	m_configuration = CreateConfiguration();
	QVERIFY(m_configuration.fillProduct != nullptr);
	m_configuration.fillProduct(m_productInfo);

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfo.GetFeatures();
	QVERIFY(featureCollectionPtr != nullptr);

	const imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		QVERIFY(featureCollectionPtr->GetObjectData(elementId, dataPtr));

		const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
		QVERIFY(featureInfoPtr != nullptr);

		m_rootFeatures.append(dataPtr);
		CollectFeatures(*featureInfoPtr);
	}

	QVERIFY(!m_featurePaths.isEmpty());

	const QStringList packageCompositions = FindFiles(m_configuration.packageCompositionDirs, {QStringLiteral("*.acc")});
	for (const QString& filePath : packageCompositions){
		QString packageId = QFileInfo(filePath).dir().dirName();
		if (packageId.endsWith(u".arp")){
			packageId.chop(4);
		}

		QVERIFY2(LoadComposition(filePath, packageId.toUtf8()), qPrintable(filePath));
	}

	const QStringList applicationCompositions = FindFiles(m_configuration.applicationCompositionDirs, {QStringLiteral("*.acc")});
	for (const QString& filePath : applicationCompositions){
		QVERIFY2(LoadComposition(filePath, QByteArray()), qPrintable(filePath));
	}

	QVERIFY(!m_registries.isEmpty());
}


void CPermissionConsistencyTestBase::testCompositionPermissionsArePaths()
{
	QStringList offenders;

	const QList<PermissionValue> permissions = CollectCompositionPermissions();
	for (const PermissionValue& permission : permissions){
		// A header without a permission is open to everyone: the list goes by the position of the header.
		if (permission.value.isEmpty() && permission.attributeId == QByteArrayLiteral("HeaderPermissions")){
			continue;
		}

		if (permission.value.startsWith('/')){
			continue;
		}

		offenders.append(QStringLiteral(R"(%1 %2: "%3" is no full path)")
					.arg(
						/*1*/ permission.location,
						/*2*/ QString(permission.attributeId),
						/*3*/ QString(permission.value)
					));
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testCompositionPermissionsExist()
{
	QStringList offenders;

	const QList<PermissionValue> permissions = CollectCompositionPermissions();
	for (const PermissionValue& permission : permissions){
		if (!permission.value.startsWith('/')){
			continue;
		}

		const imtlic::IFeatureInfo* featureInfoPtr = m_features.value(permission.value);
		if (featureInfoPtr == nullptr){
			offenders.append(QStringLiteral("%1 %2: %3 is not in the tree")
						.arg(
							/*1*/ permission.location,
							/*2*/ QString(permission.attributeId),
							/*3*/ QString(permission.value)
						));

			continue;
		}

		/**
			A group is a valid check, meaning any leaf inside (R4.4), except in the header permissions:
			every column of a table has a leaf of its own, so a group there means a mistyped column.
		*/
		if (permission.attributeId == QByteArrayLiteral("HeaderPermissions") && !featureInfoPtr->GetSubFeatures().isEmpty()){
			offenders.append(QStringLiteral("%1 %2: %3 is a group, a header permission must be a leaf")
						.arg(
							/*1*/ permission.location,
							/*2*/ QString(permission.attributeId),
							/*3*/ QString(permission.value)
						));
		}
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testMigrationPermissionsAreLeaves()
{
	QList<MigrationInfo> migrations;

	const QStringList scriptPaths = FindFiles(m_configuration.migrationDirs, {QStringLiteral("*.sql")});
	for (const QString& filePath : scriptPaths){
		QFile file(filePath);
		QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(filePath));

		MigrationInfo migrationInfo;
		migrationInfo.fileName = QFileInfo(filePath).fileName();

		const QString script = file.readAll();
		QRegularExpressionMatchIterator matchIterator = s_conversionExpression.globalMatch(script);
		while (matchIterator.hasNext()){
			const QRegularExpressionMatch match = matchIterator.next();
			migrationInfo.conversions.append({ match.captured(1).toUtf8(), match.captured(2).toUtf8() });
		}

		if (!migrationInfo.conversions.isEmpty()){
			migrations.append(migrationInfo);
		}
	}

	std::sort(
		migrations.begin(),
		migrations.end(),
		[](const MigrationInfo& first, const MigrationInfo& second){
			return s_numberExpression.match(first.fileName).captured(1).toInt() < s_numberExpression.match(second.fileName).captured(1).toInt();
		}
	);

	/**
		A stored permission stays until a later migration converts it, so what the last migration leaves must be a
		leaf: a stored group grants nothing (R3.5).
	*/
	QStringList offenders;
	for (int migrationIndex = 0; migrationIndex < migrations.size(); ++migrationIndex){
		const MigrationInfo& migrationInfo = migrations.at(migrationIndex);
		for (const std::pair<QByteArray, QByteArray>& conversion : migrationInfo.conversions){
			// A conversion to NULL drops the stored value, nothing of it stays.
			if (conversion.second.isEmpty()){
				continue;
			}

			QByteArrayList storedPaths = {conversion.second};
			for (int laterIndex = migrationIndex + 1; laterIndex < migrations.size(); ++laterIndex){
				QByteArrayList convertedPaths;
				for (const QByteArray& storedPath : std::as_const(storedPaths)){
					const QByteArrayList laterPaths = ConvertByMigration(migrations.at(laterIndex), storedPath);
					for (const QByteArray& laterPath : laterPaths){
						if (!convertedPaths.contains(laterPath)){
							convertedPaths.append(laterPath);
						}
					}
				}

				storedPaths = convertedPaths;
			}

			for (const QByteArray& storedPath : std::as_const(storedPaths)){
				const imtlic::IFeatureInfo* featureInfoPtr = m_features.value(storedPath);
				if (featureInfoPtr != nullptr && featureInfoPtr->GetSubFeatures().isEmpty()){
					continue;
				}

				offenders.append(QStringLiteral("%1: %2 -> %3 is stored as %4, which %5")
							.arg(
								/*1*/ migrationInfo.fileName,
								/*2*/ QString(conversion.first),
								/*3*/ QString(conversion.second),
								/*4*/ QString(storedPath),
								/*5*/ (featureInfoPtr == nullptr) ? QStringLiteral("is not in the tree") : QStringLiteral("is a group")
							));
			}
		}
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testSourcePermissionsExist()
{
	/**
		A C++ literal counts only if its first segment is a root of the tree, so that URLs and file paths are not taken
		for permissions.
	*/
	QSet<QString> rootIds;
	for (const QByteArray& featurePath : std::as_const(m_featurePaths)){
		rootIds.insert(QRegularExpression::escape(featurePath.split('/').value(1)));
	}

	const QStringList rootAlternatives(rootIds.cbegin(), rootIds.cend());
	const QRegularExpression cppExpression(QStringLiteral(R"re("(/(?:%1)(?:/[^"]*)?)")re").arg(rootAlternatives.join('|')));
	const QStringList nameFilters = {QStringLiteral("*.qml"), QStringLiteral("*.h"), QStringLiteral("*.cpp")};

	const QStringList sourcePaths = FindFiles(m_configuration.sourceDirs, nameFilters);

	const QDir rootDir(m_configuration.rootDir);
	QStringList offenders;
	for (const QString& filePath : std::as_const(sourcePaths)){
		// Tests use trees of their own, and the tree itself is the reference.
		if (IsExcludedSource(filePath)){
			continue;
		}

		QFile file(filePath);
		QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(filePath));

		const QString text = file.readAll();
		const bool isQml = filePath.endsWith(u".qml");
		QRegularExpressionMatchIterator matchIterator = isQml ? s_qmlExpression.globalMatch(text) : cppExpression.globalMatch(text);
		while (matchIterator.hasNext()){
			const QRegularExpressionMatch match = matchIterator.next();
			const QString permissionId = match.captured(1).isEmpty() ? match.captured(2) : match.captured(1);
			if (m_features.contains(permissionId.toUtf8())){
				continue;
			}

			offenders.append(QStringLiteral("%1:%2: %3 is not in the tree")
						.arg(
							/*1*/ rootDir.relativeFilePath(filePath),
							/*2*/ QString::number(text.left(match.capturedStart()).count('\n') + 1),
							/*3*/ permissionId
						));
		}
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testRequirements()
{
	QStringList offenders;
	for (const QByteArray& featurePath : std::as_const(m_featurePaths)){
		const imtlic::IFeatureInfo* featureInfoPtr = m_features.value(featurePath);
		const QByteArray featureId = featureInfoPtr->GetFeatureId();
		const bool isLeaf = featureInfoPtr->GetSubFeatures().isEmpty();
		const QByteArrayList requirements = featureInfoPtr->GetRequirements();

		for (const QByteArray& requirement : requirements){
			if (!m_features.contains(requirement)){
				offenders.append(QStringLiteral("%1 requires %2, which is not in the tree").arg(QString(featurePath), QString(requirement)));

				continue;
			}

			if (!isLeaf || !featureId.startsWith("View")){
				continue;
			}

			// A view right never includes an edit right (R5.3).
			const QByteArrayList requiredSegments = requirement.split('/');
			for (const QByteArray& requiredSegment : requiredSegments){
				if (requiredSegment.startsWith("Edit")){
					offenders.append(QStringLiteral("%1 requires %2: a view right must not include an edit right").arg(QString(featurePath), QString(requirement)));

					break;
				}
			}
		}

		if (featureId != QByteArrayLiteral("Edit") || isLeaf){
			continue;
		}

		const QByteArray viewPath = featurePath.left(featurePath.lastIndexOf('/')) + QByteArrayLiteral("/View");
		const imtlic::IFeatureInfo* viewInfoPtr = m_features.value(viewPath);
		if (viewInfoPtr == nullptr || !viewInfoPtr->GetSubFeatures().isEmpty()){
			continue;
		}

		if (!requirements.contains(viewPath)){
			offenders.append(QStringLiteral("%1 does not require %2 (R3.4)").arg(QString(featurePath), QString(viewPath)));
		}
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testEditAreasHavePages()
{
	QStringList offenders;
	QSet<QByteArray> usedExceptions;
	for (const QByteArray& featurePath : std::as_const(m_featurePaths)){
		const imtlic::IFeatureInfo* editInfoPtr = m_features.value(featurePath + QByteArrayLiteral("/Edit"));
		if (editInfoPtr == nullptr || editInfoPtr->GetSubFeatures().isEmpty()){
			continue;
		}

		if (m_features.contains(featurePath + QByteArrayLiteral("/ViewPage"))){
			continue;
		}

		if (m_configuration.areasWithoutPage.contains(featurePath)){
			usedExceptions.insert(featurePath);

			continue;
		}

		offenders.append(QStringLiteral("%1 has an Edit group but no ViewPage").arg(QString(featurePath)));
	}

	QStringList staleExceptions;
	for (auto it = m_configuration.areasWithoutPage.constBegin(); it != m_configuration.areasWithoutPage.constEnd(); ++it){
		if (!usedExceptions.contains(it.key())){
			staleExceptions.append(QStringLiteral("%1 is allowlisted without need").arg(QString(it.key())));
		}
	}

	staleExceptions.sort();
	offenders += staleExceptions;

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testOpenCommandsAreAllowed()
{
	QStringList offenders;
	QSet<QString> openCommands;
	for (int registryIndex = 0; registryIndex < m_registries.size(); ++registryIndex){
		const RegistryInfo& registryInfo = m_registries.at(registryIndex);
		for (const ElementInfo& elementInfo : registryInfo.elements){
			if (elementInfo.componentId != QByteArrayLiteral("SingleCommandPermissionsProvider")){
				continue;
			}

			const AttributeInfo permissionsInfo = elementInfo.attributes.value(QByteArrayLiteral("CommandPermissions"));
			bool isOpen = true;
			for (const QByteArray& permissionId : permissionsInfo.values){
				isOpen = isOpen && permissionId.isEmpty();
			}

			// A disabled list may be given by the composition that uses this one.
			QString reason;
			if (!permissionsInfo.isEnabled && IsAttributeSet(registryIndex, permissionsInfo, reason)){
				isOpen = false;
			}

			if (!isOpen){
				continue;
			}

			const QString commandId = elementInfo.attributes.value(QByteArrayLiteral("CommandId")).values.value(0);
			const QString fileCommandKey = QStringLiteral("%1:%2").arg(QFileInfo(registryInfo.filePath).fileName(), commandId);
			const QString commandKey = m_configuration.openCommands.contains(fileCommandKey) ? fileCommandKey : commandId;
			openCommands.insert(commandKey);

			if (!m_configuration.openCommands.contains(commandKey)){
				offenders.append(QStringLiteral("%1: command %2 is open to any signed-in user").arg(GetElementLabel(registryIndex, elementInfo), commandId));
			}
		}
	}

	QStringList staleExceptions;
	for (auto it = m_configuration.openCommands.constBegin(); it != m_configuration.openCommands.constEnd(); ++it){
		if (!openCommands.contains(it.key())){
			staleExceptions.append(QStringLiteral("%1 is allowlisted as open, but no composition declares it open").arg(it.key()));
		}
	}

	staleExceptions.sort();
	offenders += staleExceptions;

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testHandlersDeclarePermissions()
{
	QStringList offenders;
	QSet<QString> usedExceptions;
	for (int registryIndex = 0; registryIndex < m_registries.size(); ++registryIndex){
		for (const ElementInfo& elementInfo : m_registries.at(registryIndex).elements){
			// A composition passes the references on to the handlers inside it, which are checked themselves.
			if (elementInfo.packageId.isEmpty() || elementInfo.packageId.endsWith(m_configuration.compositionPackageSuffix)){
				continue;
			}

			for (const QByteArray& attributeId : s_handlerAttributeIds){
				const auto attributeIt = elementInfo.attributes.constFind(attributeId);
				if (attributeIt == elementInfo.attributes.constEnd() || attributeIt->type != QByteArrayLiteral("Reference")){
					continue;
				}

				QString reason;
				if (IsAttributeSet(registryIndex, *attributeIt, reason)){
					continue;
				}

				const QString key = QStringLiteral("%1 %2").arg(GetElementLabel(registryIndex, elementInfo), QString(attributeId));
				if (m_configuration.handlersWithoutDeclarations.contains(key)){
					usedExceptions.insert(key);

					continue;
				}

				offenders.append(QStringLiteral("%1 is not set (%2): the handler lets every caller through").arg(key, reason));
			}
		}
	}

	QStringList staleExceptions;
	for (auto it = m_configuration.handlersWithoutDeclarations.constBegin(); it != m_configuration.handlersWithoutDeclarations.constEnd(); ++it){
		if (!usedExceptions.contains(it.key())){
			staleExceptions.append(QStringLiteral("%1 is allowlisted without need").arg(it.key()));
		}
	}

	staleExceptions.sort();
	offenders += staleExceptions;

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::testFeatureTranslations()
{
	QStringList offenders;
	for (const QString& language : m_configuration.languages){
		const QString filePath = QDir(m_configuration.rootDir).filePath(m_configuration.translationFilePattern.arg(language));
		const QHash<QString, bool> translations = LoadFeatureTranslations(filePath);
		QVERIFY2(!translations.isEmpty(), qPrintable(filePath));

		for (const QByteArray& featurePath : std::as_const(m_featurePaths)){
			const imtlic::IFeatureInfo* featureInfoPtr = m_features.value(featurePath);
			const QStringList texts = {featureInfoPtr->GetFeatureName(), featureInfoPtr->GetFeatureDescription()};
			for (const QString& text : texts){
				if (text.isEmpty() || translations.value(text)){
					continue;
				}

				offenders.append(QStringLiteral(R"(%1 %2: "%3" has %4)")
							.arg(
								/*1*/ language,
								/*2*/ QString(featurePath),
								/*3*/ text,
								/*4*/ translations.contains(text) ? QStringLiteral("no finished translation") : QStringLiteral("no message in the context Feature")
							));
			}
		}
	}

	QVERIFY2(offenders.isEmpty(), qPrintable(offenders.join('\n')));
}


void CPermissionConsistencyTestBase::CollectFeatures(const imtlic::IFeatureInfo& featureInfo)
{
	const QByteArray featurePath = imtlic::CalculateFeaturePath(featureInfo);
	m_featurePaths.append(featurePath);
	m_features.insert(featurePath, &featureInfo);

	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : featureInfo.GetSubFeatures()){
		if (subFeaturePtr.IsValid()){
			CollectFeatures(*subFeaturePtr);
		}
	}
}


QStringList CPermissionConsistencyTestBase::FindFiles(const QStringList& relativeDirs, const QStringList& nameFilters) const
{
	QStringList retVal;

	const QDir rootDir(m_configuration.rootDir);
	for (const QString& relativeDir : relativeDirs){
		QStringList dirFiles;

		QDirIterator fileIterator(
					rootDir.filePath(relativeDir),
					nameFilters,
					QDir::Files,
					QDirIterator::Subdirectories);

		while (fileIterator.hasNext()){
			const QString filePath = fileIterator.next();

			bool isIgnored = false;
			for (const QString& ignoredPart : std::as_const(m_configuration.ignoredPathParts)){
				isIgnored = isIgnored || filePath.contains(ignoredPart, Qt::CaseInsensitive);
			}

			if (!isIgnored){
				dirFiles.append(filePath);
			}
		}

		dirFiles.sort();
		retVal += dirFiles;
	}

	return retVal;
}


bool CPermissionConsistencyTestBase::IsExcludedSource(const QString& filePath) const
{
	for (const QString& excludedPart : std::as_const(m_configuration.excludedSourceParts)){
		if (filePath.contains(excludedPart)){
			return true;
		}
	}

	return false;
}


bool CPermissionConsistencyTestBase::LoadComposition(const QString& filePath, const QByteArray& packageId)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return false;
	}

	QXmlStreamReader reader(&file);
	QList<int> registryStack;
	ElementInfo elementInfo;
	QByteArray attributeId;
	AttributeInfo attributeInfo;
	bool isInAttribute = false;

	while (!reader.atEnd()){
		reader.readNext();

		if (reader.isEndElement()){
			if (reader.name() == u"EmbeddedRegistry"){
				registryStack.removeLast();
			}
			else if (reader.name() == u"Element"){
				m_registries[registryStack.last()].elements.append(elementInfo);
			}
			else if (reader.name() == u"AttributeInfo"){
				elementInfo.attributes.insert(attributeId, attributeInfo);
				isInAttribute = false;
			}

			continue;
		}

		if (!reader.isStartElement()){
			continue;
		}

		const QXmlStreamAttributes attributes = reader.attributes();
		if (reader.name() == u"Acf" || reader.name() == u"EmbeddedRegistry"){
			RegistryInfo registryInfo;
			registryInfo.filePath = filePath;
			registryInfo.packageId = packageId;
			registryInfo.id = registryStack.isEmpty() ? QFileInfo(filePath).completeBaseName().toUtf8() : attributes.value(u"Id").toUtf8();
			registryInfo.parentIndex = registryStack.isEmpty() ? -1 : registryStack.last();

			m_registries.append(registryInfo);
			registryStack.append(m_registries.size() - 1);
		}
		else if (reader.name() == u"Element"){
			elementInfo = ElementInfo();
			elementInfo.id = attributes.value(u"Id").toUtf8();
			elementInfo.packageId = attributes.value(u"PackageId").toUtf8();
			elementInfo.componentId = attributes.value(u"ComponentId").toUtf8();
		}
		else if (reader.name() == u"AttributeInfo"){
			attributeId = attributes.value(u"Id").toUtf8();
			attributeInfo = AttributeInfo();
			attributeInfo.type = attributes.value(u"Type").toUtf8();
			attributeInfo.exportId = attributes.value(u"ExportId").toUtf8();
			isInAttribute = true;
		}
		else if (isInAttribute && reader.name() == u"Data"){
			attributeInfo.isEnabled = (attributes.value(u"IsEnabled") == u"true");
			if (attributes.hasAttribute(u"Value")){
				attributeInfo.values.append(attributes.value(u"Value").toUtf8());
			}
		}
		else if (isInAttribute && reader.name() == u"Value"){
			attributeInfo.values.append(reader.readElementText().toUtf8());
		}
	}

	return !reader.hasError() && !registryStack.isEmpty();
}


QString CPermissionConsistencyTestBase::GetElementLabel(int registryIndex, const ElementInfo& elementInfo) const
{
	QStringList names = {elementInfo.id};
	int parentIndex = registryIndex;
	while (m_registries.at(parentIndex).parentIndex >= 0){
		names.prepend(m_registries.at(parentIndex).id);
		parentIndex = m_registries.at(parentIndex).parentIndex;
	}

	const QString retVal = QStringLiteral("%1: %2").arg(QFileInfo(m_registries.at(registryIndex).filePath).fileName(), names.join('/'));

	return retVal;
}


bool CPermissionConsistencyTestBase::IsAttributeSet(int registryIndex, const AttributeInfo& attributeInfo, QString& reason) const
{
	if (attributeInfo.isEnabled){
		for (const QByteArray& value : attributeInfo.values){
			if (!value.isEmpty()){
				return true;
			}
		}

		reason = QStringLiteral("set to nothing");

		return false;
	}

	if (attributeInfo.exportId.isEmpty()){
		reason = QStringLiteral("disabled");

		return false;
	}

	/**
		An exported attribute takes the value that the elements using the registry give it: the elements of the parent
		registry that name an embedded registry with an empty package, or the elements of any composition that name a
		composition by its package and its file name.
	*/
	const RegistryInfo& registryInfo = m_registries.at(registryIndex);
	bool isUsed = false;
	for (int userRegistryIndex = 0; userRegistryIndex < m_registries.size(); ++userRegistryIndex){
		const bool isEmbeddedUser = (registryInfo.parentIndex == userRegistryIndex);
		if (!isEmbeddedUser && (registryInfo.parentIndex >= 0 || registryInfo.packageId.isEmpty())){
			continue;
		}

		const QByteArray userPackageId = isEmbeddedUser ? QByteArray() : registryInfo.packageId;
		for (const ElementInfo& userInfo : m_registries.at(userRegistryIndex).elements){
			if (userInfo.packageId != userPackageId || userInfo.componentId != registryInfo.id){
				continue;
			}

			isUsed = true;

			const auto userAttributeIt = userInfo.attributes.constFind(attributeInfo.exportId);
			if (userAttributeIt == userInfo.attributes.constEnd()){
				reason = QStringLiteral("%1 does not pass %2").arg(GetElementLabel(userRegistryIndex, userInfo), QString(attributeInfo.exportId));

				return false;
			}

			if (!IsAttributeSet(userRegistryIndex, *userAttributeIt, reason)){
				reason = QStringLiteral("%1 %2 %3").arg(GetElementLabel(userRegistryIndex, userInfo), QString(attributeInfo.exportId), reason);

				return false;
			}
		}
	}

	if (!isUsed){
		reason = QStringLiteral("exported as %1, but no composition uses %2").arg(QString(attributeInfo.exportId), QString(registryInfo.id));
	}

	return isUsed;
}


QList<CPermissionConsistencyTestBase::PermissionValue> CPermissionConsistencyTestBase::CollectCompositionPermissions() const
{
	QList<PermissionValue> retVal;
	for (int registryIndex = 0; registryIndex < m_registries.size(); ++registryIndex){
		for (const ElementInfo& elementInfo : m_registries.at(registryIndex).elements){
			QByteArrayList attributeIds = elementInfo.attributes.keys();
			std::sort(attributeIds.begin(), attributeIds.end());

			for (const QByteArray& attributeId : std::as_const(attributeIds)){
				const AttributeInfo attributeInfo = elementInfo.attributes.value(attributeId);
				if (!attributeId.contains("Permission") || !attributeInfo.isEnabled || !s_permissionValueTypes.contains(attributeInfo.type)){
					continue;
				}

				for (const QByteArray& value : attributeInfo.values){
					PermissionValue permission;
					permission.location = GetElementLabel(registryIndex, elementInfo);
					permission.attributeId = attributeId;
					permission.value = value;

					retVal.append(permission);
				}
			}
		}
	}

	return retVal;
}


QByteArrayList CPermissionConsistencyTestBase::ConvertByMigration(const MigrationInfo& migrationInfo, const QByteArray& path) const
{
	QByteArrayList retVal;
	bool isConverted = false;
	for (const std::pair<QByteArray, QByteArray>& conversion : migrationInfo.conversions){
		if (conversion.first != path){
			continue;
		}

		isConverted = true;
		if (!conversion.second.isEmpty()){
			retVal.append(conversion.second);
		}
	}

	if (!isConverted){
		retVal.append(path);
	}

	return retVal;
}


QHash<QString, bool> CPermissionConsistencyTestBase::LoadFeatureTranslations(const QString& filePath) const
{
	QHash<QString, bool> retVal;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return retVal;
	}

	QXmlStreamReader reader(&file);
	bool isFeatureContext = false;
	QString source;

	while (!reader.atEnd()){
		reader.readNext();

		if (!reader.isStartElement()){
			continue;
		}

		if (reader.name() == u"name"){
			isFeatureContext = (reader.readElementText() == u"Feature");
		}
		else if (isFeatureContext && reader.name() == u"source"){
			source = reader.readElementText();
		}
		else if (isFeatureContext && reader.name() == u"translation"){
			// Unfinished, vanished and obsolete messages carry a type.
			const bool hasType = reader.attributes().hasAttribute(u"type");
			const bool isFinished = !hasType && !reader.readElementText(QXmlStreamReader::IncludeChildElements).trimmed().isEmpty();

			retVal[source] = retVal.value(source) || isFinished;
		}
	}

	return retVal;
}


} // namespace imtpermtest

