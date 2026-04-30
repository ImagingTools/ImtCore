import QtQuick 2.12


/*!
    \qmltype FilterBuilder
    \inqmlmodule imtcolgui
    \brief Pure QML fluent builder for standalone filter-request query strings.

    This component keeps only local builder state and emits both a URL query
    string and an SDL-friendly model that can be converted to imtbase::CFilter
    on the server. It does not wrap or mimic existing collection-filter QML types.

    Example:
    \code
    myFilter.search("john", ["Name", "Email"])
            .where("Status", "is", "Active")
            .any(function(or) {
                or.where("Name", "like", searchText)
                  .where("Email", "like", searchText)
            })
            .orderBy("CreatedAt", true)
            .page(1, 50)
            .apply()
    \endcode
*/
QtObject {
    id: root

    property string searchText: ""
    property var searchScopes: []
    property int offset: -1
    property int count: -1
    property string queryString: ""
    property var filterModel: null

    signal filterReady(string queryString)
    signal filterModelReady(var filterModel)
    signal filterChanged()
    signal cleared()

    readonly property double _maxSafeInteger: Number.MAX_SAFE_INTEGER
    property var _rules: _createEmptyRuleSet()
    property var _orders: []

    function search(text, scopes) {
        searchText = text || ""
        searchScopes = scopes || []
        return root
    }

    function join(mode) {
        _rules.join = _normalizeJoin(mode)
        return root
    }

    function where(path, predicate, argument) {
        _rules.items.push({path: path, pred: predicate || "is", arg: argument})
        return root
    }

    function all(configure) {
        _addGroup(_rules, "all", configure)
        return root
    }

    function any(configure) {
        _addGroup(_rules, "any", configure)
        return root
    }

    function orderBy(path, descending) {
        _orders = _orders.concat([{path: path, descending: descending === true}])
        return root
    }

    function page(pageNumber, pageSize) {
        if (pageNumber > 0 && pageSize > 0 && pageNumber <= _maxSafeInteger) {
            var zeroBasedPage = pageNumber - 1
            if (_canMultiplyWithoutOverflow(zeroBasedPage, pageSize)) {
                offset = zeroBasedPage * pageSize
                count = pageSize
                return root
            }
        }

        offset = -1
        count = -1
        return root
    }

    function _canMultiplyWithoutOverflow(left, right) {
        return left <= Math.floor(_maxSafeInteger / right)
    }

    function window(first, size) {
        // window() is offset-based and accepts first=0; page() is 1-based.
        offset = first >= 0 ? first : -1
        count = size > 0 ? size : -1
        return root
    }

    function apply() {
        filterModel = _buildModel()
        queryString = _buildQueryString(filterModel)
        filterModelReady(filterModel)
        filterReady(queryString)
        filterChanged()
    }

    function reset() {
        searchText = ""
        searchScopes = []
        offset = -1
        count = -1
        queryString = ""
        filterModel = null
        _rules = _createEmptyRuleSet()
        _orders = []
        cleared()
        return root
    }

    function _createEmptyRuleSet() {
        return {join: "all", items: [], sets: []}
    }

    function _addGroup(parent, joinMode, configure) {
        var group = {join: _normalizeJoin(joinMode), items: [], sets: []}
        if (configure) {
            configure(_groupBuilder(group))
        }
        parent.sets.push(group)
    }

    function _groupBuilder(group) {
        return {
            where: function(path, predicate, argument) {
                group.items.push({path: path, pred: predicate || "is", arg: argument})
                return this
            },
            all: function(configure) {
                root._addGroup(group, "all", configure)
                return this
            },
            any: function(configure) {
                root._addGroup(group, "any", configure)
                return this
            }
        }
    }

    function _normalizeJoin(mode) {
        var normalized = String(mode || "all").toLowerCase()
        return normalized === "any" || normalized === "or" ? "any" : "all"
    }

    function _hasRules(group) {
        return group && (group.items.length > 0 || group.sets.length > 0)
    }

    function _copyValue(value) {
        if (value instanceof Array) {
            var arrayCopy = []
            for (var i = 0; i < value.length; ++i) {
                arrayCopy.push(_copyValue(value[i]))
            }
            return arrayCopy
        }

        if (value && typeof value === "object") {
            var objectCopy = {}
            for (var key in value) {
                objectCopy[key] = _copyValue(value[key])
            }
            return objectCopy
        }

        return value
    }

    function _buildModel() {
        var model = {}

        if (searchText !== "" && searchScopes.length > 0) {
            model.search = {text: searchText, scopes: searchScopes.slice(0)}
        }

        if (_hasRules(_rules)) {
            model.rules = _copyValue(_rules)
        }

        if (_orders.length > 0) {
            model.orders = _copyValue(_orders)
        }

        if (offset >= 0 && count > 0) {
            model.window = {first: offset, count: count}
        }

        return model
    }

    function _buildQueryString(model) {
        var parts = []

        if (model.search) {
            parts.push("search=" + encodeURIComponent(model.search.text))
            parts.push("scope=" + encodeURIComponent(model.search.scopes.join(",")))
        }

        if (model.rules) {
            parts.push("rules=" + encodeURIComponent(JSON.stringify(model.rules)))
        }

        if (model.orders) {
            for (var j = 0; j < model.orders.length; ++j) {
                var o = model.orders[j]
                var orderPrefix = "order[" + j + "]."
                parts.push(orderPrefix + "path=" + encodeURIComponent(o.path))
                parts.push(orderPrefix + "dir=" + (o.descending ? "down" : "up"))
            }
        }

        if (model.window) {
            parts.push("offset=" + model.window.first)
            parts.push("count=" + model.window.count)
        }

        return parts.join("&")
    }
}
