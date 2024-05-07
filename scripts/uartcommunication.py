import sys
import json
import logging
import queue
import threading
import time

import serial

logger = logging.getLogger("BPY")
logger.setLevel(logging.DEBUG)
        
if not logger.handlers:
    ch = logging.StreamHandler()
    ch.setStream(sys.stdout)
    ch.setLevel(logging.DEBUG)
    ch.setFormatter(logging.Formatter("%(asctime)s %(levelname)s [%(name)s]: %(message)s", datefmt="%Y-%m-%d %H:%M:%S"))
    logger.addHandler(ch)


class UARTTable:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = 115200 #default baud rate if other isn't stated
        self.ser = None
        self.stop_sig = threading.Event()
        self.stop_sig.clear()

        self.data_table = {}
        
    def connect(self):
        logger.info("Connecting to serial port {0}...".format(self.port))
        # close any exising connection
        if self.ser:
            self.ser.close()
        while 1:
            # if main want to exit, we no longer try to connect
            if self.stop_sig.is_set():
                return
            try:
                self.ser = serial.Serial(self.port, baudrate=self.baudrate)
            except serial.serialutil.SerialException as e:
                logger.error(str(e))
                logger.debug("Still trying...")
                time.sleep(0.5)
                continue
            
            # we only reach here if serial connection is established
            if self.ser.is_open:
                break

        logger.info("Connected.")
    
    def stop(self):
        self.stop_sig.set()
        if self.ser:
            self.ser.close()
        logger.info("Stopped.")
    
    def start(self):
        logger.debug("Starting...")
        self.recvPacket()
    
    def startThreaded(self):
        logger.debug("Starting threaded")
        self.t = threading.Thread(target=self.recvPacket)
        self.t.start()

    def get(self, key):
        return self.data_table.get(key)
    
    def recvPacket(self):
        while 1:
            if self.stop_sig.is_set():
                return
            
            code = b""
            buf = b""
            while code != b"\n":
                if self.stop_sig.is_set():
                    return
                buf += code
                try:
                    code = self.ser.read()
                except (AttributeError, TypeError, serial.serialutil.SerialException) as e:
                    print(e)
                    self.connect()
                    continue

            data = buf.decode()
            try:
                data = json.loads(data)
            except json.decoder.JSONDecodeError:
                logger.warning("Packet format error.")
                continue
            key = data.get("key")
            if not key:
                logger.warning("Packet data error.")
                continue
            
            self.data_table[key] = data.get("value")
        

if __name__ == "__main__":
    try:
        # If you run this script it will try this:
        uart_table = UARTTable("COM3")
        uart_table.startThreaded()
        while 1:
            print(uart_table.get("lh0"))
            time.sleep(0.01)
        # But in under no curcumstances you should run this one as it will just lock Blender for some reason
    except KeyboardInterrupt:
        uart_table.stop()
