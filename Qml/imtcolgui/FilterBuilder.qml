import QtQuick 2.12


/*!
    \qmltype FilterBuilder
    \inqmlmodule imtcolgui
    \brief Pure QML fluent builder for standalone collection-filter query strings.

    This component does not wrap existing collection-filter QML types. It only keeps
    builder state and emits a URL query string that can be parsed by
    imtbase::CFilterSerializer.

    Example:
    \code
    FilterBuilder {
        id: myFilter
        onFilterReady: collectionView.applyFilter(queryString)
    }

    myFilter.where("Status", "eq", "Active")
            .where("Name", "contains", searchText)
            .orderBy("CreatedAt", "desc")
            .page(1, 50)
            .apply()
    \endcode
*/
QtObject {
    id: root

    property string textFilter: ""
    property var textFilteringInfoIds: []
    // Pagination sentinel: -1 means both currentPage and pageSize are intentionally
    // unset and should not be serialized.
    property int currentPage: -1
    property int pageSize: -1
    property string queryString: ""

    signal filterReady(string queryString)
    signal filterChanged()
    signal cleared()

    property var _filters: []
    property var _sortFields: []

    function text(value, fields) {
        textFilter = value || ""
        textFilteringInfoIds = fields || textFilteringInfoIds
        return root
    }

    function where(field, op, value) {
        _filters = _filters.concat([{field: field, op: op || "eq", value: value}])
        return root
    }

    function orderBy(field, direction) {
        _sortFields = _sortFields.concat([{field: field, direction: direction || "asc"}])
        return root
    }

    function page(pageNum, size) {
        currentPage = pageNum > 0 ? pageNum : -1
        pageSize = size > 0 ? size : -1
        return root
    }

    function apply() {
        queryString = _buildQueryString()
        filterReady(queryString)
        filterChanged()
    }

    function reset() {
        textFilter = ""
        textFilteringInfoIds = []
        currentPage = -1
        pageSize = -1
        queryString = ""
        _filters = []
        _sortFields = []
        cleared()
        return root
    }

    function _buildQueryString() {
        var parts = []

        if (textFilter !== "") {
            parts.push("text=" + encodeURIComponent(textFilter))
        }

        if (textFilteringInfoIds.length > 0) {
            parts.push("textFields=" + encodeURIComponent(textFilteringInfoIds.join(",")))
        }

        for (var i = 0; i < _filters.length; ++i) {
            var f = _filters[i]
            parts.push("filter[" + encodeURIComponent(f.field) + "][" + encodeURIComponent(f.op) + "]=" + encodeURIComponent(f.value))
        }

        if (_sortFields.length > 0) {
            var sortParts = []
            for (var j = 0; j < _sortFields.length; ++j) {
                var s = _sortFields[j]
                sortParts.push(encodeURIComponent(s.field) + ":" + encodeURIComponent(s.direction))
            }
            parts.push("sort=" + sortParts.join(","))
        }

        if (currentPage > 0) {
            parts.push("page=" + currentPage)
        }
        if (pageSize > 0) {
            parts.push("limit=" + pageSize)
        }

        return parts.join("&")
    }
}
