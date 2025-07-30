#include <imtsdlgenv2/CSdlQObjectGenerator.h>


// Qt includes
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


CSdlQObjectGenerator::CSdlQObjectGenerator(const imtsdl::ISdlEnumListProvider& enumListProvider,
	const imtsdl::ISdlUnionListProvider& unionListProvider, const imtsdl::ISdlTypeListProvider &typeListProvider):
		m_enumListProvider(enumListProvider),
		m_unionListProvider(unionListProvider),
		m_typeListProvider(typeListProvider)
{

}

bool CSdlQObjectGenerator::ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType)
{
	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());

	stream << QStringLiteral("class C") << sdlType.GetName();
	stream << QStringLiteral("ObjectList;");
	FeedStream(stream, 2, false);

	// class begin
	stream << QStringLiteral("class C") << sdlType.GetName();
	stream << QStringLiteral("Object: public ::imtbase::CItemModelBase, public ");
	stream << QStringLiteral("C") << sdlType.GetName();
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_OBJECT");
	FeedStream(stream, 1, false);

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	// class Q_PROPERTY
	const imtsdl::SdlFieldList fieldList =  sdlType.GetFields();
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_PROPERTY(") + GetQObjectTypeName(field); // QVariant");
		stream << QStringLiteral(" m_") << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral(" READ Get") << GetCapitalizedValue(field.GetId());
		stream << QStringLiteral(" WRITE Set") << GetCapitalizedValue(field.GetId());
		stream << QStringLiteral(" NOTIFY ") << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Changed)");
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("typedef ::imtbase::CItemModelBase BaseClass;");
	FeedStream(stream, 1, false);

	FeedStream(stream, 1, false);
	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("C") << sdlType.GetName() << QStringLiteral("Object");;
	stream << QStringLiteral("(QObject* parent = nullptr);");
	FeedStream(stream, 2, false);

	// class Getters and Setters

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

		FeedStreamHorizontally(stream);
		stream << GetQObjectTypeName(field);
		stream << QStringLiteral(" Get") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("void Set") << GetCapitalizedValue(field.GetId());
		stream << QStringLiteral("(") << GetQObjectTypeName(field);
		stream << QStringLiteral(" v);");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_INVOKABLE bool has") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
		FeedStream(stream, 1, false);

		if (isCustom && !isEnum){
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("Q_INVOKABLE void create") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
			FeedStream(stream, 1, false);
		}
	}

	// CItemModelBase implemented
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("// CItemModelBase implemented");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toJson() const override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE virtual bool createFromJson(const QString& json) override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toGraphQL() const override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QObject* CreateObject(const QString& key) override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;");
	FeedStream(stream, 1, false);

	// class signals
	FeedStream(stream, 1, false);
	stream << QStringLiteral("signals:");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);		FeedStreamHorizontally(stream);
		stream << QStringLiteral("void ") << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Changed();");
		FeedStream(stream, 1, false);
	}
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("void finished();");
	FeedStream(stream, 1, false);

	// protected class data
	FeedStream(stream, 1, false);
	stream << QStringLiteral("protected:");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
		if (isCustom && !isEnum){
			FeedStreamHorizontally(stream);
			stream << GetQObjectTypeName(field);
			stream << QStringLiteral(" m_") << GetDecapitalizedValue(field.GetId());
			stream << QStringLiteral("QObjectPtr;");
			FeedStream(stream, 1, false);
		}
	}

	// end of class
	stream << QStringLiteral("};");
	FeedStream(stream, 3, false);

	QString itemClassName = sdlNamespace + QStringLiteral("::C") + sdlType.GetName();
	QString modelDataTypeName = itemClassName + QStringLiteral("::V1_0");
	QString modelObjectDataTypeName = itemClassName + QStringLiteral("Object");
	QString objectListClassName = QStringLiteral("C") + sdlType.GetName() + QStringLiteral("ObjectList");
	QString objectListClassNameWithNamespace = sdlNamespace + QStringLiteral("::") + objectListClassName;

	stream << QStringLiteral("class ") << objectListClassName;
	stream << QStringLiteral(": public ::imtbase::TListModelBase<");
	stream << modelDataTypeName << QStringLiteral(", ") << modelObjectDataTypeName;
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
	stream << QStringLiteral("typedef ::imtbase::TListModelBase<");
	stream << modelDataTypeName << QStringLiteral(", ") << modelObjectDataTypeName;
	stream << QStringLiteral("> BaseClass;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << objectListClassName;
	stream << QStringLiteral("(QObject* parent = nullptr): BaseClass(parent) {}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return true;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE int getItemsCount(){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return rowCount();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	// get method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariantMap get(int row) const{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QVariantMap data;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QModelIndex idx = index(row, 0);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (!idx.isValid()) return data;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QHash<int, QByteArray> roles = roleNames();");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("for (auto it = roles.begin(); it != roles.end(); ++it)");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("data[it.value()] = idx.data(it.key());");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return data;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	// get method end

	// append method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void append(") << modelObjectDataTypeName << QStringLiteral("* item){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("beginInsertRows(QModelIndex(), rowCount(), rowCount());");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("Version_1_0->append(*item->Version_1_0);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("ClearCache();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("endInsertRows();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	// append method end

	// copyMe method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE ") << objectListClassNameWithNamespace << QStringLiteral("* copyMe(){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << objectListClassNameWithNamespace << QStringLiteral("* objectListPtr = new ") << objectListClassNameWithNamespace << QStringLiteral("();");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("QVariant item = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (!item.canConvert<") + modelObjectDataTypeName + QStringLiteral(">()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream <<  modelObjectDataTypeName + QStringLiteral("* itemObjectPtr = item.value<") + modelObjectDataTypeName + QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (itemObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("objectListPtr->addElement(dynamic_cast<") + modelObjectDataTypeName + QStringLiteral("*>(itemObjectPtr->copyMe()));");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return objectListPtr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// copyMe method end

	// toJson method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toJson(){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QString retVal = QStringLiteral(\"[\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (i > 0 && i < this->rowCount() - 1){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("retVal += QStringLiteral(\", \");");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("QVariant item = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (!item.canConvert<") + modelObjectDataTypeName + QStringLiteral(">()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << modelObjectDataTypeName + QStringLiteral("* itemObjectPtr = item.value<") + modelObjectDataTypeName + QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (itemObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return QString();");
	FeedStream(stream, 1, false);
	
	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("retVal += itemObjectPtr->toJson();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("retVal += QStringLiteral(\"]\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// toJson method end

	// toGraphQL method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toGraphQL(){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("QString retVal = QStringLiteral(\"[\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (i > 0 && i < this->rowCount() - 1){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("retVal += QStringLiteral(\", \");");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("QVariant item = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (!item.canConvert<") + modelObjectDataTypeName + QStringLiteral(">()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << modelObjectDataTypeName + QStringLiteral("* itemObjectPtr = item.value<") + modelObjectDataTypeName + QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (itemObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return QString();");
	FeedStream(stream, 1, false);
	
	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("retVal += itemObjectPtr->toGraphQL();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("retVal += QStringLiteral(\"]\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// toGraphQL method end

	// addElement method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void addElement(") + modelObjectDataTypeName + QStringLiteral("* item){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("append(item);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// addElement method end

	// removeElement method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void removeElement(int index){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("remove(index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);
	// removeElement method end

	// isEqualWithModel method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool isEqualWithModel(") << objectListClassNameWithNamespace << QStringLiteral("* otherModelPtr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (otherModelPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (this == otherModelPtr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (this->rowCount() != otherModelPtr->rowCount()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("for (int i = 0; i < this->rowCount(); i++){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("QVariant selfItem = this->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("QVariant otherItem = otherModelPtr->getData(\"item\", i);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (!selfItem.canConvert<") + modelObjectDataTypeName + QStringLiteral(">()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << modelObjectDataTypeName + QStringLiteral("* selfItemObjectPtr = selfItem.value<") + modelObjectDataTypeName + QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (selfItemObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream <<  modelObjectDataTypeName + QStringLiteral("* otherItemObjectPtr = selfItem.value<") + modelObjectDataTypeName + QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (otherItemObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 4);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return true;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	FeedStream(stream, 1, false);
	// isEqualWithModel method end

	// insert method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void insert(int index, ") << modelObjectDataTypeName << QStringLiteral("* item){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (index < 0 || index > Version_1_0->size()) return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("beginInsertRows(QModelIndex(), index, index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("Version_1_0->insert(index, *item->Version_1_0);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("ClearCache();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("endInsertRows();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	// insert method end

	// remove method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void remove(int index){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (index < 0 || index >= Version_1_0->size()) return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("beginRemoveRows(QModelIndex(), index, index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("Version_1_0->removeAt(index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("ClearCache();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("endRemoveRows();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	// remove method end

	// clear method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void clear(){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("beginResetModel();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("ClearCache();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("Version_1_0->clear();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("endResetModel();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);
	// clear method end

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariant getData(const QString& nameId, int index){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (nameId == \"item\" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << modelObjectDataTypeName << QStringLiteral("* retVal = GetOrCreateCachedObject(index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return QVariant::fromValue(retVal);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
		// if (isCustom && !isEnum){
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (nameId == \"m_") << GetDecapitalizedValue(field.GetId());
			stream << QStringLiteral("\"){");
			FeedStream(stream, 1, false);

			if (!isCustom || isEnum){
				FeedStreamHorizontally(stream, 3);
				stream << QStringLiteral("return QVariant::fromValue(Version_1_0.GetPtr()->at(index).");
				stream << field.GetId() << QStringLiteral(".value());");
				FeedStream(stream, 1, false);
			}
			else{
				// FeedStreamHorizontally(stream, 3);
				// stream << modelObjectDataTypeName << QStringLiteral("* retVal = GetOrCreateCachedObject(index);");
				// FeedStream(stream, 1, false);
				
				FeedStreamHorizontally(stream, 3);
				stream << sdlNamespace << QStringLiteral("::C") << sdlType.GetName();
				stream << QStringLiteral("Object* retVal = GetOrCreateCachedObject(index);");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 3);
				stream << QStringLiteral("return QVariant::fromValue(retVal->Get");
				stream << GetCapitalizedValue(field.GetId()) << QStringLiteral("());");
				FeedStream(stream, 1, false);
			}

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);
		// }
	}
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return QVariant();");
	// if ( && !isEnum){
		// stream << QStringLiteral("return Version_1_0.GetPtr()->at(index).property(nameId.toUtf8());"); //Version_1_0.GetPtr()->at(index)
	// }
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
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


bool CSdlQObjectGenerator::ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType)
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
	const imtsdl::SdlFieldList fieldList =  sdlType.GetFields();

	const QString className = QStringLiteral("C") + sdlType.GetName() + QStringLiteral("Object");
	stream << className;
	stream << QStringLiteral("::")<< className;
	stream << QStringLiteral("(QObject* parent): ::imtbase::CItemModelBase(parent)");

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
		if (isCustom && !isEnum){
			FeedStreamHorizontally(stream,3);
			stream << QStringLiteral(", m_") << GetDecapitalizedValue(field.GetId());
			stream << QStringLiteral("QObjectPtr(nullptr)");
			FeedStream(stream, 1, false);
		}
	}

	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Version_1_0.emplace();");
	FeedStream(stream, 1, false);

	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: fieldList){
		QString signalName = className + QStringLiteral("::") + GetDecapitalizedValue(field.GetId()) + QStringLiteral("Changed");
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("QObject::connect(this, &") << signalName << QStringLiteral(", this, &CItemModelBase::OnInternalModelChanged);");
		FeedStream(stream, 1, false);
	}

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	// class Q_PROPERTY
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

			// Getter implemented

			stream << GetQObjectTypeName(field);
			stream << QStringLiteral(" C") << sdlType.GetName() << QStringLiteral("Object");
			stream << QStringLiteral("::Get") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (Version_1_0->") << field.GetId() << QStringLiteral(".has_value()){");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 2);

			std::shared_ptr<imtsdl::CSdlEntryBase> sdlEntryBase = imtsdl::CSdlTools::GetSdlTypeOrEnumOrUnionForField(field, m_typeListProvider.GetSdlTypes(false), m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false));
			QString fieldNameSpace;
			if (sdlEntryBase != nullptr){
				fieldNameSpace = GetNamespaceFromSchemaParams(sdlEntryBase->GetSchemaParams());
			}

			if (isEnum){
				if (isArray){

				}
				else{
					stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(" valueType = Version_1_0->");
					stream << field.GetId() << QStringLiteral(".value();");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("QMetaEnum metaEnum = QMetaEnum::fromType<");
					stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(">();");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("QString retval = metaEnum.valueToKey((int)valueType);");
					FeedStream(stream, 2, false);
					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("return retval;");
				}
			}
			else {
				if (!isCustom){
					if (isArray && field.GetType() == "ID"){
						QString tempVariableName = QStringLiteral("temp") + GetCapitalizedValue(field.GetId()) + QStringLiteral("List");

						stream << QStringLiteral("QStringList ") << tempVariableName << ';';
						FeedStream(stream, 1, false);

						FeedStreamHorizontally(stream, 2);
						stream << QStringLiteral("for (const auto& tempValue: Version_1_0->");
						stream << field.GetId();
						stream << QStringLiteral(".value()){");
						FeedStream(stream, 1, false);

						FeedStreamHorizontally(stream, 3);
						stream << tempVariableName << QStringLiteral(" << tempValue;");
						FeedStream(stream, 1, false);

						FeedStreamHorizontally(stream, 2);
						stream << '}';
						FeedStream(stream, 1, false);
						FeedStreamHorizontally(stream, 2);
						stream << QStringLiteral("return ") << tempVariableName << QStringLiteral(";");
					}
					else {
						stream << QStringLiteral("return Version_1_0->") << field.GetId() << QStringLiteral(".value();");
					}
				}
				else{
					stream << QStringLiteral("if (!m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr){");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 3);
					stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr = dynamic_cast<") << GetQObjectTypeName(field, true);
					stream << QStringLiteral(">(CreateObject(\"") << field.GetId() << QStringLiteral("\"));");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 3);
					stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr->Version_1_0 = Version_1_0->") << field.GetId() << ';';
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("}");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("return m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr;");
				}

			}

			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("}");

			FeedStream(stream, 2, false);

			FeedStreamHorizontally(stream);
			if (isCustom && !isEnum){
				stream << QStringLiteral("return nullptr;");
			}
			else{
				stream << QStringLiteral("return ");
				if (field.GetType() == "String" || isEnum){
					if (isArray){
						stream << QStringLiteral("QStringList();");
					}
					else{
						stream << QStringLiteral("QString();");
					}
				}
				else if (field.GetType() == "Integer" || field.GetType() == "Int"){
					if (isArray){
						stream << QStringLiteral("QList<int>();");
					}
					else{
						stream << QStringLiteral("0;");
					}
				}
				else if (field.GetType() == "LongLong" || field.GetType() == "longLong"){
					if (isArray){
						stream << QStringLiteral("QList<int>();");
					}
					else{
						stream << QStringLiteral("0;");
					}
				}
				else if (field.GetType() == "Double"){
					if (isArray){
						stream << QStringLiteral("QList<double>();");
					}
					else{
						stream << QStringLiteral("0;");
					}
				}
				else if (field.GetType() == "Boolean" || field.GetType() == "Bool"){
					if (isArray){
						stream << QStringLiteral("QList<bool>();");
					}
					else{
						stream << QStringLiteral("false;");
					}
				}
				else if (field.GetType() == "ID"){
					if (isArray){
						stream << QStringLiteral("QStringList();");
					}
					else{
						stream << QStringLiteral("QString();");
					}
				}
				else {
					Q_ASSERT_X(false, "CSdlQObjectGenerator::ProcessSourceClassFile", field.GetType().toUtf8() + " field.GetType() not implemented");
				}
			}
			FeedStream(stream, 1, false);
			stream << QStringLiteral("}");
			FeedStream(stream, 3, false);

			// Setter implemented
			stream << QStringLiteral("void C") << sdlType.GetName() << QStringLiteral("Object");
			stream << QStringLiteral("::Set") << GetCapitalizedValue(field.GetId());
			stream << QStringLiteral("(") << GetQObjectTypeName(field);
			stream << QStringLiteral(" v)");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			QString tempVariableName = QStringLiteral("temp") + GetCapitalizedValue(field.GetId()) + QStringLiteral("List");
			if (isArray && field.GetType() == "ID"){
				stream << QStringLiteral("Version_1_0->") << field.GetId() << QStringLiteral(" = QList<QByteArray>(); ");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 1);
				stream << QStringLiteral("for (const auto& tempValue: v"); // << tempVariableName;
				stream << QStringLiteral("){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("Version_1_0->") << field.GetId() << QStringLiteral("->append(tempValue.toUtf8());");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 1);
				stream << '}';
				FeedStream(stream, 1, false);
			}
			else if (isEnum){
				if (isArray){

				}
				else{
					stream << QStringLiteral("Version_1_0->") << field.GetId() << QStringLiteral(".emplace();");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 1);
					stream << QStringLiteral("QMetaEnum metaEnum = QMetaEnum::fromType<");
					stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(">();");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 1);
					stream << QStringLiteral("int key = metaEnum.keyToValue(v.toUtf8());");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 1);
					stream << QStringLiteral("if (key > -1){");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("Version_1_0->") << field.GetId() << QStringLiteral(" = (");
					stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(")key;");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 1);
					stream << '}';
				}
			}
			else if (!isCustom){
				stream << QStringLiteral("Version_1_0->") << field.GetId();
				if (field.GetType() == "ID"){
					stream << QStringLiteral(" = v.toUtf8();");
				}
				else{
					stream << QStringLiteral(" = v;");
				}
			}
			else{
				stream << QStringLiteral("if (v){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("Version_1_0->") << field.GetId();
				stream << QStringLiteral(" = v->Version_1_0;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
				stream << QStringLiteral("QObjectPtr = v;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream);
				stream << QStringLiteral("else {");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("Version_1_0->") << field.GetId();
				stream << QStringLiteral(" = nullptr;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);
			}
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Changed();");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("}");
			FeedStream(stream, 3, false);

			// Has implemented
			stream << QStringLiteral("bool C") << sdlType.GetName() << QStringLiteral("Object");
			stream << QStringLiteral("::has") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral(" return Version_1_0->") << field.GetId() << QStringLiteral(".HasValue();");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("}");
			FeedStream(stream, 3, false);

			// Create property implemented
			if (isCustom && !isEnum){
				stream << QStringLiteral("void C") << sdlType.GetName() << QStringLiteral("Object");
				stream << QStringLiteral("::create") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
				FeedStream(stream, 1, false);
				stream << QStringLiteral("{");

				FeedStreamHorizontally(stream);
				stream << QStringLiteral("Version_1_0->") << field.GetId() << QStringLiteral(".emplace();");
				FeedStream(stream, 1, false);

				FeedStream(stream, 1, false);
				stream << QStringLiteral("}");
				FeedStream(stream, 3, false);
			}

	}

	// CItemModelBase implemented

	stream << QStringLiteral("QString C") << sdlType.GetName();
	stream << QStringLiteral("Object::toJson() const");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QJsonObject jsonObject;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool res = WriteToJsonObject(jsonObject);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("if (res){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream,2);
	stream << QStringLiteral("QJsonDocument document;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream,2);
	stream << QStringLiteral("document.setObject(jsonObject);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream,2);
	stream << QStringLiteral("return document.toJson(QJsonDocument::Compact);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return QString();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	// formJson implemented

	stream << QStringLiteral("bool C") << sdlType.GetName();
	stream << QStringLiteral("Object::createFromJson(const QString& json)");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return fromObject(document.object());");
	FeedStream(stream, 1, false);

	FeedStream(stream, 1, false);
	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	stream << QStringLiteral("bool C") << sdlType.GetName();
	stream << QStringLiteral("Object::fromObject(const QJsonObject& jsonObject)");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("beginChanges();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool res = ReadFromJsonObject(jsonObject);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("if (res){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream,2);
	stream << QStringLiteral("QVariantList changelist;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream,2);
	stream << QStringLiteral("modelChanged(changelist);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream,2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("endChanges();");
	FeedStream(stream,2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("finished();");
	FeedStream(stream,2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return res;");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	// toGraphQL implemented

	stream << QStringLiteral("QString C") << sdlType.GetName();
	stream << QStringLiteral("Object::toGraphQL() const");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::toGraphQL();");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	stream << QStringLiteral("QObject* C") << sdlType.GetName();
	stream << QStringLiteral("Object::CreateObject(const QString& key)");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
		if (isCustom && !isEnum){
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (key == \"") << field.GetId() << QStringLiteral("\"){");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return new ") << GetQObjectTypeName(field, false) << QStringLiteral("(this);");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);
		}
	}

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	stream << QStringLiteral("QString C") << sdlType.GetName();
	stream << QStringLiteral("Object::getJSONKeyForProperty(const QString& propertyName) const");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: fieldList){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (propertyName == (QString(\"m_\") + \"");
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("\")){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return \"") << field.GetId() << QStringLiteral("\";");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return propertyName;");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	return true;
}


QString CSdlQObjectGenerator::GetQObjectTypeName(const imtsdl::CSdlField& field, bool withPointer)
{
	QString retVal = "";
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_enumListProvider.GetEnums(false), m_unionListProvider.GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
	std::shared_ptr<imtsdl::CSdlEntryBase> sdlEntryBase = GetSdlTypeOrEnumOrUnionForField(field,
																						  m_typeListProvider.GetSdlTypes(false),
																						  m_enumListProvider.GetEnums(false),
																						  m_unionListProvider.GetUnions(false));

	QString sdlNamespace;
	if (sdlEntryBase != nullptr){
		sdlNamespace = GetNamespaceFromSchemaParams(sdlEntryBase->GetSchemaParams());
	}

	if (!isCustom || isEnum){
		if (isArray){
			retVal = QStringLiteral("QList<");
		}

		if (field.GetType() == "String" || isEnum){
			retVal += QStringLiteral("QString");
		}
		else if (field.GetType() == "Integer" || field.GetType() == "Int"){
			retVal += QStringLiteral("int");
		}
		else if (field.GetType() == "Double"){
			retVal += QStringLiteral("double");
		}
		else if (field.GetType() == "Boolean" || field.GetType() == "Bool"){
			retVal += QStringLiteral("bool");
		}
		else if (field.GetType() == "LongLong" || field.GetType() == "longLong"){
			retVal += QStringLiteral("int");
		}
		else if (field.GetType() == "ID"){
			retVal += QStringLiteral("QString");
		}
		else {
			Q_ASSERT_X(false, "CSdlQObjectGenerator::ProcessSourceClassFile", field.GetType().toUtf8() + " field.GetType() not implemented");
		}

		if (isArray){
			retVal += QStringLiteral(">");
		}
	}
	else if(isArray){

		retVal = sdlNamespace;
		retVal += QStringLiteral("::C") + field.GetType();
		retVal += QStringLiteral("ObjectList");

		if (withPointer){
			retVal += "*";
		}
	}
	else{
		retVal = sdlNamespace;
		retVal += QStringLiteral("::C") + field.GetType();
		retVal += QStringLiteral("Object");

		if (withPointer){
			retVal += "*";
		}
	}

	return retVal;
}


} // namespace imtsdlgenv2

