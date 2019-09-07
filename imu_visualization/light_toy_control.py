import visualization_controller
import mqtt_controller
import udp_controller
from datetime import datetime
import random
import time

# MQTT communication controller
mqttController = mqtt_controller.MQTTController()
udpController = udp_controller.UDPController()

visualizer = visualization_controller.VisualizatonController()
gyroscope_info_global = [0,0,0]

def handle_bluetooth_feedback():

    #Get the gyroscope information from the bluetooth controller
    #gyroscope_info = mqttController.get_gyroscope()
    gyroscope_info = udpController.get_gyroscope()

    global gyroscope_info_global
    if(gyroscope_info[0] != gyroscope_info_global[0] or gyroscope_info[1] != gyroscope_info_global[1]  or gyroscope_info[2] != gyroscope_info_global[2] ):

        gyroscope_info_global[0] = gyroscope_info[0]
        gyroscope_info_global[1] = gyroscope_info[1]
        gyroscope_info_global[2] = gyroscope_info[2]

        #Send the information to the visualization
        visualizer.send_3D_info(gyroscope_info[0],gyroscope_info[1],gyroscope_info[2])

if __name__== "__main__":

    #Start MQTT communication
    mqttController.begin()
    visualizer.begin()
    udpController.begin()

    #Endless loop
    do_loop = True
    while do_loop:

        try:

            #Get new MQTT commands

            #Send bluetooth commands

            #Handle received bluetooth information
            handle_bluetooth_feedback()

            time.sleep(0.01)

        except KeyboardInterrupt:
                print "Ctrl-c received! Sending kill to threads..."
                # Display al lamps as OFFLINE in the Dashboard
                mqtt_controller.publish_alive_rx_status(0)
                #Stop MQTT controller
                mqtt_controller.stop()
                #Stop UDP controller
                udp_handler.stop()
                do_loop = False
