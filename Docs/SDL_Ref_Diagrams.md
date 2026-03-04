# SDL Schema "ref" Attribute - Visual Diagrams
# Визуальные диаграммы атрибута "ref" в SDL схеме

This document provides visual representations to help understand the `ref` attribute in SDL schemas.
Этот документ содержит визуальные представления для понимания атрибута `ref` в SDL схемах.

---

## Diagram 1: Basic ref Structure
## Диаграмма 1: Базовая структура ref

```
┌─────────────────────────────────────────────────────────────┐
│                      SDL Schema File                         │
│                                                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ GraphQL Type Definitions                              │  │
│  │                                                        │  │
│  │  type UserItemData {     type UserData {             │  │
│  │      id: ID!                 id: ID                   │  │
│  │      typeId: ID!             name: String             │  │
│  │      name: String            email: String            │  │
│  │  }                       }                            │  │
│  └───────────────────────────────────────────────────────┘  │
│                           │                                  │
│                           │ References                       │
│                           ↓                                  │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ collectionSchema {                                    │  │
│  │     type User {                                       │  │
│  │         ref: UserItemData  ←─────────┐               │  │
│  │         operations { list: ... }      │               │  │
│  │                                       │               │  │
│  │         subtype User {                │               │  │
│  │             ref: UserData ←───────────┤               │  │
│  │             operations { get: ... }   │               │  │
│  │         }                              │               │  │
│  │     }                                  │               │  │
│  │ }                                      │               │  │
│  └────────────────────────────────────────┘               │  │
│                                           │               │  │
│                                           │ Links to      │  │
│                                           └───────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

**Explanation / Объяснение:**
- `ref` creates a link from collectionSchema to GraphQL type definitions
- `ref` создает связь от collectionSchema к определениям GraphQL типов

---

## Diagram 2: Data Flow with ref
## Диаграмма 2: Поток данных с ref

```
Client Request                SDL Schema                  Generated Code
Запрос клиента               SDL схема                   Сгенерированный код
      │                           │                            │
      │                           │                            │
      ▼                           │                            │
┌──────────┐                      │                            │
│ List     │                      │                            │
│ Users    │                      │                            │
└──────────┘                      │                            │
      │                           │                            │
      │                           ▼                            │
      │                    ┌──────────────┐                    │
      │                    │ ref:         │                    │
      │                    │ UserItemData │───────────────────▶│
      │                    └──────────────┘                    │
      │                           │                            │
      │                           │                            ▼
      │                           │                    ┌───────────────┐
      │                           │                    │ UsersList()   │
      │                           │                    │ Returns:      │
      ◀───────────────────────────┴────────────────────│ [UserItemData]│
      │                                                └───────────────┘
      │
      ▼
┌──────────┐
│ Get      │
│ User     │
│ Details  │
└──────────┘
      │
      │                           ┌──────────────┐
      │                           │ subtype ref: │
      │                           │ UserData     │───────────────────▶
      │                           └──────────────┘
      │                                  │
      │                                  │                    ┌───────────────┐
      │                                  │                    │ UserItem()    │
      │                                  │                    │ Returns:      │
      ◀──────────────────────────────────┴────────────────────│ UserData      │
                                                              └───────────────┘

Legend / Легенда:
─────▶  Data flow / Поток данных
─────│  Processing / Обработка
```

---

## Diagram 3: Parser Processing
## Диаграмма 3: Обработка парсером

```
SDL File (.sdl)
    │
    │ Read
    ▼
┌─────────────────────────────────────────┐
│ CGqlExtSchemaParser                      │
│                                          │
│ 1. Parse type definitions               │
│    ┌────────────────────────────────┐   │
│    │ m_sdlTypes: [UserItemData,     │   │
│    │              UserData, ...]     │   │
│    └────────────────────────────────┘   │
│                                          │
│ 2. Parse collectionSchema               │
│    ┌────────────────────────────────┐   │
│    │ if (keyword == "ref") {         │   │
│    │   typeRefName = "UserItemData"  │   │
│    │                                 │   │
│    │   // Search in types            │   │
│    │   found = find(m_sdlTypes,      │   │
│    │               typeRefName)      │   │
│    │                                 │   │
│    │   documentType.SetReferenceType │   │
│    │        (*found)                 │   │
│    │ }                               │   │
│    └────────────────────────────────┘   │
│                                          │
│ 3. Store in CSdlDocumentType            │
│    ┌────────────────────────────────┐   │
│    │ m_referenceType: UserItemData   │   │
│    │ m_operationsList: [...]         │   │
│    └────────────────────────────────┘   │
└─────────────────────────────────────────┘
    │
    │ Output
    ▼
