// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CSdlQObjectGeneratorComp.h>


// Qt includes
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlEntryBase.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlField.h>


namespace imtsdlgencpp
{

// public methods


bool CSdlQObjectGeneratorComp::ProcessEntry(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* headerDevicePtr, QIODevice* sourceDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());

	bool ok = true;
	if (headerDevicePtr != nullptr){
		QTextStream stream(headerDevicePtr);
		ok = ok && ProcessHeaderClassFile(stream, sdlEntry);
		ok = ok && ProcessHeaderClassListFile(stream, sdlEntry);
	}

	if (sourceDevicePtr != nullptr){
		QTextStream stream(sourceDevicePtr);
		ok = ok && ProcessSourceClassFile(stream, sdlEntry);
		ok = ok && ProcessSourceClassListFile(stream, sdlEntry);
	}

	return ok;
}


QList<imtsdl::IncludeDirective> CSdlQObjectGeneratorComp::GetIncludeDirectives() const
{
	return {};
}


// private methods

bool CSdlQObjectGeneratorComp::ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	const auto* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr){
		return true;
	}

	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());

	stream << QStringLiteral("class C") << sdlEntry.GetName();
	stream << QStringLiteral("ObjectList;");
	FeedStream(stream, 2, false);

	// class begin
	stream << QStringLiteral("class C") << sdlEntry.GetName();
	stream << QStringLiteral("Object: public ::imtbase::CItemModelBase, public ");
	stream << QStringLiteral("C") << sdlEntry.GetName();
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
	const imtsdl::SdlFieldList fieldList =  sdlTypePtr->GetFields();
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(
			field,
			m_sdlEnumListCompPtr->GetEnums(false),
			m_sdlUnionListCompPtr->GetUnions(false),
			&isCustom,
			nullptr,
			&isArray,
			&isEnum,
			&isUnion);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_PROPERTY(QVariant");
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
	stream << QStringLiteral("C") << sdlEntry.GetName() << QStringLiteral("Object");;
	stream << QStringLiteral("(QObject* parent = nullptr);");
	FeedStream(stream, 2, false);

	// class Getters and Setters
	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("QVariant");
		stream << QStringLiteral(" Get") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("void Set") << GetCapitalizedValue(field.GetId());
		stream << QStringLiteral("(const QVariant& v);");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_INVOKABLE bool has") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
		FeedStream(stream, 1, false);

		if (isCustom && !isEnum){
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("Q_INVOKABLE void emplace") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("void Reset") << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
			FeedStream(stream, 1, false);
		}

		if (isCustom && !isEnum && isArray && !isUnion){
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("Q_INVOKABLE QVariant create") << GetCapitalizedValue(field.GetId()) << QStringLiteral("ArrayElement(const QVariant& v);");
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
	stream << QStringLiteral("Q_INVOKABLE QVariant CreateObject(const QString& key) override;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;");
	FeedStream(stream, 1, false);

	// class signals
	FeedStream(stream, 1, false);
	stream << QStringLiteral("signals:");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: fieldList){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("void ") << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Changed();");
		FeedStream(stream, 1, false);
	}

	// protected class data
	if (!fieldList.isEmpty()){
		FeedStream(stream, 1, false);
		stream << QStringLiteral("protected:");
		FeedStream(stream, 1, false);
		for (const imtsdl::CSdlField& field: fieldList){
			const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
			if (isCustom && !isEnum){
				FeedStreamHorizontally(stream);
				stream << QStringLiteral("QVariant");
				stream << QStringLiteral(" m_") << GetDecapitalizedValue(field.GetId());
				stream << QStringLiteral("QObjectPtr;");
				FeedStream(stream, 1, false);
			}
		}
	}

	// end of class
	stream << QStringLiteral("};");
	FeedStream(stream, 3, false);

	return true;
}


