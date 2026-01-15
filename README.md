# sensor-qc-firmware

# Version 199.101 

If the project is compiled from Arduino IDE, the following symlinks should be made to the "libraries" folder of the IDE.

Arduino\libraries\SensorQCCore -- libraries\SensorQCCore

Arduino\libraries\SensorQCDrivers -- libraries\SensorQCDrivers

Arduino\libraries\SensorQCSenders -- libraries\SensorQCSenders

Steps (Windows):
Run cmd as Administrator

mklink /D "Path\to\Arduino\libraries\SensorQCCore" "Path\to\SensorQC\libraries\SensorQCCore"

mklink /D "Path\to\Arduino\libraries\SensorQCDrivers" "Path\to\SensorQC\libraries\SensorQCDrivers"

mklink /D "Path\to\Arduino\libraries\SensorQCSenders" "Path\to\SensorQC\libraries\SensorQCSenders"
