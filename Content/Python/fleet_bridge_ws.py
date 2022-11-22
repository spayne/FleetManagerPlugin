# Copyright Sean Payne All Rights Reserved.

import asyncio
import websockets
import time
from threading import Thread, Lock

class FleetBridgeWebSocketService:
    def __init__(self, websocket_port):
        self.connected = set()
        self.connected_lock = Lock()
        self.connected_lock.acquire()
        self.websocket_port = websocket_port

    def wait_for_first_connection(self):
        self.connected_lock.acquire()

    async def handler(self, websocket):
        self.connected.add(websocket)
        self.connected_lock.release()

        try:
            await websocket.recv()
        except websockets.exceptions.ConnectionClosed:
            pass
        finally:
            self.connected.remove(websocket)

    def send_on_websocket(self, json_str):
        for websocket in self.connected.copy():
           coro = websocket.send(json_str)
           future = asyncio.run_coroutine_threadsafe(coro, self.loop)

    async def serve_and_wait(self):
        self.loop = asyncio.get_running_loop()
        self.server = await websockets.serve(self.handler, "localhost", self.websocket_port)
        await self.server.wait_closed()

    def websocket_main(self):
        asyncio.run(self.serve_and_wait())

    async def async_close(self):
        self.server.close()

    def start_thread(self):
        self.thread = Thread(target=self.websocket_main)
        self.thread.start()

    def join_thread(self):
        future = asyncio.run_coroutine_threadsafe(self.async_close(), self.loop)
        self.thread.join()
