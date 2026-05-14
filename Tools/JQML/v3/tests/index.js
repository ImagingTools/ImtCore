const { spawn } = require('child_process')
const fs = require('fs')
const { Builder, Capabilities, By, until } = require('selenium-webdriver')
const chrome = require('selenium-webdriver/chrome')
const path = require('path')
require('chromedriver')

const colors = {
    reset: "\x1b[0m",
    red: "\x1b[31m",
    green: "\x1b[32m",
    yellow: "\x1b[33m",
    blue: "\x1b[34m",
    magenta: "\x1b[35m",
    cyan: "\x1b[36m",
    white: "\x1b[37m",
    gray: "\x1b[90m"
}

let compiler = null

function getCompiler() {
    if (compiler) return compiler

    const originalArgv = process.argv

    try {
        process.argv = [originalArgv[0], originalArgv[1]]
        compiler = require('../compiler/compiler')
        return compiler
    } finally {
        process.argv = originalArgv
    }
}

function getQmlLog(fullLogs) {
    const regex = /^qml:\s+(?!warning:)(.+)$/gm
    let matches = []
    let match

    while ((match = regex.exec(fullLogs)) !== null) {
        matches.push(match[1])
    }

    return matches
}

function getErrorMessage(error) {
    if (!error) return 'Unknown error'
    if (error.message) return error.message
    return String(error)
}

function runQmlTest(filePath, timeout = 5000) {
    return new Promise((resolve, reject) => {
        // Запуск в headless режиме
        const child = spawn('qml', ['-platform', 'offscreen', filePath], {
            cwd: path.dirname(filePath),
            env: { ...process.env, QT_LOGGING_TO_CONSOLE: "1" }
        })

        let logs = []

        child.stdout.on('data', (data) => {
            let result = getQmlLog(data.toString().trim())
            logs.push(...result)
        })
        child.stderr.on('data', (data) => {
            let result = getQmlLog(data.toString().trim())
            logs.push(...result)
        })

        // Тайм-аут на случай зависания теста
        const timer = setTimeout(() => {
            child.kill()
            reject(new Error(`Test timed out: ${filePath}`))
        }, timeout)

        child.on('close', (code) => {
            clearTimeout(timer)
            resolve(logs)
        })
    })
}

function checkIsDirectorySync(path) {
    try {
        return fs.statSync(path).isDirectory()
    } catch (e) {
        return false
    }
}

function parseCliOptions() {
    const args = process.argv.slice(2)
    let testName = ''

    for (let i = 0; i < args.length; i++) {
        const arg = args[i]

        if (arg.startsWith('--test=')) {
            testName = arg.slice('--test='.length).trim()
            continue
        }

        if (arg === '--test' || arg === '-t') {
            testName = (args[i + 1] || '').trim()
            i++
            continue
        }

        if (!arg.startsWith('-') && !testName) {
            testName = arg.trim()
        }
    }

    return { testName }
}

function getTestsToRun(allEntries, selectedTestName) {
    const allTests = allEntries.filter(testdir => {
        const testDirPath = path.resolve(__dirname, `./${testdir}`)
        const mainFilePath = path.resolve(__dirname, `./${testdir}/Main.qml`)

        return checkIsDirectorySync(testDirPath) && fs.existsSync(mainFilePath)
    })

    if (!selectedTestName) return allTests

    return allTests.filter(testName => testName === selectedTestName)
}

