import socket
import time
import json
from threading import Thread
from datetime import datetime


UDP_IP = '192.168.0.112'
UDP_PORT = 7001

class UDPController:

    def __init__(self):

        self._gyroscope = [0,0,0]

    def __del__(self):

        #Stop communication
        self.stop()

    #Start the client.
    def begin(self):

        print("Starting UDP socket")
        self._sock_payload = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._sock_payload.settimeout(1)

    #Stop sockets and finish thread
    def stop(self):

        print("Stoping UDP socket")
        #Close socket
        self._sock_payload.close()

    #Send UDP multicast message
    def send_message(self, msg):

        global UDP_IP
        global UDP_PORT

        self._sock_payload.sendto(msg, (UDP_IP, UDP_PORT))

    def receive_message(self):

        try:
            data, address = self._sock_payload.recvfrom(128)
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

        msgId = 0x00 #Todo: define message IDs in a different file

        m = ''
        m += chr(msgId) + chr(mode)

        self.send_message(m)

    #Send synchroniztion request. Not used
    def send_power_req(self, mode):

        print("Sending power request")

        msgId = 0x99

        m = ''
        m += chr(msgId) + chr(mode)

        self.send_message(m)

    def send_brightness_req(self, brightness):

        print("Sending brightness request")

        msgId = 0x02

        m = ''
        m += chr(msgId) + chr(brightness)

        self.send_message(m)

    #send mode change request
    def send_color_req(self, color):

        print("Sending color request")

        msgId = 0x01

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