CSdlDocumentType object
    │
    │ Used by
    ▼
Code Generators (imtsdlgencpp, imtsdlgenqml)
```

---

## Diagram 4: Type Hierarchy
## Диаграмма 4: Иерархия типов

```
collectionSchema
│
└── type User
    │
    ├── ref: UserItemData ────────────┐
    │                                  │
    ├── operations                     │
    │   └── list: UsersList            │
    │                                  │
    └── subtype User                   │
        │                              │
        ├── ref: UserData ─────────┐   │
        │                          │   │
        └── operations             │   │
            ├── get: UserItem      │   │
            ├── update: ...        │   │
            └── insert: ...        │   │
                                   │   │
        ┌──────────────────────────┘   │
        │                              │
        ▼                              ▼
    UserData                     UserItemData
    ├── id: ID                   ├── id: ID!
    ├── name: String             ├── typeId: ID!
    ├── email: String            ├── name: String
    ├── groups: [ID]             └── (minimal fields)
    └── (all fields)


Operation Usage / Использование операций:
────────────────────────────────────────

UsersList query returns:         UserItem query returns:
    └─▶ [UserItemData]               └─▶ UserData

```

---

## Diagram 5: List vs Detail Pattern
## Диаграмма 5: Паттерн Список vs Детали

```
┌───────────────────────────────────────────────────────────────────┐
│                        Client Application                          │
│                                                                    │
│  ┌─────────────────────┐              ┌────────────────────────┐  │
│  │   List View         │              │   Detail View          │  │
│  │  ┌──────────────┐   │              │  ┌─────────────────┐  │  │
│  │  │ User 1       │   │              │  │ Name: John Doe  │  │  │
│  │  │ User 2       │   │──── Click ──▶│  │ Email: john@... │  │  │
│  │  │ User 3       │   │              │  │ Groups: [A, B]  │  │  │
│  │  │ ...          │   │              │  │ Roles: [Admin]  │  │  │
│  │  └──────────────┘   │              │  └─────────────────┘  │  │
│  └─────────────────────┘              └────────────────────────┘  │
│           │                                        │               │
│           │ Request List                           │ Request Details
│           ▼                                        ▼               │
└───────────────────────────────────────────────────────────────────┘
            │                                        │
            │                                        │
┌───────────────────────────────────────────────────────────────────┐
│                          GraphQL Server                            │
│           │                                        │               │
│           ▼                                        ▼               │
│  ┌─────────────────────┐              ┌────────────────────────┐  │
│  │ UsersList           │              │ UserItem               │  │
│  │  Query              │              │  Query                 │  │
│  └─────────────────────┘              └────────────────────────┘  │
│           │                                        │               │
│           │ Uses ref:                              │ Uses ref:     │
│           ▼ UserItemData                           ▼ UserData      │
│  ┌─────────────────────┐              ┌────────────────────────┐  │
│  │ Lightweight:        │              │ Complete:              │  │
│  │  - id               │              │  - id                  │  │
│  │  - typeId           │              │  - name                │  │
│  │  - name             │              │  - email               │  │
│  │  (3 fields)         │              │  - groups: [ID]        │  │
│  │                     │              │  - roles: [ID]         │  │
│  │ Fast load for 100s  │              │  (10+ fields)          │  │
│  │ of items            │              │                        │  │
│  └─────────────────────┘              │ Load only when needed  │  │
│                                       └────────────────────────┘  │
└───────────────────────────────────────────────────────────────────┘

Benefits / Преимущества:
─────────────────────────
List View                          Detail View
- Fast loading                     - Complete information
- Minimal bandwidth                - All fields for editing
- Optimized for display            - Rich nested data
- Быстрая загрузка                 - Полная информация
- Минимальный трафик               - Все поля для редактирования
- Оптимизировано для показа        - Богатые вложенные данные
```

---

## Diagram 6: Error Scenarios
## Диаграмма 6: Сценарии ошибок

```
┌──────────────────────────────────────────────────────────────┐
│ Scenario 1: Type Not Found                                   │
│ Сценарий 1: Тип не найден                                    │
└──────────────────────────────────────────────────────────────┘

SDL File:
    collectionSchema {
        type User {
            ref: UnknownType  ❌
        }
    }