bool CSdlQObjectGeneratorComp::ProcessHeaderClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	const auto* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr){
		return true;
	}

	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());
	QString itemClassName = sdlNamespace + QStringLiteral("::C") + sdlEntry.GetName();
	QString modelDataTypeName = itemClassName + QStringLiteral("::V") + GetSdlEntryVersion(sdlEntry, false);
	QString modelObjectDataTypeName = itemClassName + QStringLiteral("Object");
	QString objectListClassName = QStringLiteral("C") + sdlEntry.GetName() + QStringLiteral("ObjectList");
	QString objectListClassNameWithNamespace = sdlNamespace + QStringLiteral("::") + objectListClassName;

	stream << QStringLiteral("class ") << objectListClassName;
	stream << QStringLiteral(": public ::imtsdl::TListModelBase<");
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
	stream << QStringLiteral("typedef ::imtsdl::TListModelBase<");
	stream << modelDataTypeName << QStringLiteral(", ") << modelObjectDataTypeName;
	stream << QStringLiteral("> BaseClass;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("explicit ");
	stream << objectListClassName;
	stream << QStringLiteral("(QObject* parent = nullptr): BaseClass(parent) {}");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE int getItemsCount() const;");
	FeedStream(stream, 1, false);


	// get method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariantMap get(int row) const override;");
	FeedStream(stream, 1, false);

	// append method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void append(") << modelObjectDataTypeName << QStringLiteral("* item);");
	FeedStream(stream, 1, false);

	// append method

	// copyMe method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE ") << objectListClassNameWithNamespace << QStringLiteral("* copyMe();");
	FeedStream(stream, 1, false);


	// toJson method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toJson();");
	FeedStream(stream, 1, false);

	// toGraphQL method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QString toGraphQL();");
	FeedStream(stream, 1, false);

	// addElement method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void addElement(") + modelObjectDataTypeName + QStringLiteral("* item);");
	FeedStream(stream, 1, false);

	// removeElement method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void removeElement(int index);");
	FeedStream(stream, 1, false);

	// isEqualWithModel method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE bool isEqualWithModel(") << objectListClassNameWithNamespace << QStringLiteral("* otherModelPtr);");
	FeedStream(stream, 1, false);

	// insert method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void insert(int index, ") << modelObjectDataTypeName << QStringLiteral("* item);");
	FeedStream(stream, 1, false);

	// remove method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void remove(int index) override;");
	FeedStream(stream, 1, false);

	// clear method begin
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE void clear() override;");
	FeedStream(stream, 1, false);


	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;");
	FeedStream(stream, 2, false);

	stream << QStringLiteral("signals:");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("void countChanged();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("};");
	FeedStream(stream, 1, false);

	return true;
}


