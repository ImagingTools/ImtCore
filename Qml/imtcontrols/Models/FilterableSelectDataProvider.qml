import QtQuick 2.12

/*!
    \qmltype FilterableSelectDataProvider
    \inqmlmodule imtcontrols
    \brief Abstract cursor-paginated provider for FilterablePopup.

    Subclasses must implement fetch(params) and return a Promise-like object
    resolving to { items: [], nextCursor: string|null }. The params object has
    the shape { filter: string, cursor: string|null, limit: int }.
*/
QtObject {
    id: root

    property int defaultLimit: 50

    function fetch(params){
        console.warn("FilterableSelectDataProvider.fetch() should be implemented in a subclass")
        return Promise.resolve({"items": [], "nextCursor": null})
    }
}
