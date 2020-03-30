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
k4arecorder -l 5 output.mkv
```




