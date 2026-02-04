# Ordered Object Collection Facade

## Overview

The `IOrderedObjectCollection` interface provides a facade for the `IObjectCollection` interface that supports explicit item ordering. This allows users to set and maintain custom ordering of items in a collection.

## Usage Example

```cpp
#include <imtbase/COrderedObjectCollectionProxy.h>
#include <imtbase/CObjectCollection.h>

// Create a base collection
imtbase::CObjectCollection baseCollection;
baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

// Add some items
QByteArray id1 = baseCollection.InsertNewObject("TypeA", "Item1", "First item");
QByteArray id2 = baseCollection.InsertNewObject("TypeA", "Item2", "Second item");
QByteArray id3 = baseCollection.InsertNewObject("TypeA", "Item3", "Third item");

// Create an ordered proxy to manage custom ordering
imtbase::COrderedObjectCollectionProxy orderedCollection(baseCollection);

// Get current order (initially matches insertion order)
imtbase::ICollectionInfo::Ids currentOrder = orderedCollection.GetOrderedItemIds();
// Result: [id1, id2, id3]

// Move item3 to the beginning
orderedCollection.SetItemOrder(id3, 0);

// Get new order
currentOrder = orderedCollection.GetOrderedItemIds();
// Result: [id3, id1, id2]

// Set complete custom order
imtbase::ICollectionInfo::Ids newOrder;
newOrder << id2 << id3 << id1;
orderedCollection.SetItemsOrder(newOrder);

// Get position of specific item
int position = orderedCollection.GetItemOrder(id2);
// Result: 0 (first position)

// Reset to default order
orderedCollection.ResetItemOrder();

// Get IDs with pagination (respects custom order)
imtbase::ICollectionInfo::Ids subset = orderedCollection.GetElementIds(0, 2);
// Returns first 2 items in custom order
```

## Key Features

- **Custom Ordering**: Set explicit order for collection items
- **Position Management**: Move items to specific positions
- **Automatic Synchronization**: Order is maintained when items are added or removed
- **Delegation Pattern**: All other operations are delegated to the wrapped collection
- **Reset Capability**: Return to default ordering when needed

## API Reference

### IOrderedObjectCollection Methods

- `bool SetItemOrder(const Id& itemId, int position)` - Set position of an item
- `int GetItemOrder(const Id& itemId) const` - Get current position of an item
- `bool SetItemsOrder(const Ids& orderedIds)` - Set complete ordering
- `Ids GetOrderedItemIds() const` - Get all items in current order
- `bool ResetItemOrder()` - Reset to default ordering

### Implementation Notes

- The proxy maintains a `QVector<QByteArray>` to store custom ordering
- When custom order is not set, the default parent collection order is used
- Insert operations add new items to the end of custom order
- Remove operations automatically remove items from custom order
- The facade follows the same patterns as `CFilterCollectionProxy`

## Testing

Unit tests are provided in `Include/imtbase/Test/COrderedObjectCollectionTest.cpp` covering:
- Setting and getting item order
- Reordering items
- Resetting order
- Insertion and removal with custom order
- Pagination with custom order
