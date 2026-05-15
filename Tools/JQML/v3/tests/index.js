const { spawn } = require('child_process')
const fs = require('fs')
const { Builder, Capabilities, By, until } = require('selenium-webdriver')
const chrome = require('selenium-webdriver/chrome')
const path = require('path')
const chromedriverModule = require('chromedriver')
const chromedriverPath = (() => {
    const p = chromedriverModule.path
    if (fs.existsSync(p)) return p
    const globalBin = process.platform === 'win32' ? 'chromedriver.exe' : 'chromedriver'
    return require('child_process').execSync(`which ${globalBin}`).toString().trim()
})()

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

function envFill(source) {
    let result = String(source)
    for (const key of Object.keys(process.env)) {
        const value = (process.env[key] || '').replaceAll('\\', '\\\\').trim()
        result = result.replaceAll('${' + key + '}', value)
    }
    return result
}

function ensureDefaultTargetName() {
    if (!process.env.IMTCOREDIR_BUILD && process.env.IMTCOREDIR) {
        process.env.IMTCOREDIR_BUILD = process.env.IMTCOREDIR
    }

    if (process.env.TARGETNAME && process.env.TARGETNAME.trim()) return

    const roots = [process.env.IMTCOREDIR_BUILD, process.env.IMTCOREDIR, process.env.PROLIFEDIR].filter(Boolean)
    for (const root of roots) {
        const auxIncludePath = path.resolve(root, 'AuxInclude')
        if (!fs.existsSync(auxIncludePath) || !checkIsDirectorySync(auxIncludePath)) continue

        const targets = fs.readdirSync(auxIncludePath)
            .map(name => path.resolve(auxIncludePath, name))
            .filter(p => checkIsDirectorySync(p))
            .map(p => path.basename(p))

        if (targets.length > 0) {
            process.env.TARGETNAME = targets[0]
            return
        }
    }
}

function getQmlImportPathsFromConfig(testDirPath) {
    const visitedConfigs = new Set()
    const resolvedDirs = new Set()

    function collectConfig(configPath) {
        const normalizedConfigPath = path.resolve(configPath)

        if (visitedConfigs.has(normalizedConfigPath)) return
        visitedConfigs.add(normalizedConfigPath)

        if (!fs.existsSync(normalizedConfigPath)) return

        let config
        try {
            const content = envFill(fs.readFileSync(normalizedConfigPath, { encoding: 'utf8', flag: 'r' }))
            config = JSON.parse(content)
        } catch (err) {
            return
        }

        const configDirPath = path.dirname(normalizedConfigPath)

        if (Array.isArray(config.dirs)) {
            for (const dirPath of config.dirs) {
                const absoluteDirPath = path.resolve(configDirPath, envFill(dirPath))
                if (fs.existsSync(absoluteDirPath) && checkIsDirectorySync(absoluteDirPath)) {
                    resolvedDirs.add(absoluteDirPath)
                    resolvedDirs.add(path.dirname(absoluteDirPath))
                }
            }
        }

        if (Array.isArray(config.includes)) {
            for (const includePath of config.includes) {
                const absoluteIncludePath = path.resolve(configDirPath, envFill(includePath))
                collectConfig(absoluteIncludePath)
            }
        }
    }

    collectConfig(path.resolve(testDirPath, 'test.json'))

    return Array.from(resolvedDirs)
}

