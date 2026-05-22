pragma Singleton

import QtQuick 2.12

QtObject {
    id: builder

    /*
        Creates a normalized tree node.

        Node format:
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

    /*
        Converts a JS array of node-like objects to normalized format.
    */
    function fromArray(items) {
        var result = []
        if (!items)
            return result

        for (var i = 0; i < itemCount(items); ++i)
            result.push(normalizeNode(itemAt(items, i), String(i)))

        return result
    }

    /*
        Converts a JS array or QML ListModel to normalized tree nodes.

        mapItem(item, index) -> node-like object
        getChildren(item, index) -> array/ListModel/null
    */
    function fromListModel(listModel, mapItem, getChildren) {
        var result = []
        if (!listModel)
            return result

        for (var i = 0; i < itemCount(listModel); ++i) {
            var item = itemAt(listModel, i)
            result.push(convertListItem(item, i, String(i), mapItem, getChildren))
        }

        return result
    }

    /*
        Simplified field-based conversion for list models / JS arrays.

        fields = {
            key: "id",           // property name for node key
            text: "name",        // property name for display text
            children: "children" // property name for child items (array/ListModel)
        }

        Example:
            TreeModelBuilder.fromListModelByFields(myArray, {
                key: "id",
                text: "name",
                children: "subItems"
            })

        Additional optional fields:
            checkable: "isCheckable"
            checked: "isChecked"
            enabled: "isEnabled"
            expanded: "isExpanded"
    */
    function fromListModelByFields(listModel, fields) {
        if (!listModel || !fields)
            return []

        var keyProp = fields.key || ""
        var textProp = fields.text || ""
        var childrenProp = fields.children || ""

        var mapItem = function(item) {
            var obj = item && item.item ? item.item : item
            var result = {}
            if (keyProp)
                result.key = obj ? (obj[keyProp] || "") : ""
            if (textProp)
                result.text = obj ? (obj[textProp] || "") : ""
            if (fields.checkable)
                result.checkable = obj ? (obj[fields.checkable] || false) : false
            if (fields.checked)
                result.checked = obj ? (obj[fields.checked] || Qt.Unchecked) : Qt.Unchecked
            if (fields.enabled)
                result.enabled = obj ? (obj[fields.enabled] !== undefined ? obj[fields.enabled] : true) : true
            if (fields.expanded)
                result.expanded = obj ? (obj[fields.expanded] || false) : false
            return result
        }

        var getChildren = childrenProp
            ? function(item) {
                var obj = item && item.item ? item.item : item
                var ch = obj ? obj[childrenProp] : null
                return (ch && (itemCount(ch) > 0)) ? ch : null
            }
            : null

        return fromListModel(listModel, mapItem, getChildren)
    }

    /*
        Converts tree item models (GetItemsCount/GetData/GetTreeItemModel API).

        mapItem(wrapper, index) -> node-like object
        getChildren(wrapper, index) -> child tree item model or null

        Wrapper properties: model, row, data(role, fallback), childModel(role)
    */
    function fromTreeItemModel(treeItemModel, mapItem, getChildren) {
        var result = []
        if (!treeItemModel)
            return result

        for (var i = 0; i < treeItemCount(treeItemModel); ++i) {
            var wrapper = createTreeItemWrapper(treeItemModel, i)
            result.push(convertTreeItem(wrapper, i, String(i), mapItem, getChildren))
        }

        return result
    }

    /*
        Simplified field-based conversion for tree item models.

        Instead of callbacks, specify field names (roles) to map data:

        fields = {
            key: "Id",           // role name for node key
            text: "Name",        // role name for display text
            children: "Children" // role name for child tree item model
        }

        Example:
            TreeModelBuilder.fromTreeItemModelByFields(myModel, {
                key: "FeatureId",
                text: "DisplayName",
                children: "SubFeatures"
            })

        Additional optional fields:
            checkable: "IsCheckable"  // role for checkable flag (default: false)
            checked: "IsChecked"      // role for checked state
            enabled: "IsEnabled"      // role for enabled state (default: true)
            expanded: "IsExpanded"    // role for expanded state (default: false)
    */
    function fromTreeItemModelByFields(treeItemModel, fields) {
        if (!treeItemModel || !fields)
            return []

        var keyField = fields.key || ""
        var textField = fields.text || ""
        var childrenField = fields.children || ""

        var mapItem = function(wrapper) {
            var result = {}
            if (keyField)
                result.key = wrapper.data(keyField, "")
            if (textField)
                result.text = wrapper.data(textField, "")
            if (fields.checkable)
                result.checkable = wrapper.data(fields.checkable, false)
            if (fields.checked)
                result.checked = wrapper.data(fields.checked, Qt.Unchecked)
            if (fields.enabled)
                result.enabled = wrapper.data(fields.enabled, true)
            if (fields.expanded)
                result.expanded = wrapper.data(fields.expanded, false)
            return result
        }

        var getChildren = childrenField
            ? function(wrapper) { return wrapper.childModel(childrenField) }
            : null

        return fromTreeItemModel(treeItemModel, mapItem, getChildren)
    }

    // --- Internal ---

    function convertListItem(item, index, path, mapItem, getChildren) {
        var mapped = mapItem ? mapItem(item, index) : item
        mapped = mapped || {}

        var childrenSource = getChildren ? getChildren(item, index) : (item && item.children !== undefined ? item.children : null)
        var children = []

        if (childrenSource) {
            for (var i = 0; i < itemCount(childrenSource); ++i) {
                var childItem = itemAt(childrenSource, i)
                children.push(convertListItem(childItem, i, path + "/" + i, mapItem, getChildren))
            }
        }

        mapped.children = children
        if (mapped.data === undefined)
            mapped.data = item

        return normalizeNode(mapped, path)
    }

    function convertTreeItem(wrapper, index, path, mapItem, getChildren) {
        var mapped = mapItem ? mapItem(wrapper, index) : {}
        mapped = mapped || {}

        var childrenSource = getChildren ? getChildren(wrapper, index) : null
        var children = []

        if (childrenSource) {
            for (var i = 0; i < treeItemCount(childrenSource); ++i) {
                var childWrapper = createTreeItemWrapper(childrenSource, i)
                children.push(convertTreeItem(childWrapper, i, path + "/" + i, mapItem, getChildren))
            }
        }

        mapped.children = children
        if (mapped.data === undefined)
            mapped.data = wrapper

        return normalizeNode(mapped, path)
    }

    function createTreeItemWrapper(model, row) {
        return {
            model: model,
            row: row,

            data: function(role, fallbackValue) {
                if (!model)
                    return fallbackValue
                var fn = model.GetData || model.getData
                if (!fn)
                    return fallbackValue
                var value = fn.call(model, role, row)
                return value === undefined || value === null ? fallbackValue : value
            },

            childModel: function(role) {
                if (!model)
                    return null
                var fn = model.GetTreeItemModel || model.getTreeItemModel
                if (!fn)
                    return null
                return fn.call(model, role, row)
            }
        }
    }

    function normalizeNode(input, path) {
        input = input || {}

        var fallbackKey = "path/" + path
        var key = input.key !== undefined && input.key !== null && String(input.key).length > 0 ? String(input.key) : fallbackKey
        var text = input.text !== undefined && input.text !== null ? String(input.text) : key
        var rawChildren = input.children || []
        var children = []

        for (var i = 0; i < itemCount(rawChildren); ++i)
            children.push(normalizeNode(itemAt(rawChildren, i), path + "/" + i))

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
        if (model.getItemsCount)
            return model.getItemsCount()
        if (model.count !== undefined)
            return model.count
        if (model.length !== undefined)
            return model.length
        return 0
    }
}
