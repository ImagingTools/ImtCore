# SDL "ref" Attribute - Practical Examples
# Практические примеры использования атрибута "ref" в SDL

This file contains comprehensive examples demonstrating various use cases of the `ref` attribute in ImtCore SDL schemas.

---

## Example 1: Simple Collection (Blog Posts)
## Пример 1: Простая коллекция (Блог посты)

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: myblog
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# Type for list view - lightweight data
# Тип для списка - облегченные данные
type BlogPostItemData {
    id: ID!
    typeId: ID!
    title: String!
    author: String
    publishedDate: String
    excerpt: String           # Short preview
    commentsCount: Integer
}

# Type for detailed view - complete data
# Тип для детального просмотра - полные данные
type BlogPostData {
    id: ID
    title: String
    author: String
    publishedDate: String
    content: String          # Full content
    tags: [String]
    categories: [ID]
    commentsEnabled: Boolean
}

input BlogPostsListInput {
    viewParams: CollectionViewParams
}

type BlogPostsListPayload {
    items: [BlogPostItemData]
    notification: NotificationItem
}

input BlogPostItemInput {
    id: ID!
}

input BlogPostDataInput {
    id: ID!
    item: BlogPostData!
}

type Query {
    BlogPostsList(input: BlogPostsListInput): BlogPostsListPayload!
    BlogPostItem(input: BlogPostItemInput): BlogPostData!
}

type Mutation {
    BlogPostAdd(input: BlogPostDataInput): AddedNotificationPayload!
    BlogPostUpdate(input: BlogPostDataInput): UpdatedNotificationPayload!
}

collectionSchema {
    type BlogPost {
        # ref points to lightweight list type
        # ref указывает на облегченный тип для списка
        ref: BlogPostItemData
        operations {
            list: BlogPostsList
        }
        subtype BlogPost {
            # ref points to complete data type
            # ref указывает на полный тип данных
            ref: BlogPostData
            operations {
                get: BlogPostItem
                update: BlogPostUpdate
                insert: BlogPostAdd
            }
        }
    }
}
```

**Использование / Usage:**
- `ref: BlogPostItemData` - используется для быстрого отображения списка постов
- `ref: BlogPostData` - используется для редактирования и детального просмотра

---

## Example 2: Hierarchical Collection (File System)
## Пример 2: Иерархическая коллекция (Файловая система)

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: myfs
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# Enumeration for file types
# Перечисление типов файлов
enum FileSystemItemType {
    FILE
    FOLDER
    SYMLINK
}

# List view type - shows hierarchical structure
# Тип для списка - показывает иерархическую структуру
type FileSystemItemData {
    id: ID!
    typeId: ID!
    name: String!
    itemType: FileSystemItemType!
    parentId: ID               # Reference to parent folder
    size: Integer
    modifiedDate: String
    icon: String
}

# Detail type - complete file/folder information
# Детальный тип - полная информация о файле/папке
type FileSystemData {
    id: ID
    name: String
    itemType: FileSystemItemType
    parentId: ID
    path: String               # Full path
    size: Integer
    createdDate: String
    modifiedDate: String
    permissions: String
    owner: String
    content: String            # For files
    childrenIds: [ID]          # For folders
}

input FileSystemListInput {
    parentId: ID               # List files in specific folder
    viewParams: CollectionViewParams
}

type FileSystemListPayload {
    items: [FileSystemItemData]
    notification: NotificationItem
}

input FileSystemItemInput {
    id: ID!
}

input FileSystemDataInput {
    id: ID!
    item: FileSystemData!
}

type Query {
    FileSystemList(input: FileSystemListInput): FileSystemListPayload!
    FileSystemItem(input: FileSystemItemInput): FileSystemData!
}

type Mutation {
    FileSystemAdd(input: FileSystemDataInput): AddedNotificationPayload!
    FileSystemUpdate(input: FileSystemDataInput): UpdatedNotificationPayload!
}

collectionSchema {
    type FileSystem {
        # ref for hierarchical list data
        # ref для иерархических данных списка
        ref: FileSystemItemData
        operations {
            list: FileSystemList
        }
        subtype FileSystem {
            # ref for complete file/folder data
            # ref для полных данных файла/папки
            ref: FileSystemData
            operations {
                get: FileSystemItem
                update: FileSystemUpdate
                insert: FileSystemAdd
            }
        }
    }
}
```