bool CSdlQObjectGeneratorComp::ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	const auto* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr){
		return true;
	}

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());
	const imtsdl::SdlFieldList fieldList =  sdlTypePtr->GetFields();
	const QString versionVariableName = QStringLiteral("Version_") + GetSdlEntryVersion(sdlEntry, false);

	const QString className = QStringLiteral("C") + sdlEntry.GetName() + QStringLiteral("Object");
	stream << className;
	stream << QStringLiteral("::")<< className;
	stream << QStringLiteral("(QObject* parent): ::imtbase::CItemModelBase(parent)");

	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << versionVariableName << QStringLiteral(".emplace();");
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
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

		// Getter implemented
		std::shared_ptr<imtsdl::CSdlEntryBase> sdlEntryField  = FindEntryByName(field.GetType(), m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
		if (isCustom && !sdlEntryField){
			qFatal() <<  __func__ << "Invalid field: " << field.GetType();
		}
		QString typeName = GetQObjectTypeName(field, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
		stream << QStringLiteral("QVariant");
		stream << QStringLiteral(" C") << sdlEntry.GetName() << QStringLiteral("Object");
		stream << QStringLiteral("::Get") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
		FeedStream(stream, 1, false);

		stream << QStringLiteral("{");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << versionVariableName;
		stream << QStringLiteral(" && ") << versionVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		QString fieldNameSpace;
		if (sdlEntryField != nullptr){
			fieldNameSpace = GetNamespaceFromSchemaParams(sdlEntryField->GetSchemaParams());
		}

		if (isEnum){
			if (isArray){

			}
			else{
				stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(" valueType = ") << versionVariableName << QStringLiteral("->");
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
				if (isArray){
					QString tempVariableName = QStringLiteral("temp") + GetCapitalizedValue(field.GetId()) + QStringLiteral("List");

					stream << typeName << ' ' << tempVariableName << ';';
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("for (const auto& tempValue: ") << versionVariableName << QStringLiteral("->");
					stream << field.GetId();
					stream << QStringLiteral(".value()){");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 3);
					stream << tempVariableName << QStringLiteral(" << *tempValue;");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 2);
					stream << '}';
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 2);
					stream << QStringLiteral("return QVariant::fromValue(") << tempVariableName << QStringLiteral(");");
				}
				else {
					stream << QStringLiteral("return ") << versionVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral(".value();");
				}
			}
			else{
				if (isUnion && !isArray){
					stream << QStringLiteral("if (m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr.isValid()){");
					FeedStream(stream, 1, false);
					const auto* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(sdlEntryField.get());

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
							FeedStreamHorizontally(stream, 3);
							const QString sourceVariableName = QStringLiteral("Version_") + GetSdlEntryVersion(sdlEntry, false) + QStringLiteral("->") + field.GetId();

							stream << QStringLiteral("if (const ") << convertedType << QStringLiteral("* val = std::get_if<") << convertedType << QStringLiteral(">((");
							stream << sourceVariableName;
							stream << QStringLiteral(").GetPtr())){");

							FeedStream(stream, 1, false);
							FeedStreamHorizontally(stream, 4);

							if (isCustom){
								imtsdl::CSdlField lookupField;
								lookupField.SetType(sdlType);
								std::shared_ptr<imtsdl::CSdlEntryBase> foundType = GetSdlTypeOrEnumOrUnionForField(lookupField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
								if (!foundType){
									SendCriticalMessage(0, QString("Unable to find type %1").arg(sdlType));
									I_CRITICAL();
								}
								stream << convertedType << QStringLiteral("Object *newObjectPtr = new ");
								stream << convertedType << QStringLiteral("Object(this);");
								FeedStream(stream, 1, false);

								FeedStreamHorizontally(stream, 4);
								stream << QStringLiteral("newObjectPtr->") << versionVariableName << QStringLiteral(" = val->") << versionVariableName << QStringLiteral(";");
								FeedStream(stream, 1, false);
								FeedStreamHorizontally(stream, 4);
								stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
								stream << QStringLiteral("QObjectPtr = QVariant::fromValue(newObjectPtr);");
								FeedStream(stream, 1, false);
							}
							else{
								stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
								stream << QStringLiteral("QObjectPtr = QVariant::fromValue(val);");
								FeedStream(stream, 1, false);
							}

							FeedStreamHorizontally(stream, 3);
							stream << '}';
							FeedStream(stream, 1, false);
						}
					}
					else {
						I_CRITICAL();
					}
				}
				else{
					stream << QStringLiteral("if (!m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr.isValid()){");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, 3);
					stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr = CreateObject(\"") << field.GetId() << QStringLiteral("\");");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 3);
					stream << QStringLiteral("auto itemPtr = m_") << GetDecapitalizedValue(field.GetId());
					stream << QStringLiteral("QObjectPtr.value<") << typeName << QStringLiteral(">();");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, 3);
					stream << QStringLiteral("if (itemPtr != nullptr) itemPtr->") << versionVariableName << QStringLiteral(" = ") << versionVariableName << QStringLiteral("->") << field.GetId() << ';';
					FeedStream(stream, 1, false);
				}

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
		stream << QStringLiteral("return QVariant();");

		FeedStream(stream, 1, false);
		stream << QStringLiteral("}");
		FeedStream(stream, 3, false);

		// Setter implemented
		stream << QStringLiteral("void C") << sdlEntry.GetName() << QStringLiteral("Object");
		stream << QStringLiteral("::Set") << GetCapitalizedValue(field.GetId());
		stream << QStringLiteral("(const QVariant& v)");
		FeedStream(stream, 1, false);

		stream << QStringLiteral("{");
		FeedStream(stream, 1, false);
		const QString versionStructVariableName = QStringLiteral("Version_") + GetSdlEntryVersion(sdlEntry, false);
		
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (!");
		stream << versionStructVariableName;
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		
		FeedStreamHorizontally(stream, 2);
		stream << versionStructVariableName;
		stream << QStringLiteral(".emplace();");
		FeedStream(stream, 1, false);
		
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 2, false);
		
		FeedStreamHorizontally(stream);
		if (isArray && !isCustom){
			stream << QStringLiteral("if (!");
			stream << versionStructVariableName;
			stream << QStringLiteral("->") << field.GetId();
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);
			
			FeedStreamHorizontally(stream, 2);
			stream << versionStructVariableName;
			stream << QStringLiteral("->") << field.GetId();
			stream << QStringLiteral(".emplace();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 1);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 1);
			stream << QStringLiteral("else{");
			FeedStream(stream, 1, false);
			
			FeedStreamHorizontally(stream, 2);
			stream << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral("->clear();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 1);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("for (const auto& tempValue: v.value<") << typeName;
			stream << QStringLiteral(">()){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("istd::TSharedNullable<") << convertedType;
			if (field.GetType() == "ID"){
				stream << QStringLiteral("> value(tempValue.toUtf8());");
			}
			else{
				stream << QStringLiteral("> value(tempValue);");
			}
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral("->append(value);");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);
		}
		else if (isEnum){
			if (isArray){

			}
			else{
				stream << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral(".emplace();");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream);
				stream << QStringLiteral("QMetaEnum metaEnum = QMetaEnum::fromType<");
				stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(">();");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream);
				stream << QStringLiteral("int key = metaEnum.keyToValue(v.value<") << typeName << QStringLiteral(">().toUtf8());");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream);
				stream << QStringLiteral("if (key > -1){");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, 2);
				stream << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral(" = (");
				stream << fieldNameSpace << QStringLiteral("::") << field.GetType() << QStringLiteral(")key;");
				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream);
				stream << '}';
			}
		}
		else if (!isCustom){
			stream << versionStructVariableName << QStringLiteral("->") << field.GetId();
			if (field.GetType() == "ID"){
				stream << QStringLiteral(" = v.value<") << typeName << QStringLiteral(">().toUtf8();");
			}
			else{
				stream << QStringLiteral(" = v.value<") << typeName << QStringLiteral(">();");
			}
		}
		else{
			if (isUnion && !isArray){
				stream << QStringLiteral("if (v.isValid()){");
				FeedStream(stream, 1, false);
				const auto* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(sdlEntryField.get());

				if (sdlUnion != nullptr){
					// QList<QString> convertedTypeList;
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
						const QString sourceVariableName = QStringLiteral("*Version_") + GetSdlEntryVersion(sdlEntry, false) + QStringLiteral("->") + field.GetId();
						QString objectConvertedType = convertedType;
						if (isCustom){
							objectConvertedType += "Object";
						}

						stream << QStringLiteral("if (const ") << objectConvertedType;

						stream << QStringLiteral("* val = v.value<const ") << convertedType;
						if (isCustom){
							stream << QStringLiteral("Object");
						}
						stream << QStringLiteral("*>()){");
						FeedStream(stream, 1, false);

						FeedStreamHorizontally(stream, 3);
						stream << sourceVariableName << QStringLiteral(" = *val;");;
						FeedStream(stream, 1, false);

						FeedStreamHorizontally(stream, 2);
						stream << QStringLiteral("}");;
						FeedStream(stream, 1, false);
					}
				}
			}
			else{
				stream << QStringLiteral("if (v.isValid()){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << typeName << QStringLiteral(" itemPtr = v.value<") << typeName << QStringLiteral(">();");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("if (itemPtr != nullptr)  Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->") << field.GetId() << QStringLiteral(" = itemPtr->Version_") << GetSdlEntryVersion(*sdlEntryField, false) << QStringLiteral(";");
				FeedStream(stream, 1, false);
			}

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("else {");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->") << field.GetId();
			stream << QStringLiteral(" = nullptr;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
			stream << QStringLiteral("QObjectPtr = v;");
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Changed();");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("}");
		FeedStream(stream, 3, false);

		// Has implemented
		stream << QStringLiteral("bool C") << sdlEntry.GetName() << QStringLiteral("Object");
		stream << QStringLiteral("::has") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
		FeedStream(stream, 1, false);

		stream << QStringLiteral("{");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral(" return ") << versionStructVariableName << QStringLiteral(" && ") << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral(".HasValue();");
		FeedStream(stream, 1, false);

		stream << QStringLiteral("}");
		FeedStream(stream, 3, false);

		// Create property implemented
		if (isCustom && !isEnum){
			stream << QStringLiteral("void C") << sdlEntry.GetName() << QStringLiteral("Object");
			stream << QStringLiteral("::emplace") << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if(!");
			stream << versionStructVariableName;
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << versionStructVariableName;
			stream << QStringLiteral(".emplace();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 1);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << versionStructVariableName << QStringLiteral("->") << field.GetId() << QStringLiteral(".emplace();");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("}");
			FeedStream(stream, 3, false);

			// Reset implementation
			stream << QStringLiteral("void C") << sdlEntry.GetName() << QStringLiteral("Object::Reset");
			stream << GetCapitalizedValue(field.GetId()) << QStringLiteral("()");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("auto* itemPtr = m_");
			stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("QObjectPtr");
			stream << QStringLiteral(".value<QObject*>();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (itemPtr != nullptr){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("itemPtr->deleteLater();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("m_");
			stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("QObjectPtr");
			stream << QStringLiteral(" = QVariant();");
			FeedStream(stream, 1, false);

			stream << '}';
			FeedStream(stream, 3, false);
		}

		// create array element implementation
		if (isCustom && !isEnum && isArray && !isUnion){
			stream << QStringLiteral("QVariant C") << sdlEntry.GetName() << QStringLiteral("Object");
			stream << QStringLiteral("::create") << GetCapitalizedValue(field.GetId()) << QStringLiteral("ArrayElement(const QVariant& v)");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("{");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("Q_UNUSED(v);");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("return QVariant::fromValue(new ");
			stream << CSdlGenTools::GetQObjectTypeName(
				field, 
				m_sdlTypeListCompPtr->GetSdlTypes(false), 
				m_sdlEnumListCompPtr->GetEnums(false), 
				m_sdlUnionListCompPtr->GetUnions(false), 
				false, 
				true);
			stream << QStringLiteral("());");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("}");
			FeedStream(stream, 3, false);
		}
	}

	// CItemModelBase implemented

	stream << QStringLiteral("QString C") << sdlEntry.GetName();
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

	stream << QStringLiteral("bool C") << sdlEntry.GetName();
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

	stream << QStringLiteral("bool C") << sdlEntry.GetName();
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

	stream << QStringLiteral("QString C") << sdlEntry.GetName();
	stream << QStringLiteral("Object::toGraphQL() const");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::toGraphQL();");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	stream << QStringLiteral("QVariant C") << sdlEntry.GetName();
	stream << QStringLiteral("Object::CreateObject(const QString& key)");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_UNUSED(key);");

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
		if ((isCustom && !isEnum && !isUnion) || (isUnion && isArray)){
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (key == \"") << field.GetId() << QStringLiteral("\"){");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return QVariant::fromValue(new ") << CSdlGenTools::GetQObjectTypeName(field, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), false) << QStringLiteral("(this));");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);
		}
	}

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return QVariant();");
	FeedStream(stream,1, false);

	stream << QStringLiteral("}");
	FeedStream(stream,3, false);

	stream << QStringLiteral("QString C") << sdlEntry.GetName();
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


bool CSdlQObjectGeneratorComp::ProcessSourceClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const
{
	const auto* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr){
		return true;
	}

	FeedStream(stream, 3, false);

	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEntry.GetSchemaParams());
	QString itemClassName = QStringLiteral("C") + sdlEntry.GetName();
	QString itemClassNameWithNamespace = sdlNamespace + "::" + itemClassName;
	QString modelDataTypeName = itemClassName + QStringLiteral("::V") + GetSdlEntryVersion(sdlEntry, false);

	QString modelObjectDataTypeName = itemClassNameWithNamespace + QStringLiteral("Object");
	QString objectListClassName = QStringLiteral("C") + sdlEntry.GetName() + QStringLiteral("ObjectList");
	QString objectListClassNameWithNamespace = sdlNamespace + QStringLiteral("::") + objectListClassName;
	const imtsdl::SdlFieldList fieldList =  sdlTypePtr->GetFields();
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	stream << QStringLiteral("bool ") + objectListClassName + QStringLiteral("::containsKey(const QString& /*nameId*/, int /*index*/) const");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return true;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	stream << QStringLiteral("int ") + objectListClassName + QStringLiteral("::getItemsCount() const");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return rowCount();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);

	// get method begin
	stream << QStringLiteral("QVariantMap ") + objectListClassName + QStringLiteral("::get(int row) const");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::get(row);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// get method end

	// append method begin
	stream << QStringLiteral("void ") + objectListClassName + QStringLiteral("::append(") << modelObjectDataTypeName << QStringLiteral("* item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("BaseClass::append(item);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// append method end

	// copyMe method begin
	stream << objectListClassNameWithNamespace << QStringLiteral("* ") + objectListClassNameWithNamespace + QStringLiteral("::copyMe()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << objectListClassNameWithNamespace << QStringLiteral("* retVal = new ");
	stream << objectListClassNameWithNamespace << QStringLiteral("();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("BaseClass::fromMe(retVal);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// copyMe method end

	// toJson method begin
	stream << QStringLiteral("QString ") + objectListClassNameWithNamespace + QStringLiteral("::toJson()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::toJson();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// toJson method end

	// toGraphQL method begin
	stream << QStringLiteral("QString ") + objectListClassNameWithNamespace + QStringLiteral("::toGraphQL()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::toGraphQL();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// toGraphQL method end

	// addElement method begin
	stream << QStringLiteral("void ") + objectListClassNameWithNamespace + QStringLiteral("::addElement(") + modelObjectDataTypeName + QStringLiteral("* item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("append(item);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// addElement method end

	// removeElement method begin
	stream << QStringLiteral("void ") + objectListClassNameWithNamespace + QStringLiteral("::removeElement(int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("remove(index);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// removeElement method end

	// isEqualWithModel method begin
	stream << QStringLiteral("bool ") + objectListClassNameWithNamespace + QStringLiteral("::isEqualWithModel(") << objectListClassNameWithNamespace << QStringLiteral("* otherModelPtr)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::isEqualWithModel(otherModelPtr);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// isEqualWithModel method end

	// insert method begin
	stream << QStringLiteral("void ") + objectListClassNameWithNamespace + QStringLiteral("::insert(int index, ") << modelObjectDataTypeName << QStringLiteral("* item)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::insert(index, item);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// insert method end

	// remove method begin
	stream << QStringLiteral("void ") + objectListClassNameWithNamespace + QStringLiteral("::remove(int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::remove(index);");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// remove method end

	// clear method begin
	stream << QStringLiteral("void ") + objectListClassNameWithNamespace + QStringLiteral("::clear()");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::clear();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 3, false);
	// clear method end

	stream << QStringLiteral("QVariant ") + objectListClassNameWithNamespace + QStringLiteral("::getData(const QString& nameId, int index)");
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant item = GetOrCreateCachedObject(index);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("auto* itemPtr = item.value<") << modelObjectDataTypeName << QStringLiteral("*>();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("if (itemPtr == nullptr){");
	FeedStream(stream, 1, false);
	
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return QVariant();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("if (nameId == \"item\" && Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral(".has_value() && index >= 0 && index < Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->count()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return QVariant::fromValue(item);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: fieldList){
		const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (nameId == \"m_") << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("\"){");
		FeedStream(stream, 1, false);

		if (!isCustom || isEnum){
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return QVariant::fromValue(Version_") << GetSdlEntryVersion(sdlEntry, false) << QStringLiteral("->at(index)->");
			stream << field.GetId() << QStringLiteral(".value());");
			FeedStream(stream, 1, false);
		}
		else{
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return itemPtr->Get");
			stream << GetCapitalizedValue(field.GetId()) << QStringLiteral("();");
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}

	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return QVariant();");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("}");
	FeedStream(stream, 1, false);

	return true;
}


} // namespace imtsdlgencpp

