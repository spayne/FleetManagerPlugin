import logging
import logging.handlers
import queue
from threading import Thread
import json

# Python's default behaviour is to slap tracebacks to the end of messages.  This is not good for the json formatting.
#    * return None when requesting to format the exceptin
#    * paste the cleaned up exc_info to the end of record.msg in the format() method
class AppendExceptionToMsgFormatter(logging.Formatter):
    def formatException(self, exc_info):
        return None

    def format(self, record):
        if record.exc_info:
            a = str(record.msg) + " "
            b = a + super().formatException(record.exc_info)
            record.msg = b
        else:
            record.msg = str(record.msg)
        record.msg = json.dumps(record.msg)
        s = super().format(record)
        return s


class FleetBridgeLoggingService:
    def __init__(self, websocket_service):
        self.logging_queue = queue.Queue()
        self.logging_handler = logging.handlers.QueueHandler(self.logging_queue)
        self.json_formatter=AppendExceptionToMsgFormatter('{"time":"%(asctime)s", "name": "%(name)s", "level": "%(levelname)s", "message": %(message)s}') 
        self.logging_handler.setFormatter(self.json_formatter)
        self.websocket_service = websocket_service;
        self.shutting_down = False

    def get_logging_handler(self):
        return self.logging_handler

    def logging_thread_main(self):
        while not self.shutting_down:
            try:
                log_record = self.logging_queue.get(block=True, timeout=1)
                self.websocket_service.send_on_websocket(log_record.msg)
                self.logging_queue.task_done()
            except queue.Empty:
                pass

    def start_thread(self):
        self.logging_thread = Thread(target=self.logging_thread_main)
        self.logging_thread.start()

    def join_thread(self):
        self.shutting_down = True
        self.logging_thread.join()

