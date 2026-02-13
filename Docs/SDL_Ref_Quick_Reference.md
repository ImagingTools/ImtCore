# SDL "ref" Attribute - Quick Reference

## Quick Summary

The **`ref`** attribute in ImtCore SDL schema links a document collection to a specific GraphQL type or union, defining the data structure for collection elements.

## Syntax

```graphql
collectionSchema {
    type <CollectionName> {
        ref: <TypeName>           // Links to a GraphQL type
        operations { ... }
        
        subtype <SubtypeName> {
            ref: <DetailedTypeName>  // Links to detailed type
            operations { ... }
        }
    }
}
```

## Basic Example

```graphql
// Define types first
type UserItemData {
    id: ID!
    typeId: ID!
    name: String
}

type UserData {
    id: ID
    name: String
    email: String
}

// Use them in collectionSchema
collectionSchema {
    type User {
        ref: UserItemData        // For list operations
        operations {
            list: UsersList
        }
        subtype User {
            ref: UserData        // For get/update/insert
            operations {
                get: UserItem
                update: UserUpdate
                insert: UserAdd
            }
        }
    }
}
```

## Key Points

1. **Define Before Use**: Types must be defined before being referenced in `ref`
2. **Required Fields**: Referenced types should have `id: ID!` and `typeId: ID!`
3. **Type Matching**: The ref type must match the return type of operations
4. **Can Reference**: Both `type` and `union` can be referenced

## Common Pattern

```graphql
// List data (lightweight)
type <Entity>ItemData {
    id: ID!
    typeId: ID!
    // minimal fields for display
}

// Detail data (complete)
type <Entity>Data {
    id: ID
    // all fields for editing
}

collectionSchema {
    type <Entity> {
        ref: <Entity>ItemData    // For lists
        operations {
            list: <Entity>List
        }
        subtype <Entity> {
            ref: <Entity>Data    // For details
            operations {
                get: <Entity>Item
                update: <Entity>Update
                insert: <Entity>Add
            }
        }
    }
}
```

## Operation-Type Mapping

| Operation | Uses Ref From | Purpose |
|-----------|---------------|---------|
| `list` | Main type | Returns array of ref type |
| `get` | Subtype | Returns single ref object |
| `update` | Subtype | Accepts ref object |
| `insert` | Subtype | Accepts ref object |

## Common Errors

**Error**: "Unable to find type 'TypeName'"
- **Fix**: Define the type before collectionSchema

**Error**: "Reference for 'CollectionType' at line X"
- **Fix**: Ensure `ref:` has a value: `ref: TypeName`

**Error**: Type mismatch in operations
- **Fix**: Ensure operation return types match ref types

## Code Location

- Parser: `Include/imtsdl/CGqlExtSchemaParser.cpp:136-178`
- Document Type: `Include/imtsdl/CSdlDocumentType.h:72-74`
- Examples: `Sdl/imtauth/1.0/`, `Sdl/imtlic/1.0/`

## See Also

- [Complete SDL Reference Guide](./SDL_Reference_Attribute_Guide.md) (Russian, 21KB)
- [Practical Examples](./SDL_Ref_Examples.md) (5 detailed examples)
- [Visual Diagrams](./SDL_Ref_Diagrams.md) (8 diagrams explaining ref workflow)
- [SDL Examples](../Sdl/)
