import serial
import RPi.GPIO as GPIO
from time import sleep
import paho.mqtt.client as mqtt
import json

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)

buzzer = 13
host='localhost'
at = '4k2Zlg5hwUVuVvUxbx5X'
#at = 'nTrogx5Bn9QQkI3bljFr'
container_id=0
humidity=0
temperature=0.00
battery=0
GPIO.setup(buzzer,GPIO.OUT)
GPIO.setup(11,GPIO.OUT)

port = "/dev/ttyACM0"
s = serial.Serial(port,9600)
s.flushInput()
def on_connect(client,userdata,rc,*extra_params):
        print("connected")
        client.subscribe('v1/devices/me/attributes')
def on_message(client,userdata,msg):
	print("message arrived: "+str(msg.payload))
	d = json.loads(msg.payload.decode('utf-8'))
	if(d['gpio']==True):
		print("led on")
		GPIO.output(buzzer,GPIO.HIGH)
	else:
		print("led off")
		GPIO.output(buzzer,GPIO.LOW)
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(at)
client.connect(host,1883,60)
client.loop_start()
ack=""
	
while True:
	try:
		if s.inWaiting()>0:
			value = s.readline().decode('utf-8')
			if len(value)==6:
				container_id=value[:-2]
				print("status from container id: "+container_id)
			elif len(value)==5:
                                battery=value[:-3]
                                print("battery level:- "+str(battery)+"\n")
			elif len(value)==4:
				humidity = value[:-2]
				print("humidity is: "+humidity)
			elif len(value)==7:
				temperature=value[:-2]
				print("temperature is: "+temperature)

			if(container_id!=0 or temperature!=0 or humidity!=0 or battery!=0):
				data = {'container_id':container_id, 'temperature':temperature,'humidity':humidity,'battery':battery}
				client.publish('v1/devices/me/telemetry', json.dumps(data), 1)
	except OSError:
		pass
