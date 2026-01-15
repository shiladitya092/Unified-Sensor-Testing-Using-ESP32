#ifndef DRIVERS_H
#define DRIVERS_H

#define SENSOR_ProbeSensor 1
#define SENSOR_MultiProbeSensor 1
#define SENSOR_DHTSensor 1
#define SENSOR_MDHRegister 1

void* createSensorObject(String sensor_class);

#endif