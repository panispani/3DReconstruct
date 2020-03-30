# Azure Kinect DK notes

Developer kit with AI sensors (depth sensor, spatial microphone array with a video camera, and orientation sensor)

SDKs:
- Sensor SDK for low-level sensor and device access.
- Body Tracking SDK for tracking bodies in 3D.
- Speech Cognitive Services SDK for enabling microphone access and Azure cloud-based speech services.

For our purposes we only need the Sensor SDK.

# Installing and configuring the sensor SDK
https://docs.microsoft.com/en-us/azure/kinect-dk/sensor-sdk-download

## Linux
- configure Microsoft's Package Repository
18.04
```
curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
 sudo apt-add-repository https://packages.microsoft.com/ubuntu/18.04/prod
 sudo apt-get update
```

- Install k4a-tools
Includes the Azure Kinect Viewer, the Azure Kinect Recorder, and the Azure Kinect Firmware Tool
```
sudo apt install k4a-tools
sudo apt install libk4a1.1-dev
```

The SDK is now ready to use! You can test it by recording for 5 seconds. By default, the recorder uses the NFOV Unbinned depth mode and outputs 1080p RGB at 30 fps including IMU data.

```
sudo k4arecorder -l 5 output.mkv
```
or just use the GUI to view and record (better to see if the camera is properly configured)
```
sudo k4aviewer
```

## Troubleshooting ubuntu problems
```
lsmod |grep xhc
```
Try reading the messages in
```
dmesg
```
Example:
```
[ 3649.478205] usb 8-1: new high-speed USB device number 17 using xhci_hcd
[ 3649.632426] usb 8-1: New USB device found, idVendor=045e, idProduct=097b
[ 3649.632437] usb 8-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 3649.632442] usb 8-1: Product: 4-Port USB 2.0 Hub
[ 3649.632447] usb 8-1: Manufacturer: Generic
[ 3649.632451] usb 8-1: SerialNumber: 000161694812
[ 3649.633522] hub 8-1:1.0: USB hub found
[ 3649.636089] hub 8-1:1.0: 4 ports detected
[ 3649.926348] usb 8-1.3: new high-speed USB device number 18 using xhci_hcd
[ 3650.032159] usb 8-1.3: New USB device found, idVendor=045e, idProduct=097e
[ 3650.032165] usb 8-1.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 3650.032169] usb 8-1.3: Product: Azure Kinect Microphone Array
[ 3650.032176] usb 8-1.3: Manufacturer: Microsoft Corporation.
[ 3650.032180] usb 8-1.3: SerialNumber: 000161694812
[ 3650.041828] hid-generic 0003:045E:097E.000A: hiddev0,hidraw0: USB HID v1.11 Device [Microsoft Corporation. Azure Kinect Microphone Array] on usb-0000:00:10.1-1.3/input2
[ 3650.266271] usb 8-1.1: new high-speed USB device number 19 using xhci_hcd
[ 3650.372182] usb 8-1.1: config 1 has 3 interfaces, different from the descriptor's value: 4
[ 3650.372188] usb 8-1.1: config 1 has no interface number 2
[ 3650.372207] usb 8-1.1: Duplicate descriptor for config 1 interface 3 altsetting 0, skipping
[ 3650.375497] usb 8-1.1: New USB device found, idVendor=045e, idProduct=097d
[ 3650.375504] usb 8-1.1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 3650.375508] usb 8-1.1: Product: Azure Kinect 4K Camera
[ 3650.375511] usb 8-1.1: Manufacturer: Microsoft
[ 3650.375514] usb 8-1.1: SerialNumber: 000161694812
[ 3650.377056] uvcvideo: Found UVC 1.00 device Azure Kinect 4K Camera (045e:097d)
[ 3650.378811] uvcvideo: Failed to set UVC probe control : -32 (exp. 26).
[ 3650.382017] uvcvideo 8-1.1:1.0: Entity type for entity Extension 2 was not initialized!
[ 3650.382113] uvcvideo 8-1.1:1.0: Entity type for entity Extension 6 was not initialized!
[ 3650.382119] uvcvideo 8-1.1:1.0: Entity type for entity Processing 3 was not initialized!
[ 3650.382123] uvcvideo 8-1.1:1.0: Entity type for entity Camera 1 was not initialized!
[ 3650.382582] input: Azure Kinect 4K Camera as /devices/pci0000:00/0000:00:10.1/usb8/8-1/8-1.1/8-1.1:1.0/input/input28
[ 3650.614163] usb 8-1.2: new high-speed USB device number 20 using xhci_hcd
[ 3650.719723] usb 8-1.2: config 1 has an invalid interface number: 5 but max is 3
[ 3650.719729] usb 8-1.2: config 1 has no interface number 2
[ 3650.719741] usb 8-1.2: config 1 interface 1 altsetting 0 bulk endpoint 0x81 has invalid maxpacket 1024
[ 3650.723173] usb 8-1.2: New USB device found, idVendor=045e, idProduct=097c
[ 3650.723180] usb 8-1.2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 3650.723184] usb 8-1.2: Product: Azure Kinect Depth Camera
[ 3650.723187] usb 8-1.2: Manufacturer: Microsoft
[ 3650.723190] usb 8-1.2: SerialNumber: 000161694812
[ 3650.725229] uvcvideo: Found UVC 1.00 device Azure Kinect Depth Camera (045e:097c)
[ 3650.728393] uvcvideo 8-1.2:1.0: Entity type for entity Extension 2 was not initialized!
[ 3650.728401] uvcvideo 8-1.2:1.0: Entity type for entity Extension 6 was not initialized!
[ 3650.728406] uvcvideo 8-1.2:1.0: Entity type for entity Processing 3 was not initialized!
[ 3650.728410] uvcvideo 8-1.2:1.0: Entity type for entity Camera 1 was not initialized!
[ 3650.728686] input: Azure Kinect Depth Camera as /devices/pci0000:00/0000:00:10.1/usb8/8-1/8-1.2/8-1.2:1.0/input/input29
[ 3650.734177] hid-generic 0003:045E:097C.000B: hiddev1,hidraw1: USB HID v1.10 Device [Microsoft Azure Kinect Depth Camera] on usb-0000:00:10.1-1.2/input5
```

So for some reason 
[ 3650.378811] uvcvideo: Failed to set UVC probe control : -32 (exp. 26).
and
[ 3650.728393] uvcvideo 8-1.2:1.0: Entity type for entity Extension 2 was not initialized!
[ 3650.728401] uvcvideo 8-1.2:1.0: Entity type for entity Extension 6 was not initialized!
[ 3650.728406] uvcvideo 8-1.2:1.0: Entity type for entity Processing 3 was not initialized!
[ 3650.728410] uvcvideo 8-1.2:1.0: Entity type for entity Camera 1 was not initialized!

### Things to try

You can try to removing and adding the kernel module
```
sudo rmmod uvcvideo
sudo modprobe uvcvideo
unplug camera from USB port
plug camera back in
```

Install uvcdynctrl package which installs suitable udev rules to initialize the missing controls from userspace.
```
sudo apt install uvcdynctrl
```

Camera doesn't work on USB3.0

Only the microphone works

Current status: Error: OpenGL 4.4 context creation failed. You could try updating your graphics drivers.
find graphics card
```
lspci | grep VGA
```

Check openGL version
```
sudo apt-get install mesa-utils
glxinfo | grep "OpenGL version"
glxinfo | grep "version"
```

OpenGL 3.1 on laptop

3 ideas:

openGL stuff

https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md

use the API directly




