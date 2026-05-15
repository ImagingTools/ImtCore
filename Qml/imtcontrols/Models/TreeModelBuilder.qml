pragma Singleton

import QtQuick 2.12

QtObject {
    id: builder

    /*
        Fixed node format expected by TreeView:

        {
            key: "unique-key",
            text: "Display text",
            checked: Qt.Unchecked,
            checkable: false,
            enabled: true,
            expanded: false,
            children: [],
            data: null
        }
    */

    function node(options) {
        options = options || {}

        return {
            key: options.key !== undefined && options.key !== null ? String(options.key) : "",
            text: options.text !== undefined && options.text !== null ? String(options.text) : "",
            checked: options.checked !== undefined && options.checked !== null ? options.checked : Qt.Unchecked,
            checkable: options.checkable !== undefined && options.checkable !== null ? options.checkable : false,
            enabled: options.enabled !== undefined && options.enabled !== null ? options.enabled : true,
            expanded: options.expanded !== undefined && options.expanded !== null ? options.expanded : false,
            children: options.children !== undefined && options.children !== null ? options.children : [],
            data: options.data !== undefined ? options.data : null
        }
    }

    function fromArray(items) {
        var result = []

        if (!items)
            return result

        for (var i = 0; i < itemCount(items); ++i) {
            result.push(normalizeNode(itemAt(items, i), createContext(null, i, String(i), 0)))
        }

        return result
    }

    /*
        Converts a JS array or QML ListModel to TreeView fixed node format.

        mapItem signature:
            function(item, context) -> object

        getChildren signature:
            function(item, context) -> array/ListModel/null
    */
    function fromListModel(listModel, mapItem, getChildren) {
        var result = []

        if (!listModel)
            return result

        for (var i = 0; i < itemCount(listModel); ++i) {
            var item = itemAt(listModel, i)
            var context = createContext(null, i, String(i), 0)

            result.push(normalizeMappedListItem(item, context, mapItem, getChildren))
        }

        return result
    }

    function normalizeMappedListItem(item, context, mapItem, getChildren) {
        var mapped = mapItem ? mapItem(item, context) : item
        mapped = mapped || {}

        var childrenSource = null

        if (getChildren)
            childrenSource = getChildren(item, context)
        else if (item && item.children !== undefined)
            childrenSource = item.children

        var children = []

        if (childrenSource) {
            for (var i = 0; i < itemCount(childrenSource); ++i) {
                var childItem = itemAt(childrenSource, i)
                var childContext = createContext(mapped, i, context.path + "/" + i, context.level + 1)
                children.push(normalizeMappedListItem(childItem, childContext, mapItem, getChildren))
            }
        }

        mapped.children = children

        if (mapped.data === undefined)
            mapped.data = item

        return normalizeNode(mapped, context)
    }

    /*
        Converts tree item models with API like:
            model.GetItemsCount()
            model.GetData(role, row)
            model.GetTreeItemModel(role, row)

        User controls conversion through mapItem/getChildren rather than role options.

        Wrapper passed to mapItem:
            item.model
            item.row
            item.path
            item.level
            item.data(role, fallbackValue)
            item.setData(role, value)
            item.childModel(role)
    */
    function fromTreeItemModel(treeItemModel, mapItem, getChildren) {
        var result = []

        if (!treeItemModel)
            return result

        for (var i = 0; i < treeItemCount(treeItemModel); ++i) {
            var context = createContext(null, i, String(i), 0)
            var item = createTreeItemWrapper(treeItemModel, i, context)

            result.push(normalizeMappedTreeItem(item, context, mapItem, getChildren))
        }

        return result
    }

    function normalizeMappedTreeItem(item, context, mapItem, getChildren) {
        var mapped = mapItem ? mapItem(item, context) : {}
        mapped = mapped || {}

        var childrenSource = getChildren ? getChildren(item, context) : null
        var children = []

        if (childrenSource) {
            for (var i = 0; i < treeItemCount(childrenSource); ++i) {
                var childContext = createContext(mapped, i, context.path + "/" + i, context.level + 1)
                var childItem = createTreeItemWrapper(childrenSource, i, childContext)

                children.push(normalizeMappedTreeItem(childItem, childContext, mapItem, getChildren))
            }
        }

        mapped.children = children

        if (mapped.data === undefined)
            mapped.data = item

        return normalizeNode(mapped, context)
    }

    function createTreeItemWrapper(model, row, context) {
        return {
            model: model,
            row: row,
            path: context.path,
            level: context.level,

            data: function(role, fallbackValue) {
                if (!model || !model.GetData)
                    return fallbackValue

                var value = model.GetData(role, row)
                return value === undefined || value === null ? fallbackValue : value
            },

            setData: function(role, value) {
                if (!model)
                    return

                if (model.SetData) {
                    model.SetData(role, row, value)
                    return
                }

                if (model.setData) {
                    model.setData(role, row, value)
                    return
                }
            },

            childModel: function(role) {
                if (!model || !model.GetTreeItemModel)
                    return null

                return model.GetTreeItemModel(role, row)
            }
        }
    }

    function normalizeNode(input, context) {
        input = input || {}
        context = context || createContext(null, 0, "0", 0)

        var fallbackKey = "path/" + context.path
        var key = input.key !== undefined && input.key !== null && String(input.key).length > 0 ? String(input.key) : fallbackKey
        var text = input.text !== undefined && input.text !== null ? String(input.text) : key
        var rawChildren = input.children || []
        var children = []

        for (var i = 0; i < itemCount(rawChildren); ++i) {
            children.push(normalizeNode(itemAt(rawChildren, i), createContext(input, i, context.path + "/" + i, context.level + 1)))
        }

        return node({
            key: key,
            text: text,
            checked: input.checked,
            checkable: input.checkable,
            enabled: input.enabled,
            expanded: input.expanded,
            children: children,
            data: input.data
        })
    }

    function createContext(parent, row, path, level) {
        return {
            parent: parent,
            row: row,
            path: path,
            level: level
        }
    }

    function itemCount(items) {
        if (!items)
            return 0

        if (items.count !== undefined)
            return items.count

        if (items.length !== undefined)
            return items.length

        return 0
    }

    function itemAt(items, row) {
        if (!items)
            return null

        if (items.get)
            return items.get(row)

        return items[row]
    }

    function treeItemCount(model) {
        if (!model)
            return 0

        if (model.GetItemsCount)
            return model.GetItemsCount()

        if (model.count !== undefined)
            return model.count

        if (model.length !== undefined)
            return model.length

        return 0
    }
}
