// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <utility>

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtpermtest
{


/**
	Checks that the permissions of a product agree with everything that names them. The feature tree is filled by the
	product, and the compositions, the migrations, the QML and C++ sources and the translations are read from the source
	tree at run time, so the test follows every change of the tree without being changed itself. Only the allowlists
	of the configuration name paths of the tree. The rules are those of Docs/Permissions-Rules.en.md.

	A product derives from this class, adds Q_OBJECT and I_ADD_TEST, and implements CreateConfiguration:
	the test slots of this class are run for the derived one.
*/
class CPermissionConsistencyTestBase: public QObject
{
	Q_OBJECT

public:
	/**
		What the test needs to know about a product. Paths are relative to rootDir.
	*/
	struct Configuration
	{
		/// Root of the source tree of the product.
		QString rootDir;
		/// Fills the feature tree of the product, as the server does at startup.
		std::function<void(imtlic::CProductInfo&)> fillProduct;
		/// Folders of the compositions of the packages; the folder of a composition names its package.
		QStringList packageCompositionDirs;
		/// Folders of the compositions of the applications.
		QStringList applicationCompositionDirs;
		/// Packages whose name ends with this only pass references on to the handlers inside them.
		QByteArray compositionPackageSuffix = QByteArrayLiteral("Voce");
		/// Folders of the SQL migrations that convert stored permissions.
		QStringList migrationDirs;
		/// Folders of the QML and C++ sources that name permissions.
		QStringList sourceDirs;
		/// Sources whose path contains one of these parts are not read: tests, and the file of the tree itself.
		QStringList excludedSourceParts;
		/// Files whose path contains one of these parts (any case) are no part of the product, such as disabled code.
		QStringList ignoredPathParts;
		/// Translation file of a language, with %1 for the language.
		QString translationFilePattern;
		/// Languages whose translation of the feature names is checked.
		QStringList languages;
		/// Areas that have an Edit group but no ViewPage, keyed by the full path of the area, with the reason.
		QHash<QByteArray, QString> areasWithoutPage;
		/**
			Commands declared with an empty permission list, which any signed-in user may run, with the reason. A key is the
			command id, or the composition file and the command id ("AddressPermissions.acc:GetElementInfo") for a command id
			that other collections declare with permissions.
		*/
		QHash<QString, QString> openCommands;
		/**
			Request handlers that may lack a permission provider or a permission checker, keyed by the element as the failure
			message names it and the attribute ("ApiHandlers.acc: Registry/Element CommandPermissions"), with the reason.
		*/
		QHash<QString, QString> handlersWithoutDeclarations;
	};

protected:
	virtual Configuration CreateConfiguration() const = 0;

private slots:
	/// Builds the feature tree and reads the compositions.
	void initTestCase();

	/// Every permission value of a composition is a full path with a leading '/'.
	void testCompositionPermissionsArePaths();
	/// Every permission path of a composition names a node of the tree; header permissions name leaves.
	void testCompositionPermissionsExist();
	/// Every permission a migration stores, after the later migrations converted it, is a leaf of the tree.
	void testMigrationPermissionsAreLeaves();
	/// Every permission literal of the QML and C++ sources names a node of the tree.
	void testSourcePermissionsExist();
	/// Requirements name nodes of the tree, go from the stronger right to the weaker one, and every Edit group requires the View of its area.
	void testRequirements();
	/// Every area with an Edit group has a ViewPage, or is allowlisted.
	void testEditAreasHavePages();
	/// The commands declared with an empty permission list are exactly the allowlisted ones.
	void testOpenCommandsAreAllowed();
	/// Every request handler of the compositions gets a permission provider and a permission checker.
	void testHandlersDeclarePermissions();
	/// Every name and description of a feature has a finished translation.
	void testFeatureTranslations();

private:
	/**
		Attribute of an element of a composition.
	*/
	struct AttributeInfo
	{
		QByteArray type;
		QByteArray exportId;
		bool isEnabled = false;
		QByteArrayList values;
	};

	/**
		Element of a composition.
	*/
	struct ElementInfo
	{
		QByteArray id;
		QByteArray packageId;
		QByteArray componentId;
		QHash<QByteArray, AttributeInfo> attributes;
	};

	/**
		Composition or one of its embedded registries.
	*/
	struct RegistryInfo
	{
		QString filePath;
		QByteArray packageId;
		QByteArray id;
		int parentIndex = -1;
		QList<ElementInfo> elements;
	};

	/**
		Permission value of an attribute of a composition.
	*/
	struct PermissionValue
	{
		QString location;
		QByteArray attributeId;
		QByteArray value;
	};

	/**
		Conversions of the stored permissions of one migration, in the order of the file.
	*/
	struct MigrationInfo
	{
		QString fileName;
		QList<std::pair<QByteArray, QByteArray>> conversions;
	};

	/**
		Adds the feature and its sub-features to the paths of the tree.
	*/
	void CollectFeatures(const imtlic::IFeatureInfo& featureInfo);

	/**
		Gets the files of the folders of the configuration and their subfolders that match the filters, sorted by folder,
		without the ignored paths.
	*/
	QStringList FindFiles(const QStringList& relativeDirs, const QStringList& nameFilters) const;

	/**
		Tells whether a source is excluded by the configuration.
	*/
	bool IsExcludedSource(const QString& filePath) const;

	/**
		Reads a composition and its embedded registries.
		\param packageId Package of the composition, empty for an application composition.
		\return False if the file cannot be read.
	*/
	bool LoadComposition(const QString& filePath, const QByteArray& packageId);

	/**
		Gets the composition file and the path of the element through the embedded registries, for messages.
	*/
	QString GetElementLabel(int registryIndex, const ElementInfo& elementInfo) const;

	/**
		Tells whether the attribute has a value, set on the element or, through its export, on every element that uses
		the registry of the element.
		\param reason Why the attribute has no value.
	*/
	bool IsAttributeSet(int registryIndex, const AttributeInfo& attributeInfo, QString& reason) const;

	/**
		Gets the values of all permission attributes of the compositions: enabled attributes with "Permission" in their
		id that hold ids or strings. References name elements, not permissions.
	*/
	QList<PermissionValue> CollectCompositionPermissions() const;

	/**
		Gets the paths that a migration stores in place of the path, nothing if it drops the path, or the path itself if it does not convert it.
	*/
	QByteArrayList ConvertByMigration(const MigrationInfo& migrationInfo, const QByteArray& path) const;

	/**
		Reads the messages of the context Feature of a translation file.
		\return Finished state of the translation by source text.
	*/
	QHash<QString, bool> LoadFeatureTranslations(const QString& filePath) const;

private:
	Configuration m_configuration;
	imtlic::CProductInfo m_productInfo;
	QList<imtbase::IObjectCollection::DataPtr> m_rootFeatures;
	QByteArrayList m_featurePaths;
	QHash<QByteArray, const imtlic::IFeatureInfo*> m_features;
	QList<RegistryInfo> m_registries;
};


} // namespace imtpermtest

