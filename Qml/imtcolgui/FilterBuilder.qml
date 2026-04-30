import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtbaseComplexCollectionFilterSdl 1.0


/*!
    \qmltype FilterBuilder
    \inqmlmodule imtcolgui
    \brief Fluent builder API for constructing collection filters.

    FilterBuilder extends CollectionFilter with a chainable, fluent API that
    mirrors the style used by GitHub's Advanced Filter builder:

    \list
    \li \l {where} — add a field filter condition
    \li \l {orderBy} — set the sort column and direction
    \li \l {page} — set pagination state
    \li \l {apply} — commit the accumulated changes and emit \l filterReady
    \li \l {reset} — clear all filters
    \endlist

    The component is designed so that each builder method returns the
    FilterBuilder instance itself, enabling call chaining in JavaScript:

    \code
    FilterBuilder {
        id: myFilter
        textFilteringInfoIds: ["Name", "Email"]

        onFilterReady: {
            // queryString is ready to send to the backend
            console.log("Query string:", queryString)
        }
    }

    // In a button handler:
    myFilter.where("Status", "eq", "Active")
            .where("Name", "contains", searchText)
            .orderBy("CreatedAt", "desc")
            .page(1, 50)
            .apply()
    \endcode

    \note The component wraps a CollectionFilter and delegates all persistence
          to the underlying C++ ComplexCollectionFilter model.  Setting
          \l textFilter directly is also supported.
*/
CollectionFilter {
    id: root

    // -----------------------------------------------------------------------
    // Public properties
    // -----------------------------------------------------------------------

    /*!
        \qmlproperty string FilterBuilder::textFilter
        The current free-text search string.
    */
    property string textFilter: ""

    /*!
        \qmlproperty list<string> FilterBuilder::textFilteringInfoIds
        Column IDs searched by the text filter.  Each entry becomes a
        FieldInfo with the SO_TEXT_FILTER flag.
    */
    property var textFilteringInfoIds: []

    /*!
        \qmlproperty int FilterBuilder::currentPage
        Current page number (1-based).  Updated by \l page().
    */
    property int currentPage: 1

    /*!
        \qmlproperty int FilterBuilder::pageSize
        Number of items per page.  Updated by \l page().
    */
    property int pageSize: 50

    /*!
        \qmlproperty string FilterBuilder::queryString
        URL-encoded query string produced by the last \l apply() call.
        Example: \c "text=john&filter[Status][eq]=active&sort=Name:asc&page=2&limit=50"
    */
    property string queryString: ""

    // -----------------------------------------------------------------------
    // Signals
    // -----------------------------------------------------------------------

    /*!
        \qmlsignal FilterBuilder::filterReady(string queryString)
        Emitted by \l apply() after all pending changes have been committed.

        \a queryString contains the URL-encoded representation of the filter.
    */
    signal filterReady(string queryString)

    // -----------------------------------------------------------------------
    // Private state
    // -----------------------------------------------------------------------

    /*! \internal Pending field-filter accumulator */
    property var _pendingFilters: []

    /*! \internal Pending sort column */
    property string _sortField: ""

    /*! \internal Pending sort direction ("asc" or "desc") */
    property string _sortDirection: "asc"

    // -----------------------------------------------------------------------
    // Builder methods (each returns 'this' for chaining)
    // -----------------------------------------------------------------------

    /*!
        \qmlmethod FilterBuilder FilterBuilder::where(string field, string op, var value)
        Queues a field-filter condition.

        \a field   Column ID, e.g. \c "Status"
        \a op      Operation name: \c "eq", \c "ne", \c "lt", \c "gt",
                   \c "gte", \c "lte", or \c "contains"
        \a value   Filter value

        Returns the FilterBuilder instance for chaining.
    */
    function where(field, op, value) {
        _pendingFilters = _pendingFilters.concat([{field: field, op: op, value: value}])
        return root
    }

    /*!
        \qmlmethod FilterBuilder FilterBuilder::orderBy(string field, string direction)
        Sets the sort column and direction.

        \a field     Column ID, e.g. \c "CreatedAt"
        \a direction \c "asc" (default) or \c "desc"

        Returns the FilterBuilder instance for chaining.
    */
    function orderBy(field, direction) {
        _sortField = field
        _sortDirection = direction || "asc"
        return root
    }

    /*!
        \qmlmethod FilterBuilder FilterBuilder::page(int pageNum, int size)
        Sets the pagination state.

        \a pageNum  Page number, 1-based (default 1)
        \a size     Page size (default 50)

        Returns the FilterBuilder instance for chaining.
    */
    function page(pageNum, size) {
        currentPage = (pageNum > 0) ? pageNum : 1
        pageSize    = (size    > 0) ? size    : 50
        return root
    }

    /*!
        \qmlmethod void FilterBuilder::apply()
        Commits all queued changes to the underlying CollectionFilter,
        builds the query string, and emits \l filterReady.
    */
    function apply() {
        // Apply text filter
        setTextFilter(textFilter)
        setFilteringInfoIds(textFilteringInfoIds)

        // Apply queued field filters
        clearFieldsFilter()
        for (var i = 0; i < _pendingFilters.length; i++) {
            var f = _pendingFilters[i]
            var valueType = _inferValueType(f.value)
            var opEnum   = _mapOp(f.op)
            var field    = createFieldFilter(f.field, f.value, valueType, opEnum)
            addFieldFilter(field)
        }

        // Apply sort
        if (_sortField !== "") {
            setSortingInfo(_sortField, _sortDirection === "desc" ? "Desc" : "Asc")
        }

        // Build query string
        queryString = _buildQueryString()

        // Notify
        filterReady(queryString)
        filterChanged()
    }

    /*!
        \qmlmethod void FilterBuilder::reset()
        Clears all pending and committed filters and resets pagination.
    */
    function reset() {
        _pendingFilters  = []
        _sortField       = ""
        _sortDirection   = "asc"
        textFilter       = ""
        currentPage      = 1
        pageSize         = 50
        queryString      = ""
        clearAllFilters(false)
    }

    // -----------------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------------

    /*! \internal Infer the valueType string from a JS value */
    function _inferValueType(val) {
        if (typeof val === "boolean") return valueType.BOOL
        if (typeof val === "number")  return Number.isInteger(val) ? valueType.INTEGER : valueType.NUMBER
        return valueType.STRING
    }

    /*! \internal Map an op string to a filterOperation enum value */
    function _mapOp(op) {
        switch (op) {
        case "contains": return filterOperation.CONTAINS
        case "eq":       return filterOperation.EQUAL
        case "lt":       return filterOperation.LESS
        case "gt":       return filterOperation.GREATER
        default:         return filterOperation.EQUAL
        }
    }

    /*! \internal Build URL query string from current state */
    function _buildQueryString() {
        var parts = []

        if (textFilter !== "") {
            parts.push("text=" + encodeURIComponent(textFilter))
        }

        if (textFilteringInfoIds.length > 0) {
            parts.push("textFields=" + encodeURIComponent(textFilteringInfoIds.join(",")))
        }

        for (var i = 0; i < _pendingFilters.length; i++) {
            var f = _pendingFilters[i]
            var key = "filter[" + encodeURIComponent(f.field) + "][" + encodeURIComponent(f.op) + "]"
            parts.push(key + "=" + encodeURIComponent(f.value))
        }

        if (_sortField !== "") {
            parts.push("sort=" + encodeURIComponent(_sortField + ":" + _sortDirection))
        }

        if (currentPage > 1) {
            parts.push("page=" + currentPage)
        }

        if (pageSize !== 50) {
            parts.push("limit=" + pageSize)
        }

        return parts.join("&")
    }
}
