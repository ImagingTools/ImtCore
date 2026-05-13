# Руководство по атрибуту "ref" в SDL схеме ImtCore

## Что такое атрибут "ref"?

Атрибут **`ref`** (reference) в SDL схеме ImtCore — это ключевое свойство, которое связывает коллекцию документов с конкретным типом данных GraphQL. Он определяет структуру данных, которая будет использоваться для представления элементов коллекции.

## Назначение

Атрибут `ref` выполняет следующие функции:

1. **Связывание типов данных**: Указывает, какой GraphQL тип (type) или union будет использоваться для представления данных в коллекции
2. **Генерация кода**: На основе референсного типа SDL генераторы создают C++ код для работы с коллекциями
3. **Типизация данных**: Обеспечивает строгую типизацию данных в коллекциях документов

## Где используется атрибут "ref"?

Атрибут `ref` используется внутри блока `collectionSchema` в SDL файлах:

```graphql
collectionSchema {
    type <CollectionName> {
        ref: <TypeName>
        operations {
            // операции над коллекцией
        }
        subtype <SubtypeName> {
            ref: <DetailedTypeName>
            operations {
                // операции над подтипом
            }
        }
    }
}
```

## Как работает атрибут "ref"?

### Внутренняя обработка

При парсинге SDL схемы (в `CGqlExtSchemaParser::ExtractDocumentTypeFromCurrentEntry`):

1. Парсер считывает значение атрибута `ref`
2. Ищет соответствующий GraphQL тип или union среди определенных типов
3. Связывает найденный тип с документом коллекции через `CSdlDocumentType::SetReferenceType()`
4. Генераторы кода используют эту информацию для создания C++ классов

### Код парсера (упрощенно):

```cpp
// Из CGqlExtSchemaParser.cpp, строки 136-178
if (keyword == "ref") {
    QByteArray typeRefName;
    // Читаем имя типа после "ref:"
    ReadToDelimeterOrSpace("", typeRefName);
    
    // Ищем тип среди GraphQL типов
    auto foundType = std::find_if(m_sdlTypes, [&typeRefName](const CSdlType& type) {
        return type.GetName() == typeRefName;
    });
    
    if (foundType) {
        documentType.SetReferenceType(*foundType);
    }
    // Или ищем среди union типов
    else {
        auto foundUnion = std::find_if(m_unions, [&typeRefName](const CSdlUnion& aUnion) {
            return aUnion.GetName() == typeRefName;
        });
        if (foundUnion) {
            documentType.SetReferenceType(*foundUnion);
        }
    }
}
```

## Примеры использования

### Пример 1: Базовая коллекция пользователей

**Файл**: `Sdl/imtauth/1.0/Users.sdl`

```graphql
# Определяем тип для элемента списка (краткая информация)
type UserItemData {
    id: ID!
    typeId: ID!
    userId: ID
    name: String
    description: String
    mail: String
    roles: ID
    groups: ID
    added: String
    lastModified: String
}

# Определяем тип для детальных данных пользователя
type UserData {
    id: ID
    productId: ID
    name: String
    username: ID
    password: String
    email: String
    groups: [ID]
    roles: [ID]
    permissions: [ID]
}

# Определяем схему коллекции
collectionSchema {
    type User {
        # ref указывает на тип для элементов списка
        ref: UserItemData
        operations {
            list: UsersList
        }
        subtype User {
            # ref для подтипа указывает на детальные данные
            ref: UserData
            operations {
                get: UserItem
                update: UserUpdate
                insert: UserAdd
            }
        }
    }
}
```

**Объяснение**:
- `ref: UserItemData` в основном типе — используется для отображения списка пользователей (кратких данных)
- `ref: UserData` в подтипе — используется для работы с полными данными конкретного пользователя

### Пример 2: Коллекция лицензий

**Файл**: `Sdl/imtlic/1.0/Licenses.sdl`

