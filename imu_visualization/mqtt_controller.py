import time
import paho.mqtt.client as paho
import json
from datetime import datetime

BROKER_IP = "192.168.0.121"
BROKER_PORT = 1883

class MQTTController:

    def __init__(self):

        #Create client
        self._client = paho.Client("LightToyController")
        #Define callback for received topics
        self._client.on_message =  self.callback

        self._newMsg = False
        self._color = [0,0,0]
        self._mode = 0
        self._power = False
        self._brightness = 0

        self._gyroscope = [0,0,0]

        self._is3DFeedback = False

    def __del__(self):

        #Stop the MQTT client
        self.stop()

    def callback(self, client, userdata, message):

        #Parse Json message
        received_msg = json.loads(message.payload)
        print("received message =",received_msg)
        self._newMsg = True

        #Handle configuration message
        if(message.topic == "light_toy/power"):

            print("Received a power message")

        #Handle configuration message
        elif(message.topic == "light_toy/enable_visualization"):

            print("Received a visualization enable message")

            if( int(received_msg == 1) ):
                self._is3DFeedback = True
            else:
                self._is3DFeedback = False

        #Handle mode request message
        elif(message.topic == "light_toy/mode"):

            print("Received a mode request message")

            self._mode = int(received_msg)

        #Handle mode request message
        elif(message.topic == "light_toy/color"):

            print("Received a color request message")

            self._color[0] = int(received_msg['r'])
            self._color[1] = int(received_msg['g'])
            self._color[2] = int(received_msg['b'])

        elif(message.topic == "light_toy/brightness"):

            print("Received a brightness request message")

            self._brightness = int(received_msg)

        elif(message.topic == "light_toy/gyroscope/x"):

            print("Received a gyroscope message X (debug)")

            self._gyroscope[0] = int(received_msg)

        elif(message.topic == "light_toy/gyroscope/y"):

            print("Received a gyroscope message Y (debug)")

            self._gyroscope[1] = int(received_msg)

        elif(message.topic == "light_toy/gyroscope/z"):

            print("Received a gyroscope message Z (debug)")

            self._gyroscope[2] = int(received_msg)

    #Start the client
    def begin(self):

        print("Starting MQTT client")

        global BROKER_IP, BROKER_PORT

        self._client.connect(BROKER_IP, BROKER_PORT)
        #Start client thread
        self._client.loop_start()

        #Subscribe to topics
        self._client.subscribe("light_toy/brightness")
        self._client.subscribe("light_toy/enable_visualization")
        self._client.subscribe("light_toy/gyroscope/x")
        self._client.subscribe("light_toy/gyroscope/y")
        self._client.subscribe("light_toy/gyroscope/z")
        self._client.subscribe("light_toy/color")
        self._client.subscribe("light_toy/mode")
        self._client.subscribe("light_toy/power")


    def stop(self):

        print("Stopping MQTT client")
        self._client.disconnect() #disconnect
        self._client.loop_stop() #stop loop thread

    def is3DFeedback(self):

        return self._is3DFeedback

    #Check for new mode change message and reset the flag
    def is_new_msg_mode(self):

        retVal = self._newMsg
        self._newMsg = False

        return retVal

    def get_color(self):

        return self._color

    def get_power(self):

        return self._power

    def get_brightness(self):

        return self._brightness

    def get_gyroscope(self):

        return self._gyroscope

    def get_mode(self):

        return self._mode
