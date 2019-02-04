#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

// baud rate of the serial monitor
#define SERIAL_BAUD 115200
// enable traces (0 to disable)
#define TRACE 1

//trace
void trc(String msg){
  if (TRACE) {
  Serial.println(msg);
  }
}

// ---------- Configurations ---------------

// your wifi credentials
const char* wifi_ssid = "your-ssid";
const char* wifi_pass = "your-passwd";

// your target server
const char*   mqtt_server = "your-domain"; // e.g. mqtt.flespi.io
const uint16_t port = 8883; // your port. often 8883 is used for SSL
const char* mqtt_user = "your-username"; // not compulsory only if your broker needs authentication
const char* mqtt_pass = "your-passwd"; // not compulsory only if your broker needs authentication

// your ca root certificate in base64 format. This is the certificate of 'ISRG Root X1' used by Let's Encrypt.
// Take care when copying that you do not introduce any whitespaces!
static const char digicert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow
PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD
Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O
rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq
OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b
xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw
7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD
aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV
HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG
SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69
ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr
AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz
R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5
JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo
Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ
-----END CERTIFICATE-----
)EOF";

// ---------- Configurations End ---------------

// mqtt settings
#define Gateway_Name "TestMQTTClient"
#define will_Topic Gateway_Name "/LWT"
#define will_QoS 0
#define will_Retain true
#define will_Message "Offline"


// ssl client that will verify server certificates
BearSSL::WiFiClientSecure eClient;

// client link to pubsub mqtt
PubSubClient client(eClient);

// load certificates from base64 string
BearSSL::X509List cert(digicert);

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  trc(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  trc(F("Current time: "));
  trc(asctime(&timeinfo));
}  


void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  trc(F("Connecting to "));
  trc(wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  trc(F("WiFi connected"));
  trc(F("IP address: "));
  trc(WiFi.localIP().toString());

  // report mac address
  trc(F("OpenMQTTGateway mac: "));
  trc(WiFi.macAddress()); 

  // report ip  
  trc(F("OpenMQTTGateway ip: "));
  trc(WiFi.localIP().toString());

  // set the loaded certificates as trusted authorities
  eClient.setTrustAnchors(&cert);

  // get time to verify certificate timeouts
  setClock();

  // set target ip for PubSubClient
  trc(F("Connecting to MQTT with mqtt hostname"));
  client.setServer(mqtt_server, port);
}

void loop() {
  // start mqtt connection
  trc(F("Connecting to MQTT borker..."));
  if (client.connect(Gateway_Name, mqtt_user, mqtt_pass, will_Topic, will_QoS, will_Retain, will_Message)) {
    // successfully connected to mqtt broker
    trc(F("Connection established!"));
    // publish sth.
    trc(F("Do the MQTT stuff"));
    // TODO do your own stuff here if you only want to publish
    client.publish("esp8266/online", "1");
    // disconnect from broker again
    client.disconnect();
  }
  else {
    // connecting to broker failed
    trc(F("Connection to broker failed."));
  }

  // take some rest
  delay(5000);
}