**Особенности / Features:**
- Поддержка иерархии через `parentId` и `childrenIds`
- Различные типы элементов через `FileSystemItemType` enum
- Минимальные данные в `ItemData` для быстрой навигации
- Полные данные в `Data` для редактирования

---

## Example 3: Collection with Nested Objects (E-commerce Orders)
## Пример 3: Коллекция с вложенными объектами (Заказы электронной коммерции)

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: ecommerce
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

enum OrderStatus {
    PENDING
    PROCESSING
    SHIPPED
    DELIVERED
    CANCELLED
}

# Nested type for order items
# Вложенный тип для позиций заказа
type OrderItemInfo {
    productId: ID!
    productName: String!
    quantity: Integer!
    price: Float!
    discount: Float
}

# Nested type for shipping address
# Вложенный тип для адреса доставки
type ShippingAddress {
    street: String!
    city: String!
    state: String!
    zipCode: String!
    country: String!
}

# List view - summary information
# Список - краткая информация
type OrderItemData {
    id: ID!
    typeId: ID!
    orderNumber: String!
    customerId: ID!
    customerName: String
    status: OrderStatus!
    totalAmount: Float!
    itemsCount: Integer
    orderDate: String
}

# Detail view - complete order with nested data
# Детальный вид - полный заказ с вложенными данными
type OrderData {
    id: ID
    orderNumber: String
    customerId: ID
    customerName: String
    customerEmail: String
    status: OrderStatus
    items: [OrderItemInfo]!        # Nested array
    shippingAddress: ShippingAddress  # Nested object
    billingAddress: ShippingAddress   # Nested object
    subtotal: Float
    tax: Float
    shippingCost: Float
    totalAmount: Float
    orderDate: String
    notes: String
}

input OrdersListInput {
    customerId: ID                 # Filter by customer
    status: OrderStatus            # Filter by status
    viewParams: CollectionViewParams
}

type OrdersListPayload {
    items: [OrderItemData]
    notification: NotificationItem
}

input OrderItemInput {
    id: ID!
}

input OrderDataInput {
    id: ID!
    item: OrderData!
}

type Query {
    OrdersList(input: OrdersListInput): OrdersListPayload!
    OrderItem(input: OrderItemInput): OrderData!
}

type Mutation {
    OrderAdd(input: OrderDataInput): AddedNotificationPayload!
    OrderUpdate(input: OrderDataInput): UpdatedNotificationPayload!
}

