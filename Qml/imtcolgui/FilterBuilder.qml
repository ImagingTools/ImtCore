import QtQuick 2.12

QtObject {
    id: root

    // =========================
    // CONFIG / SCHEMA
    // =========================

    /*!
        Field schema example:
        [
            { name: "id", operators: ["=", "in", "not_in"] },
            { name: "age", operators: [">", "<", "between"] }
        ]
    */
    property var fields: []
    property var schema: ({})
    property bool strictValidation: false

    // =========================
    // STATE (SOURCE OF TRUTH)
    // =========================

    property var query: ({
        combinator: "and",
        rules: []
    })

    property var sort: []
    property var window: ({ first: -1, count: -1 })

    property var search: ({
        text: "",
        scopes: []
    })

    property string queryString: ""

    signal changed(var model, string queryString)
    signal cleared()

    // =========================
    // INTERNAL
    // =========================

    property var _stack: []

    onQueryChanged: _resetStack()

    Component.onCompleted: {
        _resetStack()
    }

    function _resetStack() {
        _stack = [query]
    }

    function _current() {
        return _stack[_stack.length - 1]
    }

    // =========================
    // VALIDATION
    // =========================

    function _validateField(field) {
        if (!field) {
            console.warn("FilterBuilder: empty field")
            return false
        }

        if (_fieldSchema(field) !== null) {
            return true
        }

        if ((fields && fields.length > 0) || strictValidation) {
            console.warn("FilterBuilder: unknown field:", field)
            return false
        }

        return true
    }

    function _validateOperator(field, operator) {
        if (!operator)
            return false

        if (_supportedOperators().indexOf(operator) === -1) {
            console.warn("FilterBuilder: unsupported operator:", operator)
            return false
        }

        var fieldInfo = _fieldSchema(field)
        if (fieldInfo && fieldInfo.operators) {
            return fieldInfo.operators.indexOf(operator) !== -1
        }

        return true
    }

    function _normalizeOperator(op) {
        if (!op)
            return "="

        op = String(op).toLowerCase()

        switch (op) {
        case "eq":
        case "=": return "="
        case "like":
        case "contains": return "contains"
        case ">":
        case "gt": return ">"
        case "<":
        case "lt": return "<"
        case "nin":
        case "not_in": return "not_in"
        case "in": return "in"
        case "between": return "between"
        case "not_between": return "not_between"
        default: return op
        }
    }

    function _fieldSchema(field) {
        if (fields && fields.length > 0) {
            for (var i = 0; i < fields.length; ++i) {
                if (fields[i] && fields[i].name === field)
                    return fields[i]
            }
        }

        if (schema && schema[field])
            return schema[field]

        return null
    }

    function _supportedOperators() {
        return ["=", ">", "<", "contains", "in", "not_in", "between", "not_between"]
    }

    function _isArray(value) {
        return value instanceof Array
    }

    function _containsUndefined(values) {
        if (!_isArray(values))
            return values === undefined

        for (var i = 0; i < values.length; ++i) {
            if (values[i] === undefined)
                return true
        }

        return false
    }

    function _cloneValue(value) {
        if (value === null || typeof value !== "object")
            return value

        var i
        if (_isArray(value)) {
            var arrayClone = []
            for (i = 0; i < value.length; ++i)
                arrayClone.push(_cloneValue(value[i]))
            return arrayClone
        }

        var objectClone = {}
        for (i in value)
            objectClone[i] = _cloneValue(value[i])
        return objectClone
    }

    function _normalizeValue(operator, value) {
        if (operator === "in" || operator === "not_in") {
            return _isArray(value) ? _cloneValue(value) : [_cloneValue(value)]
        }

        return _cloneValue(value)
    }

    function _validateValueByOperator(operator, value) {
        if (_containsUndefined(value)) {
            console.warn("FilterBuilder: undefined value")
            return false
        }

        if (operator === "in" || operator === "not_in") {
            if (!_isArray(value) || value.length === 0) {
                console.warn("FilterBuilder:", operator, "requires a non-empty array")
                return false
            }
            return true
        }

        if (operator === "between" || operator === "not_between") {
            if (!_isArray(value) || value.length !== 2) {
                console.warn("FilterBuilder:", operator, "requires an array with exactly two values")
                return false
            }
            return true
        }

        if (_isArray(value)) {
            console.warn("FilterBuilder:", operator, "requires a scalar value")
            return false
        }

        return true
    }

    // =========================
    // BUILDER API
    // =========================

    function where(field, operator, value) {
        if (!_validateField(field))
            return root

        operator = _normalizeOperator(operator)

        if (!_validateOperator(field, operator)) {
            console.warn("FilterBuilder: invalid operator", operator, "for", field)
            return root
        }

        if (_containsUndefined(value)) {
            console.warn("FilterBuilder: undefined value")
            return root
        }

        value = _normalizeValue(operator, value)
        if (!_validateValueByOperator(operator, value))
            return root

        var rule = {
            field: field,
            operator: operator,
            value: value
        }

        _current().rules.push(rule)
        return root
    }

    function beginGroup(combinator) {
        var group = {
            combinator: _normalizeCombinator(combinator),
            rules: []
        }

        _current().rules.push(group)
        _stack.push(group)

        return root
    }

    function beginAnd() { return beginGroup("and") }
    function beginOr() { return beginGroup("or") }

    function endGroup() {
        if (_stack.length > 1) {
            _stack.pop()
        } else {
            console.warn("FilterBuilder: unmatched endGroup()")
        }
        return root
    }

    function orderBy(field, descending) {
        if (!_validateField(field))
            return root

        sort.push({
            field: field,
            direction: descending ? "desc" : "asc"
        })

        return root
    }

    function page(pageNumber, pageSize) {
        if (pageNumber > 0 && pageSize > 0) {
            window.first = (pageNumber - 1) * pageSize
            window.count = pageSize
        }
        return root
    }

    function windowed(first, count) {
        window.first = first >= 0 ? first : -1
        window.count = count > 0 ? count : -1
        return root
    }

    function searchText(text, scopes) {
        search.text = text || ""
        search.scopes = scopes || []
        return root
    }

    // =========================
    // PUBLIC HELPERS
    // =========================

    function getQuery() {
        return JSON.parse(JSON.stringify(query))
    }

    function getModel() {
        return _buildModel()
    }

    function clone() {
        var copy = Qt.createQmlObject('import QtQuick 2.0; QtObject {}', root)

        copy.query = getQuery()
        copy.sort = JSON.parse(JSON.stringify(sort))
        copy.window = JSON.parse(JSON.stringify(window))
        copy.search = JSON.parse(JSON.stringify(search))

        return copy
    }

    // =========================
    // APPLY / RESET
    // =========================

    function apply() {
        if (_stack.length !== 1) {
            console.warn("FilterBuilder: unclosed groups, auto-fixing")
            while (_stack.length > 1)
                _stack.pop()
        }

        var model = _buildModel()
        queryString = _buildQueryString(model)

        changed(model, queryString)
    }

    function reset() {
        query = { combinator: "and", rules: [] }
        sort = []
        window = { first: -1, count: -1 }
        search = { text: "", scopes: [] }
        queryString = ""

        _resetStack()
        cleared()

        return root
    }

    // =========================
    // BUILD
    // =========================

    function _buildModel() {
        var model = {
            query: JSON.parse(JSON.stringify(query))
        }

        if (sort.length > 0)
            model.sort = JSON.parse(JSON.stringify(sort))

        if (window.first >= 0 && window.count > 0)
            model.window = JSON.parse(JSON.stringify(window))

        if (search.text && search.scopes.length > 0)
            model.search = JSON.parse(JSON.stringify(search))

        return model
    }

    function _buildQueryString(model) {
        var parts = []

        if (model.query)
            parts.push("query=" + encodeURIComponent(JSON.stringify(model.query)))

        if (model.sort)
            parts.push("sort=" + encodeURIComponent(JSON.stringify(model.sort)))

        if (model.window) {
            parts.push("offset=" + model.window.first)
            parts.push("count=" + model.window.count)
        }

        if (model.search) {
            parts.push("search=" + encodeURIComponent(model.search.text))
            parts.push("scope=" + encodeURIComponent(model.search.scopes.join(",")))
        }

        return parts.join("&")
    }

    function _normalizeCombinator(c) {
        c = String(c || "and").toLowerCase()
        return c === "or" ? "or" : "and"
    }
}
