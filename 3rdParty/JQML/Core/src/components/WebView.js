import { Item } from './Item'

export class WebView extends Item {
    constructor(args) {
        super(args)

        this.$cP('url', '').connect(this.$urlChanged.bind(this))
        this.$cP('canGoBack', true)
        this.$cP('canGoForward', true)
        this.$cP('httpUserAgent', '') // not implemented yet
        this.$cP('loadProgress', 0)
        this.$cP('loading', false)
        this.$cP('title', '').connect(this.$titleChanged.bind(this))

        // since Qt v6.5
        // this.$cPC('settings', {
        //     allowFileAccess: false,
        //     javascriptEnabled: false,
        //     localContentCanAccessFileUrls: false,
        //     localStorageEnabled: false,

        // }).connect(this.$settingsChanged.bind(this))

        this.$cS('cookieAdded', 'domain', 'name')
        this.$cS('cookieRemoved', 'domain', 'name')
        this.$cS('loadingChanged', 'loadRequest') // not implemented yet

        this.$window = null
    }

    $domCreate(tag = 'iframe') {
        super.$domCreate(tag)
        this.dom.style.backgroundColor = 'white'
        this.dom.onload = (e) => {
            this.$window = e.target.contentWindow
            this.loading = false
            this.loadProgress = 100
        }
    }
    // since Qt v6.5
    // $settingsChanged(){
    //     let allows = ['allow-same-origin']
    //     if(this.settings.allowFileAccess){
    //         allows.push('allow-downloads')
    //     }
    //     if(this.settings.javascriptEnabled){
    //         allows.push('allow-scripts')
    //     }
    //     if(this.settings.localContentCanAccessFileUrls){
    //         // not implemented yet
    //     }
    //     if(this.settings.localStorageEnabled){
    //         // not implemented yet
    //     }
    //     this.dom.setAttribute('sandbox', allows.join(' '))
    // }
    $titleChanged() {
        this.dom.setAttribute('title', this.title)
    }
    $urlChanged() {
        this.loading = true
        this.loadProgress = 0
        let pathname = window.location.pathname
        let index = pathname.indexOf('html')
        if(index >= 0){
            this.dom.src = (pathname.slice(0, index) + this.url).replace('//', '/')
        }
        
    }
    $widthChanged() {
        this.dom.setAttribute('width', this.width)
        super.$widthChanged()
    }
    $heightChanged() {
        this.dom.setAttribute('height', this.height)
        super.$heightChanged()
    }

    $getCookie(name) {
        let matches = this.$window.document.cookie.match(new RegExp(
            "(?:^|; )" + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + "=([^;]*)"
        ))
        return matches ? decodeURIComponent(matches[1]) : undefined
    }
    deleteAllCookies() {
        let cookies = this.$window.document.cookie.split(";")

        for (let i = 0; i < cookies.length; i++) {
            let cookie = cookies[i]
            let eqPos = cookie.indexOf("=")
            let name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie
            this.deleteCookie('', name)
        }

    }
    deleteCookie(domain, name) {
        this.setCookie(domain, name, "", {
            'max-age': -1
        })

        this.cookieRemoved(domain, name)
    }
    goBack() {
        if(this.canGoBack) this.$window.history.back()
    }
    goForward() {
        if(this.canGoForward) this.$window.history.forward()
    }
    loadHtml(html, baseUrl) {
        // not implemented yet
    }
    reload() {
        this.$window.location.reload(true)
    }
    runJavaScript(script, callback) {
        let el = this.$window.document.createElement('script')
        this.$window.document.head.appendChild(el)
        el.innerText = `(function() {${script}})()`
        el.remove()
        if(callback) callback()
    }
    setCookie(domain, name, value) {
        let added = this.$getCookie(name) ? false : true

        options = {
            path: '/',
        }

        if (options.expires instanceof Date) {
            options.expires = options.expires.toUTCString()
        }

        let updatedCookie = encodeURIComponent(name) + "=" + encodeURIComponent(value)

        for (let optionKey in options) {
            updatedCookie += "; " + optionKey
            let optionValue = options[optionKey]
            if (optionValue !== true) {
                updatedCookie += "=" + optionValue
            }
        }

        this.$window.document.cookie = updatedCookie

        if(added) this.cookieAdded(domain, name)
    }
    stop() {
        this.$window.frames[0].stop()
    }


    $destroy() {
        super.$destroy()
    }
}

QML.WebView = WebView