Parser:
    1. Read: typeRefName = "UnknownType"
    2. Search in m_sdlTypes: NOT FOUND ❌
    3. Search in m_unions: NOT FOUND ❌
    4. Error: "Unable to find type 'UnknownType'"


┌──────────────────────────────────────────────────────────────┐
│ Scenario 2: Empty ref                                        │
│ Сценарий 2: Пустой ref                                       │
└──────────────────────────────────────────────────────────────┘

SDL File:
    collectionSchema {
        type User {
            ref:           ❌ (empty)
        }
    }

Parser:
    1. Read: typeRefName = ""
    2. Check: isEmpty() == true ❌
    3. Error: "Reference for 'User' at line X"


┌──────────────────────────────────────────────────────────────┐
│ Scenario 3: Type Mismatch                                    │
│ Сценарий 3: Несоответствие типов                             │
└──────────────────────────────────────────────────────────────┘

SDL File:
    type UsersListPayload {
        items: [UserData]  ❌ Wrong type!
    }
    
    collectionSchema {
        type User {
            ref: UserItemData
            operations {
                list: UsersList  ← Returns UsersListPayload
            }
        }
    }

Result:
    Mismatch between:
    - ref type: UserItemData
    - returned type: [UserData]
    
    Code generation issues ❌
```

---

## Diagram 7: Advanced: Union Types
## Диаграмма 7: Продвинутое: Union типы

```
┌──────────────────────────────────────────────────────────────────┐
│                    Union Type Structure                           │
│                    Структура Union типа                           │
└──────────────────────────────────────────────────────────────────┘

SDL Definition:
    type ImageItem {
        id: ID!
        typeId: ID!
        width: Integer
        height: Integer
    }
    
    type VideoItem {
        id: ID!
        typeId: ID!
        duration: Integer
    }
    
    type DocumentItem {
        id: ID!
        typeId: ID!
        pages: Integer
    }
    
    union MediaItem = ImageItem | VideoItem | DocumentItem
    
    collectionSchema {
        type Media {
            ref: MediaItem  ← Points to Union
            operations {
                list: MediaList
            }
        }
    }

Parser Processing:
    1. Parse union definition
       └─▶ m_unions.append(MediaItem)
    
    2. Parse collectionSchema
       └─▶ keyword == "ref"
           └─▶ typeRefName = "MediaItem"
               ├─▶ Search in m_sdlTypes: NOT FOUND
               └─▶ Search in m_unions: FOUND ✓
                   └─▶ documentType.SetReferenceType(MediaItem)

Result / Результат:
    MediaList returns array of MediaItem union:
        [
            ImageItem { ... },
            VideoItem { ... },
            DocumentItem { ... }
        ]
```

---

## Diagram 8: Code Generation Flow
## Диаграмма 8: Поток генерации кода

```
┌────────────────────────────────────────────────────────────────────┐
│                     SDL to C++ Code Generation                      │
│                     SDL → Генерация C++ кода                       │
└────────────────────────────────────────────────────────────────────┘

Users.sdl
    │
    │ Parsed by
    ▼
CGqlSchemaParserComp
    │
    │ Creates
    ▼
CSdlDocumentType
    ├─▶ m_name: "User"
    ├─▶ m_referenceType: UserItemData
    ├─▶ m_operationsList: [list → UsersList]
    └─▶ m_subtypes: [
            ├─▶ name: "User"
            ├─▶ m_referenceType: UserData
            └─▶ m_operationsList: [get → UserItem, ...]
        ]
    │
    │ Used by
    ▼
CGqlCollectionControllerBaseClassGeneratorComp
    │
    │ Generates
    ▼
┌────────────────────────────────────────────────────────────────┐
│ CUserCollectionControllerBase.h                                │
│                                                                │
│ class CUserCollectionControllerBase {                          │
│ public:                                                        │
│     // From ref: UserItemData                                 │
│     virtual QList<UserItemData> GetUsersList(               │
│         const CollectionViewParams& params) = 0;              │
│                                                                │
│     // From subtype ref: UserData                             │
│     virtual UserData GetUserItem(                            │
│         const QString& userId) = 0;                           │
│                                                                │
│     virtual bool UpdateUser(                                  │
│         const QString& userId,                                │
│         const UserData& data) = 0;                           │
│ };                                                             │
└────────────────────────────────────────────────────────────────┘

