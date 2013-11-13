#ifndef M2XStreamClient_template_h
#define M2XStreamClient_template_h

// Implementations of template functions

int print_encoded_string(Print* print, const char* str);

template <class T>
int M2XStreamClient::send(const char* feedId, const char* streamName, T value) {
  if (_client->connect(_host, _port)) {
    DBGLN("%s", "Connected to M2X server!");
    writeSendHeader(feedId, streamName,
                    //  for {"value": and }
                    _null_print.print(value) + 10);
    _client->print("{\"value\":");
    _client->print(value);
    _client->print("}");
  } else {
    DBGLN("%s", "ERROR: Cannot connect to M2X server!");
    return E_NOCONNECTION;
  }

  return readStatusCode(true);
}

template <class T>
inline int write_multiple_values(Print* print, int streamNum,
                                 const char* names[], const int counts[],
                                 const char* ats[], T values[]) {
  int bytes = 0, value_index = 0;
  bytes += print->print("{\"values\":{");
  for (int i = 0; i < streamNum; i++) {
    bytes += print->print("\"");
    bytes += print->print(names[i]);
    bytes += print->print("\":[");
    for (int j = 0; j < counts[i]; j++) {
      bytes += print->print("{");
      if (ats && ats[value_index]) {
        bytes += print->print("\"at\": \"");
        bytes += print->print(ats[value_index]);
        bytes += print->print("\",");
      }
      bytes += print->print("\"value\": \"");
      bytes += print->print(values[value_index]);
      bytes += print->print("\"}");
      if (j < counts[i] - 1) { bytes += print->print(","); }
      value_index++;
    }
    bytes += print->print("]");
    if (i < streamNum - 1) { bytes += print->print(","); }
  }
  bytes += print->print("}}");
  return bytes;
}

template <class T>
int M2XStreamClient::sendMultiple(const char* feedId, int streamNum,
                                  const char* names[], const int counts[],
                                  const char* ats[], T values[]) {
  if (_client->connect(_host, _port)) {
    DBGLN("%s", "Connected to M2X server!");
    int length = write_multiple_values(&_null_print, streamNum, names,
                                       counts, ats, values);
    _client->print("POST /v1/feeds/");
    print_encoded_string(_client, feedId);
    _client->println(" HTTP/1.0");
    writeHttpHeader(length);
    write_multiple_values(_client, streamNum, names, counts, ats, values);
  } else {
    DBGLN("%s", "ERROR: Cannot connect to M2X server!");
    return E_NOCONNECTION;
  }
  return readStatusCode(true);
}

template <class T>
static int write_location_data(Print* print, const char* name,
                               T latitude, T longitude,
                               T elevation) {
  int bytes = 0;
  bytes += print->print("{\"name\":\"");
  bytes += print->print(name);
  bytes += print->print("\",\"latitude\":\"");
  bytes += print->print(latitude);
  bytes += print->print("\",\"longitude\":\"");
  bytes += print->print(longitude);
  bytes += print->print("\",\"elevation\":\"");
  bytes += print->print(elevation);
  bytes += print->print("\"}");
  return bytes;
}

static int write_location_data(Print* print, const char* name,
                               double latitude, double longitude,
                               double elevation) {
  int bytes = 0;
  bytes += print->print("{\"name\":\"");
  bytes += print->print(name);
  bytes += print->print("\",\"latitude\":\"");
  bytes += print->print(latitude, MAX_DOUBLE_DIGITS);
  bytes += print->print("\",\"longitude\":\"");
  bytes += print->print(longitude, MAX_DOUBLE_DIGITS);
  bytes += print->print("\",\"elevation\":\"");
  bytes += print->print(elevation);
  bytes += print->print("\"}");
  return bytes;
}

template <class T>
int M2XStreamClient::updateLocation(const char* feedId,
                                    const char* name,
                                    T latitude,
                                    T longitude,
                                    T elevation) {
  if (_client->connect(_host, _port)) {
    DBGLN("%s", "Connected to M2X server!");

    int length = write_location_data(&_null_print, name, latitude, longitude,
                                     elevation);
    _client->print("PUT /v1/feeds/");
    print_encoded_string(_client, feedId);
    _client->println("/location HTTP/1.0");

    writeHttpHeader(length);
    write_location_data(_client, name, latitude, longitude, elevation);
  } else {
    DBGLN("%s", "ERROR: Cannot connect to M2X server!");
    return E_NOCONNECTION;
  }
  return readStatusCode(true);
}

#endif