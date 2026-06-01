pragma Singleton

import QtQuick 2.12

QtObject {
    id: builder

    /*
        Creates a normalized tree node.

        Node format (fixed fields):
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

        Any extra properties in options are preserved on the node
        for multi-column display (accessed via node.data.<columnId>).
    */
    function node(options) {
        options = options || {}

        var result = {
            key: options.key !== undefined && options.key !== null ? String(options.key) : "",
            text: options.text !== undefined && options.text !== null ? String(options.text) : "",
            checked: options.checked !== undefined && options.checked !== null ? options.checked : Qt.Unchecked,
            checkable: options.checkable !== undefined && options.checkable !== null ? options.checkable : false,
            enabled: options.enabled !== undefined && options.enabled !== null ? options.enabled : true,
            expanded: options.expanded !== undefined && options.expanded !== null ? options.expanded : false,
            children: options.children !== undefined && options.children !== null ? options.children : [],
            data: options.data !== undefined ? options.data : null
        }

        var reserved = {key:1, text:1, checked:1, checkable:1, enabled:1, expanded:1, children:1, data:1}
        var keys = Object.keys(options)
        for (var i = 0; i < keys.length; ++i) {
            if (!reserved[keys[i]])
                result[keys[i]] = options[keys[i]]
        }

        return result
    }

    /*
        Converts a JS array of node-like objects to normalized format.
    */
    function fromArray(items) {
        var result = []
        if (!items)
            return result

        for (var i = 0; i < internal.modelCount(items); ++i)
            result.push(internal.normalizeNode(internal.itemAt(items, i), String(i)))

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

        for (var i = 0; i < internal.modelCount(listModel); ++i) {
            var item = internal.itemAt(listModel, i)
            result.push(internal.convertListItem(item, i, String(i), mapItem, getChildren))
        }

        return result
    }

    /*
        Simplified field-based conversion for list models / JS arrays.

        fields = {
            key:      "id",         // property name for node key
            children: "children"    // property name for child items (array/ListModel)
        }

        Additional optional fields:
            text:      "name"
            checkable: "isCheckable"
            checked:   "isChecked"
            enabled:   "isEnabled"
            expanded:  "isExpanded"

        Multi-column support (extra properties placed into node.data):
            columns: {
                description: "descriptionProp",
                priority:    "priorityProp"
            }
        Column names become node.data.<name>; a BasicTreeView column with
        { id: "description" } automatically resolves "data.description".
    */
    function fromListModelByFields(listModel, fields) {
        if (!listModel || !fields)
            return []

        var keyProp = fields.key || ""
        var textProp = fields.text || ""
        var childrenProp = fields.children || ""
        var columns = fields.columns || null

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
            result.data = internal.mapColumns(columns, function(src) {
                return obj ? (obj[src] !== undefined ? obj[src] : "") : ""
            })
            return result
        }

        var getChildren = childrenProp
            ? function(item) {
                var obj = item && item.item ? item.item : item
                var ch = obj ? obj[childrenProp] : null
                return (ch && (internal.modelCount(ch) > 0)) ? ch : null
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

        for (var i = 0; i < internal.modelCount(treeItemModel); ++i) {
            var wrapper = internal.createTreeItemWrapper(treeItemModel, i)
            result.push(internal.convertTreeItem(wrapper, i, String(i), mapItem, getChildren))
        }

        return result
    }

    /*
        Simplified field-based conversion for tree item models.

        fields = {
            key:      "Id",         // role name for node key
            children: "Children"    // role name for child tree item model
        }

        Additional optional fields:
            text:      "Name"
            checkable: "IsCheckable"
            checked:   "IsChecked"
            enabled:   "IsEnabled"
            expanded:  "IsExpanded"

        Multi-column support (extra properties placed into node.data):
            columns: {
                description: "DescriptionRole",
                priority:    "PriorityRole"
            }
    */
    function fromTreeItemModelByFields(treeItemModel, fields) {
        if (!treeItemModel || !fields)
            return []

        var keyField = fields.key || ""
        var textField = fields.text || ""
        var childrenField = fields.children || ""
        var columns = fields.columns || null

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
            result.data = internal.mapColumns(columns, function(src) {
                return wrapper.data(src, "")
            })
            return result
        }

        var getChildren = childrenField
            ? function(wrapper) { return wrapper.childModel(childrenField) }
            : null

        return fromTreeItemModel(treeItemModel, mapItem, getChildren)
    }

    /*
        Converts BaseModel instances (getData/getItemsCount API, rows stored as {item: QObject}).

        mapItem(wrapper, index) -> node-like object
        getChildren(wrapper, index) -> child BaseModel or null

        Wrapper properties: model, row, item, data(key, fallback), childModel(key)
    */
    function fromBaseModel(baseModel, mapItem, getChildren) {
        var result = []
        if (!baseModel)
            return result

        for (var i = 0; i < internal.modelCount(baseModel); ++i) {
            var wrapper = internal.createBaseModelWrapper(baseModel, i)
            result.push(internal.convertBaseModelItem(wrapper, i, String(i), mapItem, getChildren))
        }

        return result
    }

    /*
        Simplified field-based conversion for BaseModel.

        fields = {
            key:      "m_id",
            children: "m_children"
        }

        Additional optional fields:
            text:      "m_name"
            checkable: "m_isCheckable"
            checked:   "m_isChecked"
            enabled:   "m_isEnabled"
            expanded:  "m_isExpanded"

        Multi-column support (extra properties placed into node.data):
            columns: {
                description: "m_description",
                priority:    "m_priority"
            }
    */
    function fromBaseModelByFields(baseModel, fields) {
        if (!baseModel || !fields)
            return []

        var keyField = fields.key || ""
        var textField = fields.text || ""
        var childrenField = fields.children || ""
        var columns = fields.columns || null

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
            var data = internal.mapColumns(columns, function(src) {
                return wrapper.data(src, "")
            }) || {}
            data.sourceItem = wrapper.item
            result.data = data
            return result
        }

        var getChildren = childrenField
            ? function(wrapper) { return wrapper.childModel(childrenField) }
            : null

        return fromBaseModel(baseModel, mapItem, getChildren)
    }

    // --- Internal ---

    property QtObject internal: QtObject{
        function mapColumns(columns, getValue) {
            if (!columns)
                return undefined
            var data = {}
            var keys = Object.keys(columns)
            for (var i = 0; i < keys.length; ++i)
                data[keys[i]] = getValue(columns[keys[i]])
            return data
        }

        function convertListItem(item, index, path, mapItem, getChildren) {
            var mapped = mapItem ? mapItem(item, index) : item
            mapped = mapped || {}

            var childrenSource = getChildren ? getChildren(item, index) : (item && item.children !== undefined ? item.children : null)
            var children = []

            if (childrenSource) {
                for (var i = 0; i < modelCount(childrenSource); ++i) {
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
                for (var i = 0; i < modelCount(childrenSource); ++i) {
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
            var text = input.text !== undefined && input.text !== null ? String(input.text) : ""
            var rawChildren = input.children || []
            var children = []

            for (var i = 0; i < modelCount(rawChildren); ++i)
                children.push(normalizeNode(itemAt(rawChildren, i), path + "/" + i))

            var opts = {
                key: key,
                text: text,
                checked: input.checked,
                checkable: input.checkable,
                enabled: input.enabled,
                expanded: input.expanded,
                children: children,
                data: input.data
            }

            // Preserve extra column properties
            var reserved = {key:1, text:1, checked:1, checkable:1, enabled:1, expanded:1, children:1, data:1}
            var keys = Object.keys(input)
            for (var k = 0; k < keys.length; ++k) {
                if (!reserved[keys[k]])
                    opts[keys[k]] = input[keys[k]]
            }

            return builder.node(opts)
        }

        function itemAt(items, row) {
            if (!items)
                return null
            if (items.get)
                return items.get(row)
            return items[row]
        }

        function modelCount(model) {
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

        function createBaseModelWrapper(model, row) {
            var entry = model.get ? model.get(row) : null
            var obj = entry ? entry.item : null

            return {
                model: model,
                row: row,
                item: obj,

                data: function(key, fallbackValue) {
                    if (!model)
                        return fallbackValue
                    var fn = model.getData
                    if (fn) {
                        var value = fn.call(model, key, row)
                        return value === undefined || value === null ? fallbackValue : value
                    }
                    if (obj) {
                        var v = obj[key]
                        return v === undefined || v === null ? fallbackValue : v
                    }
                    return fallbackValue
                },

                childModel: function(key) {
                    if (!model)
                        return null
                    var fn = model.getData
                    if (fn) {
                        var child = fn.call(model, key, row)
                        return child || null
                    }
                    if (obj) {
                        var c = obj[key]
                        return c || null
                    }
                    return null
                }
            }
        }

        function convertBaseModelItem(wrapper, index, path, mapItem, getChildren) {
            var mapped = mapItem ? mapItem(wrapper, index) : {}
            mapped = mapped || {}

            var childrenSource = getChildren ? getChildren(wrapper, index) : null
            var children = []

            if (childrenSource) {
                for (var i = 0; i < modelCount(childrenSource); ++i) {
                    var childWrapper = createBaseModelWrapper(childrenSource, i)
                    children.push(convertBaseModelItem(childWrapper, i, path + "/" + i, mapItem, getChildren))
                }
            }

            mapped.children = children
            if (mapped.data === undefined)
                mapped.data = wrapper

            return normalizeNode(mapped, path)
        }
    }
}
