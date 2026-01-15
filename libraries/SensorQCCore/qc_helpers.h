#ifndef QC_HELPERS_H
#define QC_HELPERS_H

String jsonStringVal(float value);
String jsonStringVal(double value);
String jsonStringVal(String value);
String jsonStringVal(unsigned long value);
String jsonStringVal(unsigned int value);
String jsonStringVal(int value);
String jsonStringVal(unsigned long long data);
String jsonStringVal(const char *data);

#endif