```graphql
# Тип для элемента списка лицензий
type LicenseItem {
    id: ID!
    typeId: ID!
    licenseId: ID!
    licenseName: String!
    description: String
    productId: ID
    productUuid: ID
    parentLicenses: ID
    features: ID
    added: String
    timeStamp: String
}

# Тип для детальных данных лицензии
type LicenseDefinitionData {
    id: ID
    licenseName: String
    description: String
    productId: ID
    licenseId: ID
    features: ID
    parentLicenses: ID
}

collectionSchema {
    type License {
        ref: LicenseItem          # Ссылка на тип списка
        operations {
            list: LicensesList
        }
        subtype License {
            ref: LicenseDefinitionData  # Ссылка на детальные данные
            operations {
                get: LicenseItem
                update: LicenseUpdate
                insert: LicenseAdd
            }
        }
    }
}
```

### Пример 3: Коллекция ролей

**Файл**: `Sdl/imtauth/1.0/Roles.sdl`

```graphql
# Тип для краткой информации о роли
type RoleItemData {
    typeId: ID!
    id: ID!
    roleName: String!
    roleId: ID
    productId: ID
    parentRoles: ID
    roleDescription: String
    added: String
    lastModified: String
}

# Тип для полных данных роли
type RoleData {
    id: ID
    name: String
    description: String
    roleId: ID
    productId: ID
    parentRoles: [ID]
    permissions: ID
    isDefault: Bool
    isGuest: Bool
}

collectionSchema {
    type Role {
        ref: RoleItemData         # Краткие данные для списка
        operations {
            list: RolesList
        }
        subtype Role {
            ref: RoleData         # Детальные данные для редактирования
            operations {
                get: RoleItem
                update: RoleUpdate
                insert: RoleAdd
            }
        }
    }
}
```

### Пример 4: Коллекция продуктов с вложенными данными

**Файл**: `Sdl/imtlic/1.0/Products.sdl`

```graphql
# Вложенный тип для данных о лицензиях
type LicenseData {
    id: ID
    name: String
    licenseId: ID
    licenseName: String
}

# Тип элемента списка продуктов (включает вложенные лицензии)
type ProductItem {
    id: ID!
    name: String!
    productName: String!
    typeId: ID!
    productId: ID!
    categoryId: ID!
    description: String
    features: ID
    licenses: [LicenseData]    # Вложенный список лицензий
    added: String
    timeStamp: String
}

# Тип детальных данных продукта
type ProductData {
    id: ID
    name: String
    productName: String
    description: String
    productId: ID
    categoryId: ID
    features: ID
}

collectionSchema {
    type Product {
        ref: ProductItem          # Ссылка на тип со вложенными данными
        operations {
            list: ProductsList
        }
        subtype Product {
            ref: ProductData      # Ссылка на детальные данные
            operations {
                get: ProductItem
                update: ProductUpdate
                insert: ProductAdd
            }
        }
    }
}
```

**Особенность**: Тип `ProductItem` содержит вложенное поле `licenses: [LicenseData]`, показывая, что референсные типы могут быть составными и содержать вложенные структуры.

## Типичные паттерны использования

### Паттерн 1: Список + Детали (List + Details)

Это наиболее распространенный паттерн в ImtCore:

```graphql
# Краткая информация для списка (оптимизировано для отображения)
type <Entity>ItemData {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    # Другие поля для отображения в списке
}

# Полная информация (для редактирования и детального просмотра)
type <Entity>Data {
    id: ID
    name: String
    # Все поля сущности
}

collectionSchema {
    type <Entity> {
        ref: <Entity>ItemData    # Для list операции
        operations {
            list: <Entity>List
        }
        subtype <Entity> {
            ref: <Entity>Data     # Для get/update/insert операций
            operations {
                get: <Entity>Item
                update: <Entity>Update
                insert: <Entity>Add
            }
        }
    }
}
```

### Паттерн 2: Иерархические данные

Для сущностей с иерархией (родитель-потомок):

```graphql
type GroupItemData {
    id: ID!
    name: String!
    parentGroups: ID         # Ссылки на родительские группы
}

type GroupData {
    id: ID
    name: String
    parentGroups: [ID]       # Массив родительских групп
    users: [ID]              # Связанные пользователи
    roles: [ID]              # Связанные роли
}

collectionSchema {
    type Group {
        ref: GroupItemData
        operations {
            list: GroupsList
        }
        subtype Group {
            ref: GroupData
            operations {
                get: GroupItem
                update: GroupUpdate
                insert: GroupAdd
            }
        }
    }
}
```

