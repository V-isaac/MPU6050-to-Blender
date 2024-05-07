import time
import sys
import json
import os
import logging

# importing serial, numpy, blender python and 2nd script
import bpy
import numpy as np
import serial 

# Getting script from this exact folder *relatively* to Blender project
sys.path.append(os.path.join(bpy.path.abspath("//"), "scripts"))
from uartcommunication import UARTTable

# Setting up logger. If everything works may as well just remove it
logger = logging.getLogger("BPY")
logger.setLevel(logging.DEBUG)
if not logger.handlers:
    ch = logging.StreamHandler()
    ch.setStream(sys.stdout)
    ch.setLevel(logging.DEBUG)
    ch.setFormatter(logging.Formatter("%(asctime)s %(levelname)s [%(name)s]: %(message)s", datefmt="%Y-%m-%d %H:%M:%S"))
    logger.addHandler(ch)
         
COM_PORT = "COM3" # Communication port (USB). If you're unsure, use Arduino IDE to find out 
FPS = 30 # Change for slowest/slow/fast communication 24/30/60
uart_table = UARTTable(COM_PORT, 115200) # Start another script as threaded

# Defining functions for operator
# Set positions
def SetRotations(object, rotation):
    w, x, y, z = rotation
    object.rotation_quaternion[0] = w
    object.rotation_quaternion[1] = x
    object.rotation_quaternion[2] = y
    object.rotation_quaternion[3] = z

# Vector magic    
def multQuaternion(q1, q0):
    w0, x0, y0, z0 = q0
    w1, x1, y1, z1 = q1
    return np.array([-x1 * x0 - y1 * y0 - z1 * z0 + w1 * w0,
                     x1 * w0 + y1 * z0 - z1 * y0 + w1 * x0,
                     -x1 * z0 + y1 * w0 + z1 * x0 + w1 * y0,
                     x1 * y0 - y1 * x0 + z1 * w0 + w1 * z0], dtype=np.float64)
    

# Start of an actual script, here we go
class TestScript(bpy.types.Operator):
    # Without these Blender will not run an Operator
    bl_idname = "wm.test_script"
    bl_label = "Invoke test script operator"
    
    _timer = None
    
    # Our loop
    def modal(self, context,event):
        if event.type in {'RIGHTMOUSE', 'ESC'}: #Cancel
            logger.info("BlenderTimer received ESC.")
            self.cancel(context)
            # Alternatively could be used Array of objects
            # Avoiding it for speed
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L"), [1, 0, 0, 0])
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.B"), [1, 0, 0, 0])
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.P"), [1, 0, 0, 0])
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.M"), [1, 0, 0, 0])
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.R"), [1, 0, 0, 0])
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.p"), [1, 0, 0, 0])
            return {'CANCELLED'}
        
        if event.type == 'LEFTMOUSE':            #Применить
            self.cancel(context)
            logger.info("Applying transform")
            return {'FINISHED'}
        
        if event.type == 'TIMER':               # Starting Modal Operator with Timer which will return True every 1/FPS 
            # Getting values via JSON string
            lhand   = uart_table.get("lh0")
            lthumb  = uart_table.get("lh1")
            lpoint  = uart_table.get("lh2")
            lmiddle = uart_table.get("lh3")
            lring   = uart_table.get("lh4")
            lpinky  = uart_table.get("lh5")
            
            # If objects do not exist, just pass trough script without doing anything
            if not lhand or not lthumb or not lpoint or not lmiddle or not lring or not lpinky:
                logger.warning("Invalid joint data")
                return{'PASS_THROUGH'}
            
            logger.debug(str(lhand))
            # Getting arrays from Strings
            lhand   = np.array(lhand)
            lthumb  = np.array(lthumb)
            lpoint  = np.array(lpoint)
            lmiddle = np.array(lmiddle)
            lring   = np.array(lring)
            lpinky  = np.array(lpinky)
            
            # Inverting root bone
            lhandInv = lhand * np.array([1, -1, -1, -1])
            
            # Getting values relatively to root bone
            thumbRel    = multQuaternion(lhandInv, lthumb)
            pointRel    = multQuaternion(lhandInv, lpoint)
            middleRel   = multQuaternion(lhandInv, lmiddle)
            ringRel     = multQuaternion(lhandInv, lring)
            pinkyRel    = multQuaternion(lhandInv, lpinky)
            
            # Set rotations accordingly 
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L"), lhand)
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.B"), thumbRel)
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.P"), pointRel)
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.M"), middleRel)
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.R"), ringRel)
            SetRotations(bpy.data.objects["Armature"].pose.bones.get("hand.L.p"), pinkyRel)
                        
            # Uncoment if refresh rate is too low 
            #bpy.ops.wm.redraw_timer(type = 'DRAW_WIN_SWAP', iterations = 1)
        return {'RUNNING_MODAL'}

    def execute(self, context):
        # Update rate is FPS
        self._timer = context.window_manager.event_timer_add(1./FPS, window=context.window)        
        wm.modal_handler_add(self)
        return {'RUNNING_MODAL'}
    
    def cancel(self, context):
        uart_table.stop()
        context.window_manager.event_timer_remove(self._timer)

if __name__ == "__main__":
    try:
        logger.info("Starting services.")
        bpy.utils.register_class(TestScript)
        uart_table.startThreaded()
        bpy.ops.wm.test_script()
        logger.info("All started.")
    except KeyboardInterrupt:
        uart_table.stop()
        logger.info("Received KeyboardInterrupt, stopped.")

# Uncoment to register in F3 menu    
#Def menu_func(self, context):
#    self.layout.operator(TestScript.bl_idname, text="Modal Operator")

#bpy.utils.register_class(TestScript)
#bpy.types.VIEW3D_MT_object.append(menu_func)
