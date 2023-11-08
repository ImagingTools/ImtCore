const { Item } = require('./Item')
const { QReal, QBool, QString } = require('../utils/properties')

class WebView extends Item {
    static defaultProperties = {
        url: { type: QString, value: '' },
        title: { type: QString, value: '' },
        httpUserAgent: { type: QString, value: '' },
        canGoBack: { type: QBool, value: true },
        canGoForward: { type: QBool, value: true },
        loadProgress: { type: QReal, value: 0 },
        loading: { type: QBool, value: false },

    }

    static defaultSignals = {
        cookieAdded: { params: ['domain', 'name'] },
        cookieRemoved: { params: ['domain', 'name'] },
        loadingChanged: { params: ['loadRequest'] },

    }

    // not implemented yet
 
}

module.exports.WebView = WebView