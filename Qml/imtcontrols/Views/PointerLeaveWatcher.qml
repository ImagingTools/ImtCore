pragma Singleton

import QtQuick 2.12

// Lets a popup (e.g. a tooltip) learn that the pointer left the browser window or
// the window lost focus. The web runtime (JQML) does not handle that case, so no
// `exited` event reaches the owner of the popup and the popup would stay open.
// No-ops outside a browser (desktop Qt).
QtObject {
    function watch(callback){
        if (typeof document === "undefined" || typeof window === "undefined"){
            return null;
        }

        var handler = function(){
            callback();
        }
        document.documentElement.addEventListener("mouseleave", handler);
        window.addEventListener("blur", handler);

        return handler;
    }

    function unwatch(handler){
        if (!handler || typeof document === "undefined" || typeof window === "undefined"){
            return;
        }

        document.documentElement.removeEventListener("mouseleave", handler);
        window.removeEventListener("blur", handler);
    }
}