Key Mapping / Ключевое соответствие:
────────────────────────────────────
SDL ref: UserItemData    →    C++ return type: QList<UserItemData>
SDL ref: UserData        →    C++ return type: UserData
SDL operation: list      →    C++ method: GetUsersList()
SDL operation: get       →    C++ method: GetUserItem()
```

---

## Summary Diagram: Complete ref Workflow
## Итоговая диаграмма: Полный workflow ref

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Complete ref Workflow                         │
│                        Полный workflow ref                          │
└─────────────────────────────────────────────────────────────────────┘

Step 1: Define Types / Шаг 1: Определить типы
    ┌──────────────────────────────────────────────┐
    │ type UserItemData { id: ID!, ... }            │
    │ type UserData { id: ID, ... }                 │
    └──────────────────────────────────────────────┘
                      │
                      ▼
Step 2: Link via ref / Шаг 2: Связать через ref
    ┌──────────────────────────────────────────────┐
    │ collectionSchema {                            │
    │     type User {                               │
    │         ref: UserItemData  ← Link             │
    │         subtype User {                        │
    │             ref: UserData  ← Link             │
    │         }                                     │
    │     }                                         │
    │ }                                             │
    └──────────────────────────────────────────────┘
                      │
                      ▼
Step 3: Parse / Шаг 3: Парсинг
    ┌──────────────────────────────────────────────┐
    │ CGqlExtSchemaParser                           │
    │   ├─▶ Find type in m_sdlTypes                │
    │   └─▶ Call SetReferenceType()                │
    └──────────────────────────────────────────────┘
                      │
                      ▼
Step 4: Store / Шаг 4: Сохранение
    ┌──────────────────────────────────────────────┐
    │ CSdlDocumentType                              │
    │   └─▶ m_referenceType = UserItemData         │
    └──────────────────────────────────────────────┘
                      │
                      ▼
Step 5: Generate Code / Шаг 5: Генерация кода
    ┌──────────────────────────────────────────────┐
    │ Code Generators                               │
    │   ├─▶ Generate C++ classes                   │
    │   ├─▶ Generate QML models                    │
    │   └─▶ Generate GraphQL proxies               │
    └──────────────────────────────────────────────┘
                      │
                      ▼
Step 6: Runtime Usage / Шаг 6: Использование
    ┌──────────────────────────────────────────────┐
    │ Client App                                    │
    │   ├─▶ Call GetUsersList()                    │
    │   │   └─▶ Returns [UserItemData]             │
    │   └─▶ Call GetUserItem()                     │
    │       └─▶ Returns UserData                   │
    └──────────────────────────────────────────────┘

✓ Complete workflow from SDL to runtime
✓ Полный workflow от SDL до runtime
```

---

## Best Practices Visualization
## Визуализация лучших практик

```
┌─────────────────────────────────────────────────────────────────┐
│              DO's and DON'Ts / ЧТО ДЕЛАТЬ и НЕ ДЕЛАТЬ          │
└─────────────────────────────────────────────────────────────────┘

✓ DO / ДЕЛАТЬ:
───────────────

1. Define types BEFORE collectionSchema:
   Определять типы ДО collectionSchema:
   
   type UserItemData { ... }  ← First
   collectionSchema {
       ref: UserItemData      ← Second
   }


2. Include required fields:
   Включать обязательные поля:
   
   type UserItemData {
       id: ID!        ✓
       typeId: ID!    ✓
       ...
   }


3. Match operation return types:
   Сопоставлять типы возврата операций:
   
   type UsersListPayload {
       items: [UserItemData]  ← Matches
   }
   ref: UserItemData         ← ref


❌ DON'T / НЕ ДЕЛАТЬ:
──────────────────────

1. Don't use ref before type definition:
   Не использовать ref до определения типа:
   
   collectionSchema {
       ref: UserItemData  ❌ Not yet defined
   }
   type UserItemData { ... }


2. Don't forget required fields:
   Не забывать обязательные поля:
   
   type UserItemData {
       name: String   ❌ Missing id and typeId
   }


3. Don't mismatch types:
   Не несоответствовать типы:
   
   ref: UserItemData           ← Expects this
   items: [UserData]          ❌ But gets this
```

---

These diagrams should help visualize how the `ref` attribute works in ImtCore SDL schemas!
Эти диаграммы должны помочь визуализировать, как работает атрибут `ref` в SDL схемах ImtCore!

For more information, see:
Для дополнительной информации см.:
- [Complete SDL Reference Guide](./SDL_Reference_Attribute_Guide.md)
- [Quick Reference](./SDL_Ref_Quick_Reference.md)
- [Practical Examples](./SDL_Ref_Examples.md)