async function runTests() {
    const tests = fs.readdirSync('./tests')
    const options = parseCliOptions()
    const testsToRun = getTestsToRun(tests, options.testName)

    if (options.testName && testsToRun.length === 0) {
        console.error(`${colors.red}[Error] Test not found: ${options.testName}${colors.reset}`)
        process.exitCode = 1
        return
    }

    let completedTests = 0
    let testCount = 0

    const driver = await createWebDriver()

    for (let testdir of testsToRun) {
        let mainFilePath = path.resolve(__dirname, `./${testdir}/Main.qml`)
        let testDirPath = path.resolve(__dirname, `./${testdir}`)

        if (checkIsDirectorySync(testDirPath)) {
            console.log(`${colors.yellow}[i] Started test: ${testdir}${colors.reset}`)
            testCount++
            let resultDesktop
            let resultWeb

            try {
                resultDesktop = await runQmlTest(mainFilePath)
            } catch (err) {
                console.error(`${colors.red}[Error] Error running test on desktop: ${getErrorMessage(err)}${colors.reset}`)
            }

            try {
                resultWeb = await runWebTest(driver, testDirPath)
            } catch (err) {
                console.error(`${colors.red}[Error] Error running web test: ${getErrorMessage(err)}${colors.reset}`)
            }

            console.log(`${colors.cyan}[Desktop] ${resultDesktop}${colors.reset}`)
            console.log(`${colors.cyan}[Web] ${resultWeb}${colors.reset}`)

            if (resultDesktop.join('') === resultWeb.join('')) {
                completedTests++
                console.log(`${colors.green}[+] Completed test: ${testdir}${colors.reset}`)
            } else {
                console.log(`${colors.red}[-] Failed test: ${testdir}${colors.reset}`)
            }
        }
    }

    await closeWebDriver(driver)

    console.log(`${colors.yellow}[i] Completed tests: ${completedTests}/${testCount}${colors.reset}`)
}

async function runWebTest(driver, testDirPath, timeout = 5000) {
    try {
        fs.mkdirSync(path.resolve(testDirPath, '_web'), { recursive: true })
    } catch (err) {
        console.error(`${colors.red}[Error] Error creating _web folder: ${err.message}${colors.reset}`)
    }

    try {
        const compiler = getCompiler()

        compiler.compile({
            config: path.resolve(testDirPath, 'test.json'),
            output: path.resolve(testDirPath, '_web'),
            entry: path.resolve(testDirPath, 'Main.qml'),
            name: 'test',
            root: '/',
            mode: 'js',
        })

        compiler.compile({
            config: path.resolve(testDirPath, 'test.json'),
            output: path.resolve(testDirPath, '_web'),
            entry: path.resolve(testDirPath, 'Main.qml'),
            name: 'test',
            root: '/',
            mode: 'html',
        })
    } catch (err) {
        console.error(`${colors.red}[Error] Error during compilation: ${getErrorMessage(err)}${colors.reset}`)
    }

    try {
        await driver.get(path.resolve(testDirPath, './_web/test.html'))

        await driver.wait(async () => {
            const readyState = await driver.executeScript('return document.readyState')
            return readyState === 'complete'
        }, timeout) // таймаут 10 секунд

        let containTimer = fs.readFileSync(path.resolve(testDirPath, 'Main.qml'), 'utf-8').indexOf('Timer') >= 0

        if (containTimer) await driver.sleep(timeout)

        const logs = await driver.manage().logs().get('browser')

        let messages = []

        logs.forEach(log => {
            if (log.level.name === 'INFO') {
                messages.push(`${log.message.split(' ').slice(2).join(' ').replaceAll('"', '')}`) // Удаляем первые 2 слова
            }
        })

        return messages
    } finally {
        try {
            fs.rmSync(path.resolve(testDirPath, '_web'), { recursive: true, force: true });
        } catch (err) {
            console.error(`${colors.red}[Error] Error deleting _web folder: ${err.message}${colors.reset}`);
        }
    }


}

async function createWebDriver() {
    try {
        // Настройка Chrome (опционально: запуск без окна)
        let options = new chrome.Options()
        options.addArguments('--headless', '--window-size=800,600') // Раскомментировать для headless-режима

        const caps = Capabilities.chrome()
        caps.setLoggingPrefs({
            browser: 'ALL', // Собирать все типы сообщений (INFO, WARNING, SEVERE)
            driver: 'WARNING'
        })

        let driver = await new Builder()
            .forBrowser('chrome')
            .setChromeOptions(options)
            .withCapabilities(caps)
            .build()
        
        return driver
    } catch (err) {
        console.error(`${colors.red}[Error] Error creating WebDriver: ${err.message}${colors.reset}`)
    }
}


async function closeWebDriver(driver) {
    await driver.quit() // Закрыть браузер
}


runTests()