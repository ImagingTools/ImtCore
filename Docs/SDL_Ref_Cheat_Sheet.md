# SDL "ref" Attribute - Cheat Sheet
# Атрибут "ref" в SDL - Шпаргалка

Quick reference for daily use of the `ref` attribute in ImtCore SDL schemas.
Быстрая справка для ежедневного использования атрибута `ref` в SDL схемах ImtCore.

---

## 📋 Basic Template / Базовый шаблон

```graphql
# 1. Define types FIRST / Сначала определите типы
type <Entity>ItemData {
    id: ID!                    # ✓ Required
    typeId: ID!                # ✓ Required
    name: String!
    # ... minimal fields for list
}

type <Entity>Data {
    id: ID
    name: String
    # ... all fields for editing
}

# 2. Define operations / Определите операции
input <Entity>ListInput {
    viewParams: CollectionViewParams
}

type <Entity>ListPayload {
    items: [<Entity>ItemData]  # ← Must match ref
    notification: NotificationItem
}

input <Entity>ItemInput {
    id: ID!
}

input <Entity>DataInput {
    id: ID!
    item: <Entity>Data!
}

type Query {
    <Entity>List(input: <Entity>ListInput): <Entity>ListPayload!
    <Entity>Item(input: <Entity>ItemInput): <Entity>Data!
}

type Mutation {
    <Entity>Add(input: <Entity>DataInput): AddedNotificationPayload!
    <Entity>Update(input: <Entity>DataInput): UpdatedNotificationPayload!
}

# 3. Link with ref / Свяжите через ref
collectionSchema {
    type <Entity> {
        ref: <Entity>ItemData          # ← List type
        operations {
            list: <Entity>List
        }
        subtype <Entity> {
            ref: <Entity>Data          # ← Detail type
            operations {
                get: <Entity>Item
                update: <Entity>Update
                insert: <Entity>Add
            }
        }
    }
}
```

---

## ⚡ Quick Rules / Быстрые правила

| Rule / Правило | Description / Описание |
|----------------|------------------------|
| **Define Before Use** | Types must exist before `ref` / Типы должны существовать до `ref` |
| **Required Fields** | `id: ID!` and `typeId: ID!` in ref types / Обязательные поля в ref типах |
| **Type Match** | Operation return type = ref type / Тип возврата = ref тип |
| **Import First** | Import dependencies before using types / Импортируйте зависимости |

---

## 🎯 Common Patterns / Общие паттерны

### Pattern 1: Simple Collection
```graphql
type UserItemData { id: ID!, typeId: ID!, name: String }
type UserData { id: ID, name: String, email: String }

collectionSchema {
    type User {
        ref: UserItemData
        operations { list: UsersList }
        subtype User {
            ref: UserData
            operations { get: UserItem, update: UserUpdate, insert: UserAdd }
        }
    }
}
```

### Pattern 2: Hierarchical
```graphql
type FolderItemData { id: ID!, typeId: ID!, parentId: ID }
type FolderData { id: ID, parentId: ID, childrenIds: [ID] }

collectionSchema {
    type Folder {
        ref: FolderItemData
        operations { list: FolderList }
        subtype Folder {
            ref: FolderData
            operations { get: FolderItem, update: FolderUpdate }
        }
    }
}
```

### Pattern 3: With Nested Objects
```graphql
type AddressInfo { street: String, city: String }
type PersonItemData { id: ID!, typeId: ID!, name: String }
type PersonData { id: ID, name: String, address: AddressInfo }

collectionSchema {
    type Person {
        ref: PersonItemData
        operations { list: PersonList }
        subtype Person {
            ref: PersonData
            operations { get: PersonItem, update: PersonUpdate }
        }
    }
}
```

### Pattern 4: Union Types
```graphql
union MediaItem = ImageItem | VideoItem | DocumentItem

collectionSchema {
    type Media {
        ref: MediaItem          # ← Can reference union!
        operations { list: MediaList }
    }
}
```

---

## 🚫 Common Mistakes / Частые ошибки

