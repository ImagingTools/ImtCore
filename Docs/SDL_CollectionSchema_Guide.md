# CollectionSchema in SDL - Complete Guide
# CollectionSchema в SDL - Полное руководство

This document provides a comprehensive explanation of the `collectionSchema` block in ImtCore SDL (Schema Definition Language), in both English and Russian.

Этот документ содержит полное объяснение блока `collectionSchema` в ImtCore SDL (Schema Definition Language) на английском и русском языках.

---

## English Version

### What is `collectionSchema`?

The **`collectionSchema`** block is a fundamental construct in ImtCore SDL files that defines document collections and their operations. It bridges GraphQL type definitions with the underlying document collection infrastructure, enabling automatic code generation for collection management, CRUD operations, and GraphQL API endpoints.

### Purpose and Role

The `collectionSchema` block serves several critical functions:

1. **Collection Definition**: Declares document collection types that will be managed by the system
2. **Type Association**: Links collections to GraphQL types via the `ref` attribute
3. **Operation Mapping**: Maps GraphQL queries and mutations to collection operations
4. **Code Generation**: Provides metadata for C++ and QML code generators
5. **API Structure**: Defines the structure of collection-based APIs

### Basic Syntax

```graphql
collectionSchema {
    type <CollectionName> {
        ref: <TypeName>
        operations {
            list: <QueryName>
            delete: <MutationName>
        }
        subtype <SubtypeName> {
            ref: <DetailedTypeName>
            operations {
                get: <QueryName>
                update: <MutationName>
                insert: <MutationName>
            }
        }
    }
}
```

### Key Components

#### 1. Collection Type Declaration

```graphql
type User {
    ref: UserItemData
    operations { ... }
}
```

- **`type <Name>`**: Declares a collection type
- The name represents the entity being managed (e.g., User, License, Product)
- Must be unique within the SDL file

#### 2. The `ref` Attribute

```graphql
ref: UserItemData
```

- Links the collection to a GraphQL type definition
- Specifies the data structure for collection items
- The referenced type must be defined before the `collectionSchema` block
- See [SDL_Reference_Attribute_Guide.md](./SDL_Reference_Attribute_Guide.md) for detailed information

#### 3. Operations Block

```graphql
operations {
    list: UsersList        # Query to list items
    delete: UserDelete     # Mutation to delete items
}
```

Defines GraphQL operations for the collection:
- **`list`**: Query that returns multiple collection items
- **`delete`**: Mutation that removes items from collection
- Operation names must match GraphQL Query/Mutation definitions

#### 4. Subtype Declaration

```graphql
subtype User {
    ref: UserData
    operations {
        get: UserItem
        update: UserUpdate
        insert: UserAdd
    }
}
```

Subtypes define detailed operations on individual collection items:
- **`get`**: Retrieve a single item with full details
- **`update`**: Modify an existing item
- **`insert`**: Create a new item
- Uses a different `ref` type than the parent (typically more detailed)

### Complete Example

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: imtauth
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# List type - lightweight for display
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
    mail: String
    roles: ID
    groups: ID
}

# Detail type - complete for editing
type UserData {
    id: ID
    name: String
    username: ID
    email: String
    groups: [ID]
    roles: [ID]
    permissions: [ID]
}

# Input types for operations
input UsersListInput {
    viewParams: CollectionViewParams
}

type UsersListPayload {
    items: [UserItemData]
    notification: NotificationItem
}

input UserItemInput {
    id: ID!
}

input UserDataInput {
    id: ID!
    item: UserData!
}

# GraphQL operations
type Query {
    UsersList(input: UsersListInput): UsersListPayload!
    UserItem(input: UserItemInput): UserData!
}

type Mutation {
    UserAdd(input: UserDataInput): AddedNotificationPayload!
    UserUpdate(input: UserDataInput): UpdatedNotificationPayload!
    UserDelete(input: UserItemInput): DeletedNotificationPayload!
}

# Collection schema definition
collectionSchema {
    type User {
        ref: UserItemData           # Type for list view
        operations {
            list: UsersList         # Get list of users
            delete: UserDelete      # Delete a user
        }
        subtype User {
            ref: UserData          # Type for detailed operations
            operations {
                get: UserItem      # Get single user details
                update: UserUpdate # Update user
                insert: UserAdd    # Create new user
            }
        }
    }
}
```

### Design Patterns

#### Pattern 1: List + Details (Most Common)

This is the standard pattern used throughout ImtCore:

```graphql
# Lightweight type for lists
type EntityItemData {
    id: ID!
    typeId: ID!
    # Minimal fields for display
}

