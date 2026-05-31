const Item = require("../QtQuick/Item")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const StringProp = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

/**
 * WebView — renders a web page inside an <iframe>.
 */
class WebView extends Item {
    static meta = Object.assign({}, Item.meta, {
        url:           { type: StringProp, value: '' },
        title:         { type: StringProp, value: '' },
        httpUserAgent: { type: StringProp, value: '' },
        canGoBack:     { type: Bool, value: false },
        canGoForward:  { type: Bool, value: false },
        loadProgress:  { type: Real, value: 0 },
        loading:       { type: Bool, value: false },

        urlChanged:           { type: Signal, args: [] },
        titleChanged:         { type: Signal, args: [] },
        httpUserAgentChanged: { type: Signal, args: [] },
        canGoBackChanged:     { type: Signal, args: [] },
        canGoForwardChanged:  { type: Signal, args: [] },
        loadProgressChanged:  { type: Signal, args: [] },
        loadingChanged:       { type: Signal, args: [] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)

        let iframe = document.createElement('iframe')
        iframe.style.cssText = 'width:100%;height:100%;border:none;display:block;'
        iframe.setAttribute('sandbox', 'allow-scripts allow-same-origin allow-forms allow-popups')
        obj.__DOM.appendChild(iframe)
        obj.__iframe = iframe

        iframe.addEventListener('load', () => {
            obj.loading = false
            obj.loadProgress = 100
            try { obj.title = iframe.contentDocument ? (iframe.contentDocument.title || '') : '' } catch(e) {}
            try {
                let h = iframe.contentWindow ? iframe.contentWindow.history : null
                if (h) { obj.canGoBack = h.length > 1; obj.canGoForward = false }
            } catch(e) {}
        })

        return obj
    }

    SLOT_urlChanged(oldValue, newValue) {
        if (!newValue) return
        this.loading = true
        this.loadProgress = 0
        this.__iframe.src = newValue
    }

    goBack()    { try { this.__iframe.contentWindow.history.back()    } catch(e) {} }
    goForward() { try { this.__iframe.contentWindow.history.forward() } catch(e) {} }
    reload()    { try { this.__iframe.contentWindow.location.reload() } catch(e) {} }
    stop()      { try { this.__iframe.contentWindow.stop()            } catch(e) {} }

    runJavaScript(code, callback) {
        try {
            let result = this.__iframe.contentWindow.eval(code)
            if (typeof callback === 'function') callback(result)
        } catch(e) {
            if (typeof callback === 'function') callback(undefined)
        }
    }
}

module.exports = WebView