### ❌ Mistake 1: Type not defined
```graphql
collectionSchema {
    type User {
        ref: UserItemData    # ❌ Error if type not defined above
    }
}
```
**Fix / Исправление:**
```graphql
type UserItemData { ... }    # ✓ Define first
collectionSchema {
    type User {
        ref: UserItemData    # ✓ Now OK
    }
}
```

### ❌ Mistake 2: Missing required fields
```graphql
type UserItemData {
    name: String             # ❌ Missing id and typeId
}
```
**Fix / Исправление:**
```graphql
type UserItemData {
    id: ID!                  # ✓ Add required fields
    typeId: ID!              # ✓
    name: String
}
```

### ❌ Mistake 3: Type mismatch
```graphql
type UsersPayload {
    items: [UserData]        # ❌ Wrong type
}
collectionSchema {
    type User {
        ref: UserItemData    # ← Expects this
        operations {
            list: UsersList  # Returns UsersPayload with [UserData]
        }
    }
}
```
**Fix / Исправление:**
```graphql
type UsersPayload {
    items: [UserItemData]    # ✓ Matches ref
}
```

---

## 🔧 Checklist / Контрольный список

Before committing SDL changes / Перед коммитом изменений SDL:

- [ ] All ref types defined before collectionSchema?
- [ ] All ref types have `id: ID!` and `typeId: ID!`?
- [ ] Operation return types match ref types?
- [ ] Imported all dependencies?
- [ ] Tested code generation?
- [ ] No duplicate type definitions?

---

## 💡 Tips / Советы

### Naming Convention / Конвенция именования
```
<Entity>ItemData    → For lists (lightweight)
<Entity>Data        → For details (complete)
<Entity>ListInput   → Input for list query
<Entity>ListPayload → Output for list query
<Entity>ItemInput   → Input for get query
<Entity>DataInput   → Input for mutations
```

### Optimization / Оптимизация
```graphql
# List type - counts, not arrays
type TaskItemData {
    commentsCount: Integer    # ✓ Fast
    # NOT: comments: [Comment]  ❌ Slow
}

# Detail type - full data
type TaskData {
    comments: [Comment]       # ✓ Complete
}
```

### Import Pattern / Паттерн импорта
```graphql
import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtBaseTypes.sdl"
}
```

---

## 📚 Quick Links / Быстрые ссылки

- **Full Guide**: [SDL_Reference_Attribute_Guide.md](./SDL_Reference_Attribute_Guide.md) - Complete documentation (21KB)
- **Examples**: [SDL_Ref_Examples.md](./SDL_Ref_Examples.md) - 5 practical examples (23KB)
- **Diagrams**: [SDL_Ref_Diagrams.md](./SDL_Ref_Diagrams.md) - 8 visual diagrams (21KB)
- **Quick Ref**: [SDL_Ref_Quick_Reference.md](./SDL_Ref_Quick_Reference.md) - Brief reference (3KB)

---

## 🔍 Find More Examples / Найти больше примеров

```bash
# Find SDL files with collectionSchema
find Sdl -name "*.sdl" -exec grep -l "collectionSchema" {} \;

# Find all ref usages
grep -r "ref:" Sdl --include="*.sdl"

# View specific example
cat Sdl/imtauth/1.0/Users.sdl
cat Sdl/imtlic/1.0/Licenses.sdl
```

---

## 📞 Need Help? / Нужна помощь?

1. Check the full guide: [SDL_Reference_Attribute_Guide.md](./SDL_Reference_Attribute_Guide.md)
2. Review examples: [SDL_Ref_Examples.md](./SDL_Ref_Examples.md)
3. Look at existing SDL files in `Sdl/` directory
4. Check parser code: `Include/imtsdl/CGqlExtSchemaParser.cpp:136-178`

---

## 📝 Version History / История версий

- **2026-02-13**: Initial cheat sheet creation
- Based on analysis of ImtCore codebase

---

**Keep this cheat sheet handy for quick SDL development!**
**Держите эту шпаргалку под рукой для быстрой разработки SDL!**
