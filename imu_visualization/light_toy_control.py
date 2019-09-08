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

stick_on = False
stick_mode = 0
stick_color = [0,0,0]
stick_brightness = 0

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


def handle_mqtt_communication():

    global stick_on, stick_mode, stick_color, stick_brightness

    #Get current info from MQTT controller
    stick_on_tmp = mqttController.get_power()
    stick_mode_tmp = mqttController.get_mode()
    stick_color_tmp = mqttController.get_color()
    stick_brightness_tmp = mqttController.get_brightness()

    if(stick_on_tmp != stick_on):
        stick_on = stick_on_tmp

    if(stick_mode_tmp != stick_mode):
        stick_mode = stick_mode_tmp
        udpController.send_mode_req(stick_mode)

    if(stick_brightness != stick_brightness_tmp):
        stick_brightness = stick_brightness_tmp
        udpController.send_brightness_req(stick_brightness)

    if(stick_color_tmp[0] != stick_color[0] or stick_color_tmp[1] != stick_color[1] or stick_color_tmp[2] != stick_color[2]):
        stick_color[0] = stick_color_tmp[0]
        stick_color[1] = stick_color_tmp[1]
        stick_color[2] = stick_color_tmp[2]

        udpController.send_color_req(stick_color)

    if(stick_mode_tmp != stick_mode):
        stick_mode = stick_mode_tmp
        udpController.send_mode_req(stick_mode)


if __name__== "__main__":

    #Start MQTT communication
    mqttController.begin()
    visualizer.begin()
    udpController.begin()

    #Endless loop
    do_loop = True
    while do_loop:

        try:

            #Handle MQTT communication
            handle_mqtt_communication()

            #Handle 3D visualization
            if(mqttController.is3DFeedback()):
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