# Complete type for editing
type EntityData {
    id: ID
    # All fields for full operations
}

collectionSchema {
    type Entity {
        ref: EntityItemData      # For list operations
        operations {
            list: EntityList
            delete: EntityDelete
        }
        subtype Entity {
            ref: EntityData      # For detailed operations
            operations {
                get: EntityItem
                update: EntityUpdate
                insert: EntityAdd
            }
        }
    }
}
```

**Benefits:**
- Optimized list queries (less data transfer)
- Separation of display and editing concerns
- Clear API structure

#### Pattern 2: Hierarchical Collections

For entities with parent-child relationships:

```graphql
type GroupItemData {
    id: ID!
    name: String!
    parentGroups: ID        # References to parent groups
}

type GroupData {
    id: ID
    name: String
    parentGroups: [ID]      # Array of parent group IDs
    childGroups: [ID]       # Child groups
}

collectionSchema {
    type Group {
        ref: GroupItemData
        operations {
            list: GroupsList
            delete: GroupDelete
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

#### Pattern 3: Collections with Nested Data

For collections containing nested objects:

```graphql
type LicenseData {
    id: ID
    name: String
    licenseId: ID
}

type ProductItem {
    id: ID!
    name: String!
    licenses: [LicenseData]    # Nested list
}

collectionSchema {
    type Product {
        ref: ProductItem
        operations {
            list: ProductsList
        }
        subtype Product {
            ref: ProductData
            operations {
                get: ProductItem
                update: ProductUpdate
                insert: ProductAdd
            }
        }
    }
}
```

### Code Generation

The `collectionSchema` block drives automatic code generation for:

#### C++ Classes Generated:

1. **Collection Controllers** (`CGql<Name>CollectionControllerBase`):
   ```cpp
   class CGqlUserCollectionControllerBase {
   public:
       // List operation
       virtual IObjectCollectionUniquePtr GetUsersList(
           const CollectionViewParams& params);
       
       // Delete operation
       virtual bool DeleteUser(const QString& userId);
       
       // Get operation (from subtype)
       virtual UserData GetUserItem(const QString& userId);
       
       // Update operation (from subtype)
       virtual bool UpdateUser(const QString& userId, const UserData& data);
       
       // Insert operation (from subtype)
       virtual QString AddUser(const UserData& data);
   };
   ```

2. **GraphQL Handlers**: Request/response processing
3. **Data Models**: Qt/QML integration classes
4. **Proxy Classes**: Client-side collection access

#### QML Components Generated:

1. **Data Models**: `QAbstractItemModel` implementations for lists
2. **Item Objects**: QML-accessible data objects
3. **Controller Interfaces**: Collection operation wrappers

### Operation Mapping

The relationship between `collectionSchema` operations and GraphQL:

| CollectionSchema Operation | GraphQL Type | Purpose | Returns/Accepts |
|---------------------------|--------------|---------|-----------------|
| `list` (main type) | Query | Get collection items | Array of `ref` type |
| `delete` (main type) | Mutation | Remove item(s) | Notification/Status |
| `get` (subtype) | Query | Get single item | Single `ref` object |
| `update` (subtype) | Mutation | Modify item | Notification/Status |
| `insert` (subtype) | Mutation | Create item | ID + Notification |

### Requirements and Rules

#### 1. Type Order

Types must be defined **before** the `collectionSchema` block:

```graphql
# ✓ Correct order
type UserItemData { ... }
type UserData { ... }

collectionSchema {
    type User {
        ref: UserItemData
        ...
    }
}

# ✗ Wrong order
collectionSchema {
    type User {
        ref: UserItemData    # Error: type not yet defined
        ...
    }
}

type UserItemData { ... }
```

#### 2. Required Fields in Referenced Types

Types used in `ref` must contain:

```graphql
type EntityItemData {
    id: ID!          # Required
    typeId: ID!      # Required for ImtCore collections
    # Other fields...
}
```

#### 3. Operation Consistency

Operation names must match GraphQL definitions:

```graphql
# In collectionSchema
operations {
    list: UsersList
}

# Must have corresponding Query
type Query {
    UsersList(input: UsersListInput): UsersListPayload!
}
```

#### 4. Subtype Naming

By convention, subtype often has the same name as the parent type:

```graphql
collectionSchema {
    type User {           # Parent type name
        ...
        subtype User {    # Subtype with same name
            ...
        }
    }
}
```

However, different names are allowed for specific use cases.

### Common Errors and Solutions

#### Error 1: "Unable to find type 'TypeName'"

**Problem**: Referenced type doesn't exist or is defined after `collectionSchema`

**Solution**: Define the type before the `collectionSchema` block

```graphql
# Define first
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
}

# Then reference
collectionSchema {
    type User {
        ref: UserItemData    # ✓ Now found
        ...
    }
}
```

#### Error 2: "Unexpected directive 'X' at line Y"

**Problem**: Syntax error in `collectionSchema` structure

**Solution**: Follow the exact syntax:

```graphql
collectionSchema {
    type EntityName {        # Must use "type" keyword
        ref: TypeName
        operations {
            # operations here
        }
    }
}
```

#### Error 3: Missing Operations

**Problem**: Operation referenced in `collectionSchema` not defined in Query/Mutation

**Solution**: Ensure all referenced operations exist:

```graphql
collectionSchema {
    type User {
        operations {
            list: UsersList    # Must exist in Query
        }
    }
}

type Query {
    UsersList(input: UsersListInput): UsersListPayload!  # ✓ Defined
}
```

### Best Practices

1. **Naming Conventions**:
   - Collection type: `<Entity>` (e.g., `User`, `License`)
   - List ref type: `<Entity>ItemData` or `<Entity>Item`
   - Detail ref type: `<Entity>Data` or `<Entity>DefinitionData`

2. **Type Separation**:
   - Use lightweight types for lists (only essential fields)
   - Use complete types for detail operations (all fields)

3. **Documentation**:
   - Add comments explaining the purpose of each collection
   - Document special relationships or constraints

4. **Consistency**:
   - Follow established patterns in existing SDL files
   - Maintain consistent operation naming across collections

### Real-World Examples

See these files in the repository for production examples:

- **`Sdl/imtauth/1.0/Users.sdl`**: User collection
- **`Sdl/imtauth/1.0/Roles.sdl`**: Role collection  
- **`Sdl/imtauth/1.0/Groups.sdl`**: Group collection
- **`Sdl/imtlic/1.0/Licenses.sdl`**: License collection
- **`Sdl/imtlic/1.0/Products.sdl`**: Product collection

### Related Documentation

- [SDL Reference Attribute Guide](./SDL_Reference_Attribute_Guide.md) - Detailed `ref` attribute documentation (Russian)
- [SDL Quick Reference](./SDL_Ref_Quick_Reference.md) - Quick reference card
- [SDL Examples](./SDL_Ref_Examples.md) - Practical examples
- [SDL Diagrams](./SDL_Ref_Diagrams.md) - Visual diagrams
- [SDL Documentation Index](./SDL_Documentation_Index.md) - Complete documentation index

---

## Версия на русском языке

### Что такое `collectionSchema`?

Блок **`collectionSchema`** является фундаментальной конструкцией в файлах ImtCore SDL, которая определяет коллекции документов и операции над ними. Он связывает определения типов GraphQL с базовой инфраструктурой коллекций документов, обеспечивая автоматическую генерацию кода для управления коллекциями, CRUD операций и GraphQL API endpoints.

### Назначение и роль

Блок `collectionSchema` выполняет несколько критически важных функций:

1. **Определение коллекций**: Объявляет типы коллекций документов, которыми будет управлять система
2. **Ассоциация типов**: Связывает коллекции с типами GraphQL через атрибут `ref`
3. **Отображение операций**: Связывает GraphQL запросы и мутации с операциями над коллекциями
4. **Генерация кода**: Предоставляет метаданные для генераторов C++ и QML кода
5. **Структура API**: Определяет структуру API на основе коллекций

### Базовый синтаксис

```graphql
collectionSchema {
    type <ИмяКоллекции> {
        ref: <ИмяТипа>
        operations {
            list: <ИмяЗапроса>
            delete: <ИмяМутации>
        }
        subtype <ИмяПодтипа> {
            ref: <ИмяДетальногоТипа>
            operations {
                get: <ИмяЗапроса>
                update: <ИмяМутации>
                insert: <ИмяМутации>
            }
        }
    }
}
```

### Ключевые компоненты

#### 1. Объявление типа коллекции

```graphql
type User {
    ref: UserItemData
    operations { ... }
}
```

- **`type <Имя>`**: Объявляет тип коллекции
- Имя представляет управляемую сущность (например, User, License, Product)
- Должно быть уникальным в пределах SDL файла

#### 2. Атрибут `ref`

```graphql
ref: UserItemData
```

- Связывает коллекцию с определением типа GraphQL
- Указывает структуру данных для элементов коллекции
- Ссылочный тип должен быть определен до блока `collectionSchema`
- См. [SDL_Reference_Attribute_Guide.md](./SDL_Reference_Attribute_Guide.md) для подробной информации

#### 3. Блок операций (operations)

```graphql
operations {
    list: UsersList        # Запрос для получения списка
    delete: UserDelete     # Мутация для удаления элементов
}
```

Определяет GraphQL операции для коллекции:
- **`list`**: Запрос, возвращающий множество элементов коллекции
- **`delete`**: Мутация, удаляющая элементы из коллекции
- Имена операций должны соответствовать определениям GraphQL Query/Mutation

#### 4. Объявление подтипа (subtype)

```graphql
subtype User {
    ref: UserData
    operations {
        get: UserItem
        update: UserUpdate
        insert: UserAdd
    }
}
```

Подтипы определяют детальные операции над отдельными элементами коллекции:
- **`get`**: Получить один элемент с полными деталями
- **`update`**: Изменить существующий элемент
- **`insert`**: Создать новый элемент
- Использует отличный от родительского `ref` тип (обычно более детальный)

### Полный пример

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: imtauth
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# Тип для списка - облегченный для отображения
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
    mail: String
    roles: ID
    groups: ID
}

# Тип для деталей - полный для редактирования
type UserData {
    id: ID
    name: String
    username: ID
    email: String
    groups: [ID]
    roles: [ID]
    permissions: [ID]
}

# Входные типы для операций
input UsersListInput {
    viewParams: CollectionViewParams
}

type UsersListPayload {
    items: [UserItemData]
    notification: NotificationItem
}

input UserItemInput {
    id: ID!
}

input UserDataInput {
    id: ID!
    item: UserData!
}

# GraphQL операции
type Query {
    UsersList(input: UsersListInput): UsersListPayload!
    UserItem(input: UserItemInput): UserData!
}

type Mutation {
    UserAdd(input: UserDataInput): AddedNotificationPayload!
    UserUpdate(input: UserDataInput): UpdatedNotificationPayload!
    UserDelete(input: UserItemInput): DeletedNotificationPayload!
}

# Определение схемы коллекции
collectionSchema {
    type User {
        ref: UserItemData           # Тип для отображения списка
        operations {
            list: UsersList         # Получить список пользователей
            delete: UserDelete      # Удалить пользователя
        }
        subtype User {
            ref: UserData          # Тип для детальных операций
            operations {
                get: UserItem      # Получить детали пользователя
                update: UserUpdate # Обновить пользователя
                insert: UserAdd    # Создать нового пользователя
            }
        }
    }
}
```

### Паттерны проектирования

#### Паттерн 1: Список + Детали (Наиболее распространенный)

Это стандартный паттерн, используемый во всем ImtCore:

```graphql
# Облегченный тип для списков
type EntityItemData {
    id: ID!
    typeId: ID!
    # Минимальный набор полей для отображения
}

# Полный тип для редактирования
type EntityData {
    id: ID
    # Все поля для полных операций
}

collectionSchema {
    type Entity {
        ref: EntityItemData      # Для операций со списком
        operations {
            list: EntityList
            delete: EntityDelete
        }
        subtype Entity {
            ref: EntityData      # Для детальных операций
            operations {
                get: EntityItem
                update: EntityUpdate
                insert: EntityAdd
            }
        }
    }
}
```

**Преимущества:**
- Оптимизированные запросы списков (меньше передачи данных)
- Разделение ответственности отображения и редактирования
- Четкая структура API

#### Паттерн 2: Иерархические коллекции

Для сущностей с отношениями родитель-потомок:

```graphql
type GroupItemData {
    id: ID!
    name: String!
    parentGroups: ID        # Ссылки на родительские группы
}

type GroupData {
    id: ID
    name: String
    parentGroups: [ID]      # Массив ID родительских групп
    childGroups: [ID]       # Дочерние группы
}

collectionSchema {
    type Group {
        ref: GroupItemData
        operations {
            list: GroupsList
            delete: GroupDelete
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

#### Паттерн 3: Коллекции с вложенными данными

Для коллекций, содержащих вложенные объекты:

```graphql
type LicenseData {
    id: ID
    name: String
    licenseId: ID
}

type ProductItem {
    id: ID!
    name: String!
    licenses: [LicenseData]    # Вложенный список
}

collectionSchema {
    type Product {
        ref: ProductItem
        operations {
            list: ProductsList
        }
        subtype Product {
            ref: ProductData
            operations {
                get: ProductItem
                update: ProductUpdate
                insert: ProductAdd
            }
        }
    }
}
```

### Генерация кода

Блок `collectionSchema` управляет автоматической генерацией кода для:

#### Генерируемые C++ классы:

1. **Контроллеры коллекций** (`CGql<Name>CollectionControllerBase`):
   ```cpp
   class CGqlUserCollectionControllerBase {
   public:
       // Операция list
       virtual IObjectCollectionUniquePtr GetUsersList(
           const CollectionViewParams& params);
       
       // Операция delete
       virtual bool DeleteUser(const QString& userId);
       
       // Операция get (из подтипа)
       virtual UserData GetUserItem(const QString& userId);
       
       // Операция update (из подтипа)
       virtual bool UpdateUser(const QString& userId, const UserData& data);
       
       // Операция insert (из подтипа)
       virtual QString AddUser(const UserData& data);
   };
   ```

2. **Обработчики GraphQL**: Обработка запросов/ответов
3. **Модели данных**: Классы интеграции Qt/QML
4. **Прокси-классы**: Клиентский доступ к коллекциям

#### Генерируемые QML компоненты:

1. **Модели данных**: Реализации `QAbstractItemModel` для списков
2. **Объекты элементов**: Объекты данных, доступные из QML
3. **Интерфейсы контроллеров**: Обертки операций коллекций

### Отображение операций

Связь между операциями `collectionSchema` и GraphQL:

| Операция CollectionSchema | Тип GraphQL | Назначение | Возвращает/Принимает |
|--------------------------|-------------|------------|---------------------|
| `list` (основной тип) | Query | Получить элементы коллекции | Массив типа `ref` |
| `delete` (основной тип) | Mutation | Удалить элемент(ы) | Уведомление/Статус |
| `get` (подтип) | Query | Получить один элемент | Единичный объект `ref` |
| `update` (подтип) | Mutation | Изменить элемент | Уведомление/Статус |
| `insert` (подтип) | Mutation | Создать элемент | ID + Уведомление |

### Требования и правила

#### 1. Порядок типов

Типы должны быть определены **до** блока `collectionSchema`:

```graphql
# ✓ Правильный порядок
type UserItemData { ... }
type UserData { ... }

collectionSchema {
    type User {
        ref: UserItemData
        ...
    }
}

# ✗ Неправильный порядок
collectionSchema {
    type User {
        ref: UserItemData    # Ошибка: тип еще не определен
        ...
    }
}

type UserItemData { ... }
```

#### 2. Обязательные поля в ссылочных типах

Типы, используемые в `ref`, должны содержать:

```graphql
type EntityItemData {
    id: ID!          # Обязательно
    typeId: ID!      # Обязательно для коллекций ImtCore
    # Другие поля...
}
```

#### 3. Согласованность операций

Имена операций должны соответствовать определениям GraphQL:

```graphql
# В collectionSchema
operations {
    list: UsersList
}

# Должен быть соответствующий Query
type Query {
    UsersList(input: UsersListInput): UsersListPayload!
}
```

#### 4. Именование подтипов

По соглашению, подтип часто имеет то же имя, что и родительский тип:

```graphql
collectionSchema {
    type User {           # Имя родительского типа
        ...
        subtype User {    # Подтип с тем же именем
            ...
        }
    }
}
```

Однако разные имена допускаются для специфических случаев использования.

### Частые ошибки и решения

#### Ошибка 1: "Unable to find type 'TypeName'"

**Проблема**: Ссылочный тип не существует или определен после `collectionSchema`

**Решение**: Определите тип до блока `collectionSchema`

```graphql
# Сначала определяем
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
}

# Затем ссылаемся
collectionSchema {
    type User {
        ref: UserItemData    # ✓ Теперь найден
        ...
    }
}
```

#### Ошибка 2: "Unexpected directive 'X' at line Y"

**Проблема**: Синтаксическая ошибка в структуре `collectionSchema`

**Решение**: Следуйте точному синтаксису:

```graphql
collectionSchema {
    type EntityName {        # Необходимо использовать ключевое слово "type"
        ref: TypeName
        operations {
            # операции здесь
        }
    }
}
```

#### Ошибка 3: Отсутствующие операции

**Проблема**: Операция, на которую ссылается `collectionSchema`, не определена в Query/Mutation

**Решение**: Убедитесь, что все ссылочные операции существуют:

```graphql
collectionSchema {
    type User {
        operations {
            list: UsersList    # Должна существовать в Query
        }
    }
}

type Query {
    UsersList(input: UsersListInput): UsersListPayload!  # ✓ Определена
}
```

### Лучшие практики

1. **Соглашения об именовании**:
   - Тип коллекции: `<Сущность>` (например, `User`, `License`)
   - Тип ref для списка: `<Сущность>ItemData` или `<Сущность>Item`
   - Тип ref для деталей: `<Сущность>Data` или `<Сущность>DefinitionData`

2. **Разделение типов**:
   - Используйте облегченные типы для списков (только необходимые поля)
   - Используйте полные типы для детальных операций (все поля)

3. **Документирование**:
   - Добавляйте комментарии, объясняющие назначение каждой коллекции
   - Документируйте особые отношения или ограничения

4. **Согласованность**:
   - Следуйте установленным паттернам в существующих SDL файлах
   - Поддерживайте согласованное именование операций между коллекциями

### Примеры из реального кода

См. эти файлы в репозитории для производственных примеров:

- **`Sdl/imtauth/1.0/Users.sdl`**: Коллекция пользователей
- **`Sdl/imtauth/1.0/Roles.sdl`**: Коллекция ролей
- **`Sdl/imtauth/1.0/Groups.sdl`**: Коллекция групп
- **`Sdl/imtlic/1.0/Licenses.sdl`**: Коллекция лицензий
- **`Sdl/imtlic/1.0/Products.sdl`**: Коллекция продуктов

### Связанная документация

- [Руководство по атрибуту ref в SDL](./SDL_Reference_Attribute_Guide.md) - Подробная документация атрибута `ref` (на русском)
- [Быстрая справка по SDL](./SDL_Ref_Quick_Reference.md) - Справочная карточка
- [Примеры SDL](./SDL_Ref_Examples.md) - Практические примеры
- [Диаграммы SDL](./SDL_Ref_Diagrams.md) - Визуальные диаграммы
- [Индекс документации SDL](./SDL_Documentation_Index.md) - Полный индекс документации

---

## Additional Resources / Дополнительные ресурсы

### Source Code References / Ссылки на исходный код

- **Parser Implementation**: `Include/imtsdl/CGqlExtSchemaParser.cpp:53-178`
- **Document Type Class**: `Include/imtsdl/CSdlDocumentType.h`
- **Code Generators**: 
  - C++: `Include/imtsdlgencpp/CGqlCollectionControllerBaseClassGeneratorComp.cpp`
  - QML: `Include/imtsdlgenqml/`

### Command-Line Tools / Инструменты командной строки

```bash
# Find all SDL files with collectionSchema
# Найти все SDL файлы с collectionSchema
find Sdl -name "*.sdl" -exec grep -l "collectionSchema" {} \;

# Generate code from SDL
# Генерировать код из SDL
SdlCodeGenerator --input path/to/file.sdl --output path/to/output
```

---

## Version History / История версий

- **2026-02-13**: Initial creation - comprehensive collectionSchema documentation
- **2026-02-13**: Первоначальное создание - полная документация collectionSchema

---

## Feedback / Обратная связь

For questions or suggestions about this documentation:
Для вопросов или предложений по этой документации:

- Open an issue in the ImtCore repository
- Откройте issue в репозитории ImtCore
- Contact the ImtCore development team
- Свяжитесь с командой разработки ImtCore
