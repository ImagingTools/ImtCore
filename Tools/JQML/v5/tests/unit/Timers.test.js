// Run: node --test tests/unit
// A timer that expires before the next frame fires before it, as in Qt: the frame is rendered on the window's
// UpdateRequest (QWindow::requestUpdate, the next vsync), and the event loop processes the expired timers first.
// A zero Timer started while a page is built (GroupElementView lays its rows out in one) goes into the first
// frame of the page, before the polish and before the DOM is written. A chain of zero timers runs in the frame
// too, a timer the polish starts fires after the frame; a timer not due yet, or stopped, does not fire.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

function cases(name, runtime){
    const { engine, Item, types } = runtime
    const { Timer } = types

    function timer(interval, repeat = false){
        let t
        engine.batch(() => {
            t = new Timer(null)
            t.interval = interval
            t.repeat = repeat
        })
        return t
    }

    // an item that needs a polish, and records what the polish saw
    function polished(seen){
        let item
        engine.batch(() => { item = new Item(null) })
        item.$polish = () => seen.push('polish')
        engine.queuePolish(item)
        return item
    }

    test(name + ': a zero Timer started before the frame fires in it, before the polish', () => {
        let seen = []
        let t = timer(0)
        engine.connect(t, Timer.SIGNAL_triggered, () => seen.push('triggered'))
        engine.batch(() => t.start())
        polished(seen)
        engine.flushFrame()
        assert.deepEqual(seen, ['triggered', 'polish'])
        assert.equal(t.running, false)
    })

    test(name + ': a zero Timer restarting another one: both fire in the same frame', () => {
        let seen = []
        let first = timer(0), second = timer(0)
        engine.connect(first, Timer.SIGNAL_triggered, () => { seen.push('first'); second.restart() })
        engine.connect(second, Timer.SIGNAL_triggered, () => seen.push('second'))
        engine.batch(() => first.restart())
        polished(seen)
        engine.flushFrame()
        assert.deepEqual(seen, ['first', 'second', 'polish'])
    })

    // GroupElementView: its recalc timer fires, the relayout moves the children, and their changes restart it
    test(name + ': a single-shot zero Timer restarted after it fired fires again in the frame', () => {
        let seen = []
        let t = timer(0)
        engine.connect(t, Timer.SIGNAL_triggered, () => { seen.push('triggered'); if(seen.length < 2) t.restart() })
        engine.batch(() => t.start())
        polished(seen)
        engine.flushFrame()
        assert.deepEqual(seen, ['triggered', 'triggered', 'polish'])
    })

    // QQuickWindow::polishItems runs in the frame, after the event loop pass: a timer the polish starts fires after it
    test(name + ': a zero Timer started by a polish fires after the frame', () => {
        let seen = []
        let t = timer(0)
        engine.connect(t, Timer.SIGNAL_triggered, () => seen.push('triggered'))
        let item
        engine.batch(() => { item = new Item(null) })
        item.$polish = () => { seen.push('polish'); t.restart() }
        try {
            engine.queuePolish(item)
            engine.flushFrame()
            assert.deepEqual(seen, ['polish'])
            assert.equal(t.running, true)
        } finally {
            engine.batch(() => t.stop())
        }
    })

    // the event loop runs a zero timer on every pass until the vsync; the frame gives the timers its budget
    test(name + ': a zero Timer that always restarts itself runs within the frame budget, then the frame polishes', () => {
        let seen = []
        let count = 0
        let t = timer(0)
        engine.connect(t, Timer.SIGNAL_triggered, () => { count++; t.restart() })
        try {
            engine.batch(() => t.start())
            polished(seen)
            let start = Date.now()
            engine.flushFrame()
            assert.ok(count > 1, 'fired ' + count + ' times')
            assert.ok(Date.now() - start < 100)
            assert.deepEqual(seen, ['polish'])
        } finally {
            engine.batch(() => t.stop())
        }
    })

    test(name + ': a repeating zero Timer fires on every pass within the frame budget and keeps running', () => {
        let count = 0
        let t = timer(0, true)
        engine.connect(t, Timer.SIGNAL_triggered, () => count++)
        try {
            engine.batch(() => t.start())
            polished([])
            engine.flushFrame()
            assert.ok(count > 1, 'fired ' + count + ' times')
            assert.equal(t.running, true)
        } finally {
            engine.batch(() => t.stop())
        }
    })

    test(name + ': a Timer not due yet, or stopped before the frame, does not fire in it', () => {
        let count = 0
        let later = timer(1000), stopped = timer(0)
        engine.connect(later, Timer.SIGNAL_triggered, () => count++)
        engine.connect(stopped, Timer.SIGNAL_triggered, () => count++)
        try {
            engine.batch(() => { later.start(); stopped.start() })
            engine.batch(() => stopped.stop())
            polished([])
            engine.flushFrame()
            assert.equal(count, 0)
            assert.equal(later.running, true)
        } finally {
            engine.batch(() => later.stop())
        }
    })
}

for(let [name, load] of runtimes) cases(name, load())
