import paho.mqtt.client as mqtt
import mysql.connector
from datetime import datetime

# MySQL DB config
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="tdswaterquality"
)
cursor = db.cursor()

# Create table if not exists
cursor.execute("""
CREATE TABLE IF NOT EXISTS tds_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    tds_value FLOAT,
    timestamp DATETIME
)
""")

# MQTT Callback
def on_message(client, userdata, msg):
    tds_value = float(msg.payload.decode())
    now = datetime.now()
    print(f"Received TDS: {tds_value} ppm")

    cursor.execute("INSERT INTO tds_log (tds_value, timestamp) VALUES (%s, %s)", (tds_value, now))
    db.commit()

# MQTT Setup
client = mqtt.Client()
client.connect("broker.emqx.io", 1883, 60)
client.subscribe("watermonitor/tds")
client.on_message = on_message

print("Listening to MQTT topic 'watermonitor/tds' and logging to MySQL...")
client.loop_forever()