## Требования к референсным типам

### 1. Обязательные поля

Тип, на который ссылается `ref` в основной коллекции, **должен** содержать:

```graphql
type <RefType> {
    id: ID!          # Обязательно!
    typeId: ID!      # Обязательно для коллекций ImtCore!
    # Другие поля...
}
```

### 2. Соответствие операциям

Поля референсного типа должны соответствовать полям в операциях:

```graphql
# Тип для списка
type UserItemData {
    id: ID!
    name: String
    mail: String
}

# Query должен возвращать список этого типа
type UsersListPayload {
    items: [UserItemData]     # ← Должен совпадать с ref
    notification: NotificationItem
}

type Query {
    UsersList(input: UsersListInput): UsersListPayload!
}
```

### 3. Типы должны быть определены ДО collectionSchema

```graphql
# ✓ Правильно: сначала определяем типы
type MyItemData { ... }
type MyData { ... }

# Затем используем их в collectionSchema
collectionSchema {
    type MyCollection {
        ref: MyItemData
        ...
    }
}

# ✗ Неправильно: использовать ref до определения типа
collectionSchema {
    type MyCollection {
        ref: UndefinedType    # Ошибка!
    }
}

type UndefinedType { ... }
```

## Генерация кода на основе ref

SDL генераторы (`imtsdlgencpp`, `imtsdlgenqml`) используют информацию из `ref` для генерации:

### C++ классы:

1. **Коллекционные контроллеры** (`CGqlCollectionControllerBase`):
   - Методы для работы со списками
   - Методы для CRUD операций

2. **Прокси-классы** для взаимодействия с GraphQL:
   - Запросы списков
   - Запросы деталей
   - Мутации

3. **Модели данных** для Qt/QML:
   - QAbstractItemModel для списков
   - Объекты для детальных данных

### Пример сгенерированного C++ кода (концептуально):

```cpp
// На основе: ref: UserItemData
class CUserCollectionController {
public:
    // Получить список пользователей (использует UserItemData)
    QList<UserItemData> GetUsersList(const CollectionViewParams& params);
    
    // Получить детали пользователя (использует UserData из subtype)
    UserData GetUserItem(const QString& userId);
    
    // Обновить пользователя
    bool UpdateUser(const QString& userId, const UserData& data);
};
```

## Связь ref с операциями

Атрибут `ref` тесно связан с операциями в блоке `operations`:

```graphql
collectionSchema {
    type User {
        ref: UserItemData        # ← Этот тип используется в list операции
        operations {
            list: UsersList      # ← Возвращает [UserItemData]
        }
        subtype User {
            ref: UserData        # ← Этот тип используется в get/update/insert
            operations {
                get: UserItem    # ← Возвращает UserData
                update: UserUpdate   # ← Принимает UserData
                insert: UserAdd      # ← Принимает UserData
            }
        }
    }
}
```

### Соответствие операций и типов:

| Операция | Ref тип | Назначение |
|----------|---------|------------|
| `list` | Основной ref | Возвращает массив элементов типа ref |
| `get` | Ref подтипа | Возвращает детальный объект типа ref |
| `update` | Ref подтипа | Принимает объект типа ref для обновления |
| `insert` | Ref подтипа | Принимает объект типа ref для создания |
| `delete` | Основной ref | Использует ID из типа ref |

## Ошибки и их решение

### Ошибка 1: "Unable to find type 'TypeName'"

```
Ошибка: Unable to find type 'UserItemData' at line 182
```

**Причина**: Тип не определен в SDL или определен после использования.

**Решение**:
```graphql
# Определите тип ДО collectionSchema
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
}

collectionSchema {
    type User {
        ref: UserItemData    # ✓ Теперь тип найден
        ...
    }
}
```

### Ошибка 2: "Reference for 'CollectionType' at line X"

```
Ошибка: Reference for 'User' at line 180
```

**Причина**: Пустое значение `ref` или синтаксическая ошибка.

**Решение**:
```graphql
# ✗ Неправильно
collectionSchema {
    type User {
        ref:              # Пустое значение!
    }
}

# ✓ Правильно
collectionSchema {
    type User {
        ref: UserItemData
    }
}
```

