from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import sys
import time
import urllib
from pathlib import PurePosixPath
from collections import deque
from threading import Thread
import logging
import importlib

class BadRequest(Exception):
        pass

class BadResource(Exception):
        pass

def PopOneOrBadRequest(queue):
    try:
        item1 = queue.popleft()
    except IndexError:
        raise(BadRequest)
    return item1


def PopTwoOrBadRequest(queue):
    try:
        item1 = queue.popleft()
        item2 = queue.popleft()
    except IndexError:
        raise(BadRequest)
    return item1,item2

initialized_backends = {}
logging_handler = None

class _HTTPRequestHandler(BaseHTTPRequestHandler):

    def _check_backend(self, backend_module_name):
        global initialized_backends
        global logging_handler
        backend_module = initialized_backends.get(backend_module_name)
        if backend_module is None:
            print(f'loading backend {backend_module_name}')
            backend_module = importlib.import_module(backend_module_name)
            initialized_backends[backend_module_name] = backend_module
        else:
            importlib.reload(backend_module)
        getattr(backend_module, 'logging_install_handler')(logging_handler)
        getattr(backend_module, 'logging_set_level')(logging.INFO)
        #getattr(backend_module, 'test_sending_logs')()
        return backend_module

    def _send_response(self, response_code, response_dict):
        self.send_response(response_code)
        self.send_header('Content-type','application/json')
        self.end_headers()
        if response_dict is not None:
            self.wfile.write(json.dumps(response_dict).encode("utf-8"))

    # returns response_code, dict
    def _handle_request(self, part_queue, dict, verb):
        part_queue.popleft()
        backend_name, backend_id = PopTwoOrBadRequest(part_queue)
        backend_module = self._check_backend(backend_name)
        if backend_module:
            return getattr(backend_module, 'handle_request')(part_queue, dict, verb)
        else:
            raise BadResource

    def _dequote(self, s):
        """
        If a string has single or double quotes around it, remove them.
        Make sure the pair of quotes match.
        If a matching pair of quotes is not found,
        or there are less than 2 characters, return the string unchanged.
        """
        if (len(s) >= 2 and s[0] == s[-1]) and s.startswith(("'", '"')):
            return s[1:-1]
        return s

    # query dict returns a dictionary of key->list and I just want list->key
    # also trim any excess quotes
    def _simplify_dict(self, query_dict):
        simple_dict = {}
        for key in query_dict:
            simple_dict[key] = self._dequote(query_dict[key][0])
        return simple_dict

    def _do_request(self, verb):
        try:
            o = urllib.parse.urlparse(self.path)
            part_queue = deque(PurePosixPath(o.path).parts)
            query_dict = urllib.parse.parse_qs(o.query)
            simple_dict = self._simplify_dict(query_dict)
            [response_code, response_dict] = self._handle_request(part_queue, simple_dict, verb)
            self._send_response(response_code, response_dict)
        except BadRequest:
            self._send_response(400, None)
        except BadResource:
            self._send_response(404, None)
      
    def do_POST(self):
        try:
             self._do_request("POST")
        except BaseException as err:
             print(err)
             self._send_response(400, None)

    def log_message(self, format, *args):
         msg = format%args

class FleetBridgeHttpService:
    def __init__(self, http_port, logging_handler_in):
        self.http_port = http_port
        global logging_handler
        logging_handler = logging_handler_in
        

    def http_server_main(self, port):
        self.httpd = HTTPServer(('', port), _HTTPRequestHandler)
        self.httpd.serve_forever()

    def start_thread(self):
        self.http_thread = Thread(target=self.http_server_main, args=[self.http_port])
        self.http_thread.start()

    def join_thread(self):
        self.httpd.shutdown();
        self.http_thread.join()

if __name__ == '__main__':
    print("a")
    fb = FleetBridgeHttpService(1024)
    print("b")
    fb.start_thread();
    print("c")
    time.sleep(1)
    print("d")
    fb.join_thread()
    print("e")
