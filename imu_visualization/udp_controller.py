import socket
import time
import json
from threading import Thread
from datetime import datetime


UDP_IP = '192.168.0.112'
UDP_PORT = 7001
#--------------------------------------------#
#Class to handle UDP communication between
#the master and the slavesself.
#Transmission is using a UDP multicast group
#Reception is on the same UDP socket (alive check)
#--------------------------------------------#
class UDPController:

    def __init__(self):

        self._gyroscope = [0,0,0]

    def __del__(self):

        #Stop communication
        self.stop()

    #Start the client. Todo: error handling
    def begin(self):

        print("Starting UDP payload socket")
        #Create a UDP socket for payload messages (muticast) - Tx only
        self._sock_payload = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._sock_payload.settimeout(1) #Allow safe exit of socket handle thread
        #Create a UDP socket for Alive check - Rx only

        #print("Starting UDP alive socket")
        #self._sock_alive = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        #self._handle_alive = True
        #self._sock_alive.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        #self._sock_alive.bind(('',7002))
        #server_address = ('192.168.2.255',7002) #Todo: get master IP
        #self._sock_alive.bind(server_address)
        #self._sock_alive.settimeout(1) #Allow safe exit of socket handle thread

        #Start alive check thread
        #self._alive_thread = Thread(target = self.handle_alive_check)
        #self._alive_thread.start()

    #Stop sockets and finish thread
    def stop(self):

        print("Stoping UDP payload socket")
        #Close payload socket
        self._sock_payload.close()
        #print("Stoping UDP alive socket")
        #Stop alive thread
        #self._handle_alive = False
        #self._alive_thread.join()
        #Close alive socket
        #self._sock_alive.close()

        #Reset alive internal variables
        #self._last_timestamps = [0,0,0,0,0,0]
        #self._get_last_tx_timestamp = 0

    #Send UDP multicast message
    def send_message(self, msg):

        global UDP_IP
        global UDP_PORT

        self._sock_payload.sendto(msg, (UDP_IP, UDP_PORT))

    def receive_message(self):

        #Receive (with timeout) from the socket

        try:
            data, address = self._sock_payload.recvfrom(128) #Todo: max buffer size?
            print(data)

            received_msg = json.loads(data)

            print('Received data: ', received_msg)

            return [received_msg['x'],received_msg['y'],received_msg['z']]

        except socket.timeout:
            pass

        return [0,0,0]

    #Send synchroniztion request
    def send_mode_req(self, mode):

        print("Sending mode request")

        msgId = 0x01 #Todo: define message IDs in a different file

        m = ''
        m += chr(msgId) + chr(mode)

        self.send_message(m)

    #Send synchroniztion request
    def send_power_req(self, mode):

        print("Sending power request")

        msgId = 0x01 #Todo: define message IDs in a different file

        m = ''
        m += chr(msgId) + chr(mode)

        self.send_message(m)

    def send_brightness_req(self, brightness):

        print("Sending brightness request")

        msgId = 0x03 #Todo: define message IDs in a different file

        m = ''
        m += chr(msgId) + chr(brightness)

        self.send_message(m)

    #send mode change request
    def send_color_req(self, color):

        print("Sending color request")

        msgId = 0x02

        m = ''
        m += chr(msgId) + chr(color[0]) + chr(color[1]) + chr(color[2])

        self.send_message(m)

    def get_gyroscope(self):

        print("Requesting gyroscope info")

        m = ''
        m += chr(0x05)

        self.send_message(m)

        gyroscope_msg = self.receive_message()

        return gyroscope_msg
