// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CSdlUnionObjectGeneratorComp.h>


// Qt includes
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlEntryBase.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlField.h>


namespace imtsdlgencpp
{

// public methods


bool CSdlUnionObjectGeneratorComp::ProcessEntry(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* headerDevicePtr, QIODevice* sourceDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());

	bool ok = true;
	if (headerDevicePtr != nullptr){
		QTextStream stream(headerDevicePtr);
		ok = ok && ProcessHeaderClassListFile(stream, sdlEntry);
	}

	if (sourceDevicePtr != nullptr){
		QTextStream stream(sourceDevicePtr);
		ok = ok && ProcessSourceClassListFile(stream, sdlEntry);
	}

	return ok;
}


QList<imtsdl::IncludeDirective> CSdlUnionObjectGeneratorComp::GetIncludeDirectives() const
{
	return QList<imtsdl::IncludeDirective>();
}


// private methods

bool CSdlUnionObjectGeneratorComp::ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	// const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);

	if (sdlUnion == nullptr){
		return true;
	}

	FeedStream(stream, 3, false);

	QString unionTypeObjectName = QStringLiteral("%1Object").arg(sdlUnion->GetName());
	stream << QStringLiteral("class %1: public ::imtbase::CItemModelBase").arg(unionTypeObjectName);
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 0);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 0);
	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("typedef ::imtbase::CItemModelBase BaseClass;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("%1(QObject* parent = nullptr): BaseClass(parent) {}").arg(unionTypeObjectName);
	FeedStream(stream, 2, false);


	FeedStreamHorizontally(stream, 0);
	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("istd::TSharedNullable<%1> Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral(";").arg(sdlUnion->GetName());
	FeedStream(stream, 1, false);


	// Union Type end
	FeedStreamHorizontally(stream, 0);
	stream << QStringLiteral("};");
	FeedStream(stream, 2, false);

	return true;
}


bool CSdlUnionObjectGeneratorComp::ProcessHeaderClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	// const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);

	if (sdlUnion == nullptr){
		return true;
	}

	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());


	QString itemClassName = sdlNamespace + QStringLiteral("::") + sdlEntry.GetName();
	QString modelObjectDataTypeName = "QVariant"; // itemClassName + QStringLiteral("Object");
	QString objectListClassName = QStringLiteral("C") + sdlEntry.GetName() + QStringLiteral("ObjectList");
	QString objectListClassNameWithNamespace = sdlNamespace + QStringLiteral("::") + objectListClassName;


	QString unionTypeObjectName = QStringLiteral("%1Object").arg(sdlUnion->GetName());

	stream << QStringLiteral("class ") << objectListClassName;
	stream << QStringLiteral(": public ::imtsdl::TSdlAbstractListModel<");
	stream << itemClassName << QStringLiteral(", ") << modelObjectDataTypeName;
	stream << QStringLiteral(">");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_OBJECT");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("typedef ::imtsdl::TSdlAbstractListModel<");
	stream << itemClassName << QStringLiteral(", ") << modelObjectDataTypeName;
	stream << QStringLiteral("> BaseClass;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << objectListClassName;
	stream << QStringLiteral("(QObject* parent = nullptr): BaseClass(parent) {}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("virtual QVariant GetOrCreateCachedObject(int index) const override;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE int getItemsCount();");
	FeedStream(stream, 1, false);

	// get method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariantMap get(int row) const override;");
	FeedStream(stream, 1, false);
	// get method end

	// append method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void append(") << modelObjectDataTypeName << QStringLiteral(" item);");
	FeedStream(stream, 1, false);

	// append method end

	// copyMe method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE ") << objectListClassNameWithNamespace << QStringLiteral("* copyMe();");
	FeedStream(stream, 1, false);

	// copyMe method end

	// toJson method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toJson();");
	FeedStream(stream, 1, false);

	// toJson method end

	// toGraphQL method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toGraphQL();");
	FeedStream(stream, 1, false);

	// toGraphQL method end

	// addElement method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void addElement(") + modelObjectDataTypeName + QStringLiteral(" item);");
	FeedStream(stream, 1, false);

	// removeElement method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void removeElement(int index);");
	FeedStream(stream, 1, false);

	// isEqualWithModel method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool isEqualWithModel(") << objectListClassNameWithNamespace << QStringLiteral("* otherModelPtr);");
	FeedStream(stream, 1, false);


	// isEqualWithModel method end

	// insert method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void insert(int index, ") << modelObjectDataTypeName << QStringLiteral(" item);");
	FeedStream(stream, 1, false);

	// insert method end

	// remove method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void remove(int index) override;");
	FeedStream(stream, 1, false);

	// remove method end

	// clear method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void clear() override;");
	FeedStream(stream, 1, false);

	// clear method end

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("signals:");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("void countChanged();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("};");
	FeedStream(stream, 1, false);

	// stream.flush();

	return true;
}


bool CSdlUnionObjectGeneratorComp::ProcessSourceClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	// const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);

	if (sdlUnion == nullptr){
		return false;
	}

	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());


	QString itemClassName = sdlNamespace + QStringLiteral("::") + sdlEntry.GetName();
	QString modelObjectDataTypeName = "QVariant";
	QString objectListClassName = QStringLiteral("C") + sdlEntry.GetName() + QStringLiteral("ObjectList");
	QString objectListClassNameWithNamespace = sdlNamespace + QStringLiteral("::") + objectListClassName;


	QString unionTypeObjectName = QStringLiteral("%1Object").arg(sdlUnion->GetName());

	stream << QStringLiteral("QVariant ") + objectListClassName + QStringLiteral("::GetOrCreateCachedObject(int index) const");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("QVariant retVal;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (this->m_objectDataTypeMap.contains(index)){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("retVal = this->m_objectDataTypeMap[index];");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("else{");
	FeedStream(stream, 1, false);

	// const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(sdlEntryBase.get());

	if (sdlUnion != nullptr){
		QList<QString> convertedTypeList;
		for (const auto& sdlType : sdlUnion->GetTypes()){
			imtsdl::CSdlField sdlField;
			sdlField.SetType(sdlType);
			bool isCustom = false;
			QString convertedType = OptListConvertTypeWithNamespaceStruct(
				sdlField,
				GetNamespaceFromSchemaParams(sdlUnion->GetSchemaParams()),
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr,
				true,
				&isCustom,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				false);
			FeedStreamHorizontally(stream, 2);
			std::shared_ptr<imtsdl::CSdlEntryBase> entryField  = FindEntryByName(sdlField.GetType(), m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
			if (isCustom && !entryField){
				qFatal() << __func__ << "Invalid field: " << sdlType.toUtf8();
			}

			stream << QStringLiteral("if (const ") << convertedType << QStringLiteral("* val = std::get_if<") << convertedType << QStringLiteral(">(this->Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->at(index).GetPtr())){");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 3);

			if (isCustom){
				stream << convertedType << QStringLiteral("Object *newObjectPtr = new ");
				stream << convertedType << QStringLiteral("Object();");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 3);
				stream << QStringLiteral("newObjectPtr->Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral(" = val->Version_") << GetSdlEntryVersion(*entryField, false) << QStringLiteral(";");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, 3);
				stream << QStringLiteral("retVal = QVariant::fromValue(newObjectPtr);");
				FeedStream(stream, 1, false);
			}
			else{
				stream << QStringLiteral("retVal =  QVariant::fromValue(val);");
				FeedStream(stream, 1, false);
			}

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 2, false);
		}
	}

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("this->m_objectDataTypeMap.insert(index, retVal);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);


	stream << QStringLiteral("bool ") + objectListClassName + QStringLiteral("::containsKey(const QString& nameId, int /*index*/)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return nameId == \"item\";");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	stream << QStringLiteral("int ") + objectListClassName + QStringLiteral("::getItemsCount()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return rowCount();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	// get method begin
	stream << QStringLiteral("QVariantMap ") + objectListClassName + QStringLiteral("::get(int row) const");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return BaseClass::get(row);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// get method end

	// append method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::append(") << modelObjectDataTypeName << QStringLiteral(" item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	QString insertData = QStringLiteral("Version_") + GetSdlEntryVersion(sdlEntry, false) + QStringLiteral("->append(newItem);");
	ProcessInsertToList(stream, sdlEntry, insertData.toUtf8());

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// append method end

	// copyMe method begin
	stream << objectListClassNameWithNamespace << QStringLiteral("* ") + objectListClassName + QStringLiteral("::copyMe()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << objectListClassNameWithNamespace << QStringLiteral("* objectListPtr = new ") << objectListClassNameWithNamespace << QStringLiteral("();");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariant item = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);


	for (const auto& sdlType : sdlUnion->GetTypes()){
		imtsdl::CSdlField sdlField;
		sdlField.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = OptListConvertTypeWithNamespaceStruct(
			sdlField,
			GetNamespaceFromSchemaParams(sdlUnion->GetSchemaParams()),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			true,
			&isCustom,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			false);

		QString objectConvertedType = convertedType;
		if (isCustom){
			objectConvertedType += "Object*";
		}
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("if (item.canConvert<") << objectConvertedType << QStringLiteral(">()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 3);
		stream << objectConvertedType << QStringLiteral(" val = ") << QStringLiteral("item.value<") << objectConvertedType << QStringLiteral(">();");
		FeedStream(stream, 1, false);

		if (isCustom){
			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("objectListPtr->addElement(QVariant::fromValue(val->copyMe()));");
			FeedStream(stream, 1, false);
		}
		else{
			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("objectListPtr->addElement(val);");
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return objectListPtr;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// copyMe method end

	// toJson method begin
	stream << QStringLiteral("QString ") + objectListClassName + QStringLiteral("::toJson()");
	ProcessConvertListData(stream, sdlEntry, "Json");
	FeedStream(stream, 2, false);
	// toJson method end

	// toGraphQL method begin
	stream << QStringLiteral("QString ") + objectListClassName + QStringLiteral("::toGraphQL()");
	ProcessConvertListData(stream, sdlEntry, "GraphQL");
	FeedStream(stream, 2, false);
	// toGraphQL method end

	// addElement method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::addElement(") + modelObjectDataTypeName + QStringLiteral(" item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("append(item);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// addElement method end

	// removeElement method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::removeElement(int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("remove(index);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// removeElement method end

	// isEqualWithModel method begin
	stream << QStringLiteral("bool ") + objectListClassName + QStringLiteral("::isEqualWithModel(") << objectListClassNameWithNamespace << QStringLiteral("* otherModelPtr)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (otherModelPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (this == otherModelPtr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (this->rowCount() != otherModelPtr->rowCount()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariant selfItem = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariant otherItem = otherModelPtr->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (selfItem != otherItem){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return true;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// isEqualWithModel method end

	// insert method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::insert(int index, ") << modelObjectDataTypeName << QStringLiteral(" item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (index < 0 || index > Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->size()) return;");
	FeedStream(stream, 1, false);

	insertData = QStringLiteral("Version_") + GetSdlEntryVersion(sdlEntry, false).toUtf8() + QStringLiteral("->insert(index, newItem);");
	ProcessInsertToList(stream, sdlEntry, insertData.toUtf8());

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// insert method end

	// remove method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::remove(int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("BaseClass::remove(index);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// remove method end

	// clear method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::clear()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("BaseClass::clear();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// clear method end

	stream << QStringLiteral("QVariant ") + objectListClassName + QStringLiteral("::getData(const QString& nameId, int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("if (nameId == \"item\" && Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral(".has_value() && index >= 0 && index < Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->count()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariant retVal = GetOrCreateCachedObject(index);");
	FeedStream(stream, 1, false);


	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return QVariant();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);


	// stream.flush();

	return true;
}


bool CSdlUnionObjectGeneratorComp::ProcessInsertToList(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry, const QByteArray& /*insertData*/) const
{
	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);

	if (sdlUnion == nullptr){
		return false;
	}

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("beginInsertRows(QModelIndex(), Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->count(), 0);");
	FeedStream(stream, 1, false);

	for (const auto& sdlType : sdlUnion->GetTypes()){
		imtsdl::CSdlField sdlField;
		sdlField.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = OptListConvertTypeWithNamespaceStruct(
			sdlField,
			GetNamespaceFromSchemaParams(sdlUnion->GetSchemaParams()),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			true,
			&isCustom,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			false);

		QString objectConvertedType = convertedType;
		if (isCustom){
			objectConvertedType += "Object*";
		}

		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("if (item.canConvert<") << objectConvertedType << QStringLiteral(">()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << objectConvertedType << QStringLiteral(" val = item.value<") << objectConvertedType << QStringLiteral(">();");
		FeedStream(stream, 1, false);

		if (isCustom){
			FeedStreamHorizontally(stream, 2);
			stream << convertedType << QStringLiteral("* newItemPtr = dynamic_cast<") << convertedType << QStringLiteral("*>(val);");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->append(istd::TSharedNullable<") << sdlUnion->GetName() << QStringLiteral(">(*newItemPtr));");
			FeedStream(stream, 1, false);
		}
		else{
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->append(istd::TSharedNullable<") << sdlUnion->GetName() << QStringLiteral(">(val));");
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}

	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("ClearCache();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("endInsertRows();");
	FeedStream(stream, 1, false);

	return true;
}


bool CSdlUnionObjectGeneratorComp::ProcessConvertListData(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry, const QByteArray& convertFormat) const
{
	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);

	if (sdlUnion == nullptr){
		return false;
	}

	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("QString retVal = QStringLiteral(\"[\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (i > 0 && i < this->rowCount() - 1){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("retVal += QStringLiteral(\", \");");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariant item = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	for (const auto& sdlType : sdlUnion->GetTypes()){
		imtsdl::CSdlField sdlField;
		sdlField.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = OptListConvertTypeWithNamespaceStruct(
			sdlField,
			GetNamespaceFromSchemaParams(sdlUnion->GetSchemaParams()),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			true,
			&isCustom,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			false);

		QString objectConvertedType = convertedType;
		if (isCustom){
			objectConvertedType += "Object*";
		}
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("if (item.canConvert<") << objectConvertedType << QStringLiteral(">()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 3);
		stream << objectConvertedType << QStringLiteral(" val = ") << QStringLiteral("item.value<") << objectConvertedType << QStringLiteral(">();");
		FeedStream(stream, 1, false);

		if (isCustom){
			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("retVal += val->to") << convertFormat  << QStringLiteral("();");
			FeedStream(stream, 1, false);
		}
		else{
			FeedStreamHorizontally(stream, 3);
			if (convertedType == "double" || convertedType == "int"){
				stream << QStringLiteral("retVal += QString::number(val);");
			}
			else if (convertedType == "bool"){
				stream << QStringLiteral("retVal += QString(val ? \"true\" : \"false\");");
			}
			else{
				stream << QStringLiteral("retVal += QString(val);");
			}
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);


	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("retVal += QStringLiteral(\"]\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	return true;
}


} // namespace imtsdlgencpp

