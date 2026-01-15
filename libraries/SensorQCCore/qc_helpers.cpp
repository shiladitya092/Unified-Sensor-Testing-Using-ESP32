#include <Arduino.h>

String jsonStringVal(float value)
{
    if (isnan(value))
        return String("null");
    return String(value);
}

String jsonStringVal(double value)
{
    if (isnan(value))
        return String("null");
    return String(value);
}

String jsonStringVal(String value)
{
    String tmp = "\"" + value + "\"";
    return tmp;
}

String jsonStringVal(unsigned long value)
{
    return String(value);
}

String jsonStringVal(unsigned int value)
{
    return String(value);
}

String jsonStringVal(int value)
{
    return String(value);
}

String jsonStringVal(unsigned long long data)
{
    char temp_str[50];
    snprintf(temp_str, 50, "%llu", data);
    return String(temp_str);
}

String jsonStringVal(const char *data)
{
    return jsonStringVal(String(data));
}
