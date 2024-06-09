# MPU6050-to-Blender
Test project of mine to link rotation sensor and Blender bones

If you have any questions or/and suggestions - hit me in Discord 
@v.isaac is my tag

# Blender rigging
To make it work perfectly you should prepare your models to take data from sensors

First of all, bone placement
All bones must be rotated Z up. No local rotation relatively to global axis.
Note that it has to be done in Edit Mode of armature you're selecting in code.
If bones cannot be rotated straight along global axis - Disconnect them from Parent Bone.

Since this particular model gets data for Forearm, Elbow, Hand and Fingers, they all get data differently.
Fingers should have constraints, which transform that one bone rotation into 3 bone rotations.
I've done it trough Copy Rotation constraint with 1/3 of influence. This way 1 bone corresponds to 3 bones on a fingers.
// this is wrong, im thinking about why it is wrong though //

Some finger bones should not be rotated by certain axis. You should use Limit Rotation constrains.

Don't worry about rotating bones outside of their natural range. Sensors will handle it as far as you're not breaking your own bones.

# Calibration
There's Calibration.ino. Modify ``#define`` values to your needs. Do not go out of suggested range

Upload it to Arduino, then send respective numbers to serial (0-7)

Once you get all offsets, go to quaternion_hand.ino and put calculated offsets there

# Scripting
While Arduino code and Blender's Serial Communication should not be touched, cause they work just fine.
Blender's script 'Diploma_Quaternions' should be changed accordingly to your model:

You **need** to change `COM_PORT` variable;

in `class TestScript` in `def modal()` at rows 70-77 change bones to your need;

in `class TestScript` in `def modal()` at rows 129-136 change bones to your need;

they should match, otherwise it will either not assign values or won't reset them;

in `class TestScript` in `def modal()` at rows 113-126 you should:
 - invert any parent bone
 - use `variable = multQuaternion(parent, variable)`

this way you're getting Child Bone rotation relatively to Parent bone.