function runQmlTest(filePath, timeout = 5000, qmlImportPaths = []) {
    return new Promise((resolve, reject) => {
        const pathSeparator = process.platform === 'win32' ? ';' : ':'
        const mergedImportPath = qmlImportPaths.join(pathSeparator)

        const env = { ...process.env, QT_LOGGING_TO_CONSOLE: "1" }
        if (mergedImportPath) {
            env.QML2_IMPORT_PATH = env.QML2_IMPORT_PATH
                ? `${mergedImportPath}${pathSeparator}${env.QML2_IMPORT_PATH}`
                : mergedImportPath
            env.QML_IMPORT_PATH = env.QML_IMPORT_PATH
                ? `${mergedImportPath}${pathSeparator}${env.QML_IMPORT_PATH}`
                : mergedImportPath
        }

        const importArgs = []
        for (const importPath of qmlImportPaths) {
            importArgs.push('-I', importPath)
        }

        // Запуск в headless режиме
        const child = spawn('qml', ['-platform', 'offscreen', ...importArgs, filePath], {
            cwd: path.dirname(filePath),
            env,
        })

        let logs = []
        let rawOutput = ''

        child.stdout.on('data', (data) => {
            const chunk = data.toString()
            rawOutput += chunk + '\n'
            let result = getQmlLog(chunk.trim())
            logs.push(...result)
        })
        child.stderr.on('data', (data) => {
            const chunk = data.toString()
            rawOutput += chunk + '\n'
            let result = getQmlLog(chunk.trim())
            logs.push(...result)
        })

        // Тайм-аут на случай зависания теста
        const timer = setTimeout(() => {
            child.kill()
            reject(new Error(`Test timed out: ${filePath}`))
        }, timeout)

        child.on('close', (code) => {
            clearTimeout(timer)

            if (logs.length === 0 && rawOutput) {
                const rawLines = rawOutput
                    .split(/\r?\n/g)
                    .map(line => line.trim())
                    .filter(Boolean)

                const diagnostics = rawLines.filter(line =>
                    line.includes('QQmlApplicationEngine failed to load component') ||
                    line.includes('Did not load any objects, exiting.') ||
                    line.startsWith('file:///') ||
                    line.includes(' is not installed') ||
                    line.includes(': error:')
                )

                if (diagnostics.length > 0) {
                    logs.push(...diagnostics)
                } else {
                    logs.push(...rawLines.slice(0, 20))
                }
            }

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
    ensureDefaultTargetName()

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
        const qmlImportPaths = getQmlImportPathsFromConfig(testDirPath)

        if (checkIsDirectorySync(testDirPath)) {
            console.log(`${colors.yellow}[i] Started test: ${testdir}${colors.reset}`)
            testCount++
            let resultDesktop
            let resultWeb

            try {
                resultDesktop = await runQmlTest(mainFilePath, 5000, qmlImportPaths)
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

            if (resultDesktop && resultWeb && resultDesktop.join('') === resultWeb.join('')) {
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

        // Copy image assets to _web for file:// access
        const imageExts = ['.svg', '.png', '.jpg', '.jpeg', '.gif', '.webp', '.bmp', '.ico']
        const testFiles = fs.readdirSync(testDirPath)
        for(const f of testFiles){
            if(imageExts.some(ext => f.toLowerCase().endsWith(ext))){
                fs.copyFileSync(path.resolve(testDirPath, f), path.resolve(testDirPath, '_web', f))
            }
        }
    } catch (err) {
        console.error(`${colors.red}[Error] Error during compilation: ${getErrorMessage(err)}${colors.reset}`)
    }

    try {
        await driver.get('file://' + path.resolve(testDirPath, './_web/test.html'))

        await driver.wait(async () => {
            const readyState = await driver.executeScript('return document.readyState')
            return readyState === 'complete'
        }, timeout) // таймаут 10 секунд

        let mainQml = fs.readFileSync(path.resolve(testDirPath, 'Main.qml'), 'utf-8')
        let containTimer = mainQml.indexOf('Timer') >= 0
        let containAsync = mainQml.indexOf('onStatusChanged') >= 0 || mainQml.indexOf('Image {') >= 0 || mainQml.indexOf('Image{') >= 0

        if (containTimer || containAsync) {
            // Wait for Qt.quit() signal via DOM attribute or timeout
            try {
                await driver.wait(async () => {
                    return await driver.executeScript('return !!window.__jqmlQuit')
                }, timeout)
            } catch(e) {
                // timeout - proceed anyway
            }
        }

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
        options.addArguments('--headless', '--window-size=800,600', '--no-sandbox', '--disable-dev-shm-usage', '--allow-file-access-from-files') // Раскомментировать для headless-режима
        options.setLoggingPrefs({
            browser: 'ALL', // Собирать все типы сообщений (INFO, WARNING, SEVERE)
            driver: 'WARNING'
        })

        let service = new chrome.ServiceBuilder(chromedriverPath)

        let driver = await new Builder()
            .forBrowser('chrome')
            .setChromeOptions(options)
            .setChromeService(service)
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