collectionSchema {
    type Order {
        # ref to summary type for list
        # ref на краткий тип для списка
        ref: OrderItemData
        operations {
            list: OrdersList
        }
        subtype Order {
            # ref to complete type with nested objects
            # ref на полный тип с вложенными объектами
            ref: OrderData
            operations {
                get: OrderItem
                update: OrderUpdate
                insert: OrderAdd
            }
        }
    }
}
```

**Преимущества / Benefits:**
- `OrderItemData` оптимизирован для отображения списка заказов
- `OrderData` содержит вложенные объекты (`items`, `shippingAddress`)
- Четкое разделение между представлением списка и детальным представлением

---

## Example 4: Multi-Type Collection Using Unions
## Пример 4: Мультитиповая коллекция с использованием Union

```graphql
schema {
    query: Query
    mutation: Mutation
    namespace: media
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

# Base fields for all media items
# Базовые поля для всех медиа элементов
interface MediaItemBase {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    uploadDate: String
    size: Integer
}

# Image specific data
# Данные специфичные для изображений
type ImageItemData implements MediaItemBase {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    uploadDate: String
    size: Integer
    width: Integer
    height: Integer
    format: String
    thumbnail: String
}

# Video specific data
# Данные специфичные для видео
type VideoItemData implements MediaItemBase {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    uploadDate: String
    size: Integer
    duration: Integer
    resolution: String
    format: String
    thumbnail: String
}

# Document specific data
# Данные специфичные для документов
type DocumentItemData implements MediaItemBase {
    id: ID!
    typeId: ID!
    name: String!
    description: String
    uploadDate: String
    size: Integer
    format: String
    pages: Integer
}

# Union type combining all media types
# Union тип объединяющий все типы медиа
union MediaItem = ImageItemData | VideoItemData | DocumentItemData

# Detailed data types
# Типы детальных данных
type ImageData {
    id: ID
    name: String
    description: String
    width: Integer
    height: Integer
    format: String
    fileData: String
    metadata: String
}

type VideoData {
    id: ID
    name: String
    description: String
    duration: Integer
    resolution: String
    format: String
    fileData: String
    subtitles: String
}

type DocumentData {
    id: ID
    name: String
    description: String
    format: String
    pages: Integer
    fileData: String
    textContent: String
}

union MediaData = ImageData | VideoData | DocumentData

input MediaListInput {
    mediaType: String          # Filter by type: "image", "video", "document"
    viewParams: CollectionViewParams
}

type MediaListPayload {
    items: [MediaItem]         # Union type array
    notification: NotificationItem
}

input MediaItemInput {
    id: ID!
}

type Query {
    MediaList(input: MediaListInput): MediaListPayload!
    # Separate queries for each type
    ImageItem(input: MediaItemInput): ImageData!
    VideoItem(input: MediaItemInput): VideoData!
    DocumentItem(input: MediaItemInput): DocumentData!
}

collectionSchema {
    type Media {
        # ref can point to a union type!
        # ref может указывать на union тип!
        ref: MediaItem
        operations {
            list: MediaList
        }
        # Different subtypes for different media types
        # Разные подтипы для разных типов медиа
        subtype Image {
            ref: ImageData
            operations {
                get: ImageItem
            }
        }
        subtype Video {
            ref: VideoData
            operations {
                get: VideoItem
            }
        }
        subtype Document {
            ref: DocumentData
            operations {
                get: DocumentItem
            }
        }
    }
}
```

**Ключевые моменты / Key Points:**
- `ref: MediaItem` указывает на union тип
- Парсер поддерживает union через `SetReferenceType(const CSdlUnion&)`
- Разные подтипы для разных категорий медиа
- Использование interface для общих полей

---

## Example 5: Real-World: Task Management System
## Пример 5: Реальный пример: Система управления задачами

```graphql
schema {
    query: Query
    mutation: Mutation
    subscription: Subscription
    namespace: taskmanager
    version: 1.0
}

import {
    "../../../../ImtCore/Sdl/imtbase/1.0/ImtCollection.sdl"
}

enum TaskPriority {
    LOW
    MEDIUM
    HIGH
    CRITICAL
}

enum TaskStatus {
    TODO
    IN_PROGRESS
    IN_REVIEW
    DONE
    CANCELLED
}

# Nested type for task assignee
# Вложенный тип для исполнителя задачи
type TaskAssignee {
    userId: ID!
    userName: String!
    userEmail: String
}

# Nested type for task comments
# Вложенный тип для комментариев к задаче
type TaskComment {
    id: ID!
    authorId: ID!
    authorName: String!
    text: String!
    createdDate: String!
}

# Nested type for subtasks
# Вложенный тип для подзадач
type SubTask {
    id: ID!
    title: String!
    completed: Boolean!
}

# List view - optimized for kanban board/list display
# Список - оптимизирован для канбан доски/отображения списка
type TaskItemData {
    id: ID!
    typeId: ID!
    title: String!
    description: String         # Short description
    projectId: ID
    projectName: String
    assigneeId: ID
    assigneeName: String
    priority: TaskPriority!
    status: TaskStatus!
    dueDate: String
    createdDate: String
    tags: [String]
    subtasksCount: Integer
    commentsCount: Integer
}

# Detail view - complete task information
# Детальный вид - полная информация о задаче
type TaskData {
    id: ID
    title: String
    description: String         # Full description with markdown
    projectId: ID
    assignees: [TaskAssignee]   # Multiple assignees
    priority: TaskPriority
    status: TaskStatus
    dueDate: String
    createdDate: String
    modifiedDate: String
    completedDate: String
    tags: [String]
    attachments: [ID]
    parentTaskId: ID            # For hierarchical tasks
    subtasks: [SubTask]         # Nested subtasks
    comments: [TaskComment]     # Nested comments
    estimatedHours: Float
    actualHours: Float
    customFields: String        # JSON string for extensibility
}

input TasksListInput {
    projectId: ID
    assigneeId: ID
    status: TaskStatus
    priority: TaskPriority
    viewParams: CollectionViewParams
}

type TasksListPayload {
    items: [TaskItemData]
    notification: NotificationItem
}

input TaskItemInput {
    id: ID!
    projectId: ID
}

input TaskDataInput {
    id: ID!
    typeId: ID!
    projectId: ID!
    item: TaskData!
    name: String
    description: String
}

input AddTaskCommentInput {
    taskId: ID!
    text: String!
}

type AddTaskCommentPayload {
    commentId: ID!
}

input UpdateTaskStatusInput {
    taskId: ID!
    status: TaskStatus!
}

type UpdateTaskStatusPayload {
    success: Boolean!
}

type Query {
    TasksList(input: TasksListInput): TasksListPayload!
    TaskItem(input: TaskItemInput): TaskData!
}

type Mutation {
    TaskAdd(input: TaskDataInput): AddedNotificationPayload!
    TaskUpdate(input: TaskDataInput): UpdatedNotificationPayload!
    AddTaskComment(input: AddTaskCommentInput): AddTaskCommentPayload!
    UpdateTaskStatus(input: UpdateTaskStatusInput): UpdateTaskStatusPayload!
}

type Subscription {
    TaskUpdated(taskId: ID!): TaskData!
    TaskStatusChanged(projectId: ID!): TaskItemData!
}

collectionSchema {
    type Task {
        # ref points to compact list representation
        # ref указывает на компактное представление списка
        ref: TaskItemData
        operations {
            list: TasksList
        }
        subtype Task {
            # ref points to complete task with nested data
            # ref указывает на полную задачу с вложенными данными
            ref: TaskData
            operations {
                get: TaskItem
                update: TaskUpdate
                insert: TaskAdd
            }
        }
    }
}
```

**Применение в реальном мире / Real-World Application:**
- **TaskItemData**: Используется для отображения в канбан-доске, списках задач
  - Минимальные данные для быстрой загрузки
  - Счетчики (subtasksCount, commentsCount) вместо полных данных
  
- **TaskData**: Используется при открытии детальной карточки задачи
  - Полные вложенные данные (assignees, subtasks, comments)
  - Все поля для редактирования
  - Поддержка пользовательских полей через customFields

---

## Best Practices Demonstrated
## Продемонстрированные лучшие практики

### 1. Type Naming Convention
### 1. Конвенция именования типов

```graphql
<Entity>ItemData    // For list views (lightweight)
<Entity>Data        // For detail views (complete)
```

### 2. Field Optimization
### 2. Оптимизация полей

```graphql
// List type - counts instead of arrays
type TaskItemData {
    subtasksCount: Integer      // ✓ Lightweight
    commentsCount: Integer      // ✓ Lightweight
}

// Detail type - full arrays
type TaskData {
    subtasks: [SubTask]         // ✓ Complete data
    comments: [TaskComment]     // ✓ Complete data
}
```

### 3. Nested Types Organization
### 3. Организация вложенных типов

```graphql
// Define nested types before main types
type TaskAssignee { ... }
type TaskComment { ... }
type SubTask { ... }

// Then use them in main types
type TaskData {
    assignees: [TaskAssignee]
    comments: [TaskComment]
    subtasks: [SubTask]
}
```

### 4. Operation-Type Alignment
### 4. Выравнивание операций и типов

```graphql
type TasksListPayload {
    items: [TaskItemData]    // Matches ref: TaskItemData
}

collectionSchema {
    type Task {
        ref: TaskItemData    // ← Alignment
        operations {
            list: TasksList  // Returns TasksListPayload
        }
    }
}
```

---

## Testing Your ref Usage
## Тестирование использования ref

### Checklist:
### Контрольный список:

- [ ] Are all ref types defined before collectionSchema?
      Все ref типы определены до collectionSchema?
      
- [ ] Do ref types have required fields (id, typeId)?
      Есть ли у ref типов обязательные поля (id, typeId)?
      
- [ ] Do operation return types match ref types?
      Совпадают ли возвращаемые типы операций с ref типами?
      
- [ ] Are list types optimized (minimal fields)?
      Оптимизированы ли типы списков (минимум полей)?
      
- [ ] Are detail types complete (all needed fields)?
      Полные ли детальные типы (все нужные поля)?
      
- [ ] Are nested types defined before parent types?
      Определены ли вложенные типы до родительских типов?

---

## Common Mistakes and Fixes
## Распространенные ошибки и исправления

### Mistake 1: Using ref Before Type Definition
### Ошибка 1: Использование ref до определения типа

```graphql
# ✗ WRONG
collectionSchema {
    type User {
        ref: UserItemData    // Error: type not found!
    }
}

type UserItemData { ... }

# ✓ CORRECT
type UserItemData { ... }

collectionSchema {
    type User {
        ref: UserItemData    // ✓ Type is defined
    }
}
```

### Mistake 2: Missing Required Fields
### Ошибка 2: Отсутствие обязательных полей

```graphql
# ✗ WRONG
type UserItemData {
    name: String       // Missing id and typeId!
}

# ✓ CORRECT
type UserItemData {
    id: ID!            // ✓ Required
    typeId: ID!        // ✓ Required
    name: String
}
```

### Mistake 3: Type Mismatch in Operations
### Ошибка 3: Несоответствие типов в операциях

```graphql
# ✗ WRONG
type UsersListPayload {
    items: [UserData]        // Wrong type!
}

collectionSchema {
    type User {
        ref: UserItemData    // Mismatch!
        operations {
            list: UsersList  // Returns UsersListPayload
        }
    }
}

# ✓ CORRECT
type UsersListPayload {
    items: [UserItemData]    // ✓ Matches ref
}

collectionSchema {
    type User {
        ref: UserItemData    // ✓ Aligned
        operations {
            list: UsersList
        }
    }
}
```

---

## Summary
## Резюме

The `ref` attribute is fundamental to ImtCore's SDL schema system:
Атрибут `ref` является основополагающим в системе SDL схем ImtCore:

1. **Links collections to GraphQL types**
   Связывает коллекции с GraphQL типами
   
2. **Enables code generation**
   Обеспечивает генерацию кода
   
3. **Supports both simple types and unions**
   Поддерживает как простые типы, так и union
   
4. **Follows clear naming conventions**
   Следует четким конвенциям именования
   
5. **Optimizes data transfer** (list vs detail types)
   Оптимизирует передачу данных (типы списка vs детальные типы)

For more information, see:
Для дополнительной информации см.:
- [Complete SDL Reference Guide](./SDL_Reference_Attribute_Guide.md) (Полное руководство на русском, 21KB)
- [Quick Reference](./SDL_Ref_Quick_Reference.md) (Краткая справка на английском, 3KB)
- [Visual Diagrams](./SDL_Ref_Diagrams.md) (Визуальные диаграммы, 21KB)
