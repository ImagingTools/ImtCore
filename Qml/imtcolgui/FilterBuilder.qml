import QtQuick 2.12


/*!
    \qmltype FilterBuilder
    \inqmlmodule imtcolgui
    \brief Pure QML fluent builder for standalone filter-request query strings.

    This component keeps only local builder state and emits a query string that
    can be parsed by imtbase::CFilterSerializer. It does not wrap or mimic the
    existing collection-filter QML types.

    Example:
    \code
    myFilter.search("john", ["Name", "Email"])
            .where("Status", "is", "Active")
            .where("Name", "like", searchText)
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

    signal filterReady(string queryString)
    signal filterChanged()
    signal cleared()

    property var _rules: []
    property var _orders: []

    function search(text, scopes) {
        searchText = text || ""
        searchScopes = scopes || []
        return root
    }

    function where(path, predicate, argument) {
        _rules = _rules.concat([{path: path, predicate: predicate || "is", argument: argument}])
        return root
    }

    function orderBy(path, descending) {
        _orders = _orders.concat([{path: path, descending: descending === true}])
        return root
    }

    function page(pageNumber, pageSize) {
        var maxSafeInteger = 9007199254740991
        if (pageNumber > 0 && pageSize > 0 && (pageNumber - 1) <= Math.floor(maxSafeInteger / pageSize)) {
            offset = (pageNumber - 1) * pageSize
            count = pageSize
        } else {
            offset = -1
            count = -1
        }
        return root
    }

    function window(first, size) {
        offset = first >= 0 ? first : -1
        count = size > 0 ? size : -1
        return root
    }

    function apply() {
        queryString = _buildQueryString()
        filterReady(queryString)
        filterChanged()
    }

    function reset() {
        searchText = ""
        searchScopes = []
        offset = -1
        count = -1
        queryString = ""
        _rules = []
        _orders = []
        cleared()
        return root
    }

    function _buildQueryString() {
        var parts = []

        if (searchText !== "") {
            parts.push("search=" + encodeURIComponent(searchText))
        }

        if (searchScopes.length > 0) {
            parts.push("scope=" + encodeURIComponent(searchScopes.join(",")))
        }

        for (var i = 0; i < _rules.length; ++i) {
            var r = _rules[i]
            var prefix = "rule[" + i + "]."
            parts.push(prefix + "path=" + encodeURIComponent(r.path))
            parts.push(prefix + "pred=" + encodeURIComponent(r.predicate))
            parts.push(prefix + "arg=" + encodeURIComponent(r.argument))
        }

        for (var j = 0; j < _orders.length; ++j) {
            var o = _orders[j]
            var orderPrefix = "order[" + j + "]."
            parts.push(orderPrefix + "path=" + encodeURIComponent(o.path))
            parts.push(orderPrefix + "dir=" + (o.descending ? "down" : "up"))
        }

        if (offset >= 0 && count > 0) {
            parts.push("offset=" + offset)
            parts.push("count=" + count)
        }

        return parts.join("&")
    }
}