### Ошибка 3: Несоответствие типов в операциях

**Причина**: Тип в GraphQL операции не совпадает с ref типом.

**Решение**: Убедитесь в соответствии:

```graphql
type UserItemData {
    id: ID!
    name: String
}

type UsersListPayload {
    items: [UserItemData]    # ← Должен совпадать с ref
    ...
}

collectionSchema {
    type User {
        ref: UserItemData    # ← Должен совпадать с PayLoad.items
        operations {
            list: UsersList
        }
    }
}
```

## Лучшие практики

### 1. Именование типов

Следуйте конвенции именования:

```graphql
# Для списочных данных (ref основного типа)
type <Entity>ItemData { ... }
type <Entity>Item { ... }

# Для детальных данных (ref подтипа)
type <Entity>Data { ... }
type <Entity>DefinitionData { ... }
```

### 2. Разделение ответственности

- **ItemData типы**: Минимальный набор полей для отображения в списках
- **Data типы**: Полный набор полей для редактирования

```graphql
# ItemData - оптимизирован для списков
type ProductItem {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    added: String
}

# Data - полные данные для редактирования
type ProductData {
    id: ID
    name: String
    description: String
    categoryId: ID
    features: ID
    # Все остальные поля...
}
```

### 3. Документирование ref типов

Используйте комментарии для пояснения:

```graphql
# Краткая информация о пользователе для отображения в списках
# Используется как ref в collectionSchema для операции list
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
    mail: String
}

# Полная информация о пользователе для редактирования
# Используется как ref в subtype для операций get/update/insert
type UserData {
    id: ID
    name: String
    email: String
    groups: [ID]
    roles: [ID]
}
```

## Сложные сценарии

### Использование Union типов

`ref` может ссылаться не только на type, но и на union:

```graphql
union SearchResult = UserItem | GroupItem | RoleItem

collectionSchema {
    type SearchResults {
        ref: SearchResult    # Ссылка на union
        operations {
            list: Search
        }
    }
}
```

Код парсера проверяет сначала types, затем unions:

```cpp
// Сначала ищем среди типов
if (foundIterator != m_sdlTypes.cend()) {
    documentType.SetReferenceType(*foundIterator);
}
// Затем среди union
else if (foundUnionIterator != m_unions.cend()) {
    documentType.SetReferenceType(*foundUnionIterator);
}
```

### Импорт типов из других SDL файлов

Типы могут быть импортированы:

```graphql
# Users.sdl
import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# NotificationItem импортирован из ImtCollection.sdl
type UsersListPayload {
    items: [UserItemData]
    notification: NotificationItem    # Импортированный тип
}

collectionSchema {
    type User {
        ref: UserItemData               # Локальный тип
        operations {
            list: UsersList
        }
    }
}
```

## Заключение

Атрибут **`ref`** в SDL схеме ImtCore является критически важным механизмом для:

1. **Связывания** GraphQL типов с коллекциями документов
2. **Генерации** C++/QML кода для работы с данными
3. **Обеспечения** типобезопасности при работе с коллекциями

Правильное использование `ref` обеспечивает:
- Корректную генерацию кода
- Согласованность типов данных
- Удобство работы с GraphQL API

## Дополнительные ресурсы

- **Исходный код парсера**: `Include/imtsdl/CGqlExtSchemaParser.cpp` (строки 136-178)
- **Класс документа**: `Include/imtsdl/CSdlDocumentType.h`
- **Примеры SDL**: `Sdl/imtauth/1.0/`, `Sdl/imtlic/1.0/`
- **Генераторы кода**: `Include/imtsdlgencpp/`, `Include/imtsdlgenqml/`
- **Краткая справка (English)**: [SDL_Ref_Quick_Reference.md](./SDL_Ref_Quick_Reference.md)
- **Практические примеры**: [SDL_Ref_Examples.md](./SDL_Ref_Examples.md)
- **Визуальные диаграммы**: [SDL_Ref_Diagrams.md](./SDL_Ref_Diagrams.md)

## История изменений

- **2026-02-13**: Создана первая версия документации на основе анализа кодовой базы
