# Copyright Sean Payne All Rights Reserved.
# fleet_bridge.py
# provide a mechanism to communicate between UE and the fleet backends

from fleet_bridge_logging import FleetBridgeLoggingService
from fleet_bridge_http import FleetBridgeHttpService
from fleet_bridge_ws import FleetBridgeWebSocketService


class FleetBridge:
    def __init__(self, http_port, websocket_port):
        self.websocket_service = FleetBridgeWebSocketService(websocket_port)
        # logging service sends across websocket
        self.logging_service = FleetBridgeLoggingService(self.websocket_service)
        # http service needs to give backend the logging handler to install
        self.http_service = FleetBridgeHttpService(http_port, 
                                    self.logging_service.get_logging_handler()) 

    def start_threads(self):
        self.websocket_service.start_thread()
        self.logging_service.start_thread()
        self.http_service.start_thread()

    def join_threads(self):
        self.http_service.join_thread()
        self.logging_service.join_thread()
        self.websocket_service.join_thread()


def start_servers(http_port, websocket_port):
    global fleet_bridge
    fleet_bridge = FleetBridge(http_port, websocket_port)
    fleet_bridge.start_threads()
    return 200
    
def stop_servers():
    global fleet_bridge
    fleet_bridge.join_threads()

if __name__ == '__main__':
    start_servers(9000, 9765)
    stop_servers()
