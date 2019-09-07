import socket
import time
from threading import Thread
import json

#--------------------------------------------#
#Class to handle UDP communication between
#the master and the slavesself.
#Transmission is using a UDP multicast group
#Reception is on the same UDP socket (alive check)
#--------------------------------------------#
class VisualizatonController:

    def __init__(self):

        pass

    def __del__(self):

        #Stop communication
        self.stop()

    #Start the client. Todo: error handling
    def begin(self):

        print("Starting UDP socket")
        #Create a UDP socket for payload messages (muticast) - Tx only
        self._sock_visualization = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    #Stop sockets and finish thread
    def stop(self):

        print("Stoping UDP socket")
        #Close payload socket
        self._sock_visualization.close()


    #Send UDP multicast message
    def send_3D_info(self, x, y, z):

        msg = json.dumps({'x': x, 'y': y, 'z': z})

        print("Sending: ", msg)

        self._sock_visualization.sendto(msg, ('', 7777))
