Secure PubSub Client
=================

IoT applications very neatly fit into our daily lifes and simplify many tasks. However, many IoT applications neglect the security in favor great flexibility and fast development progress. Nonetheless, secure connections and reliable messages should be key ingredient for all the IoT ideas out there. As good examples are rare, this project gives a short example on how to implment an mqtts client using SSL/TLS secured connections and verification of the server's certificate chain on the ESP8266 device. This allows trustworthy communication and abandons significant security leaks.

## Prerequisites

The application has been developed to run on the ESP8266. Other devices might also work with the implementation, however, keep in mind that security comes on the cost of computational power and ram required. Thus, it's doubtful that the encryption can be done by weaker devices.

The implementation relies on the standard ESP8266 framework (see installation procedure [here](https://dzone.com/articles/programming-the-esp8266-with-the-arduino-ide-in-3)) that comes with a WifiClientSecure implementation (using BearSSL) and general wifi features. Furthermore, the PubSubClient library is used for mqtt actions. The latter can be easily installed in the Arduino-IDE using the library manager.

## Getting Started

Before running the script on your device the standard configuration has to be adapted. Hence, the code contains a configuration block.

```
// your wifi credentials
const char *wifi_ssid = "your-ssid";
const char *wifi_pass = "your-passwd";

// your target server
const char *   mqtt_server = "your-domain"; // e.g. mqtt.flespi.io
const uint16_t port = 8883; // your port. often 8883 is used for SSL
char mqtt_user[20] = "your-username"; // not compulsory only if your broker needs authentication
char mqtt_pass[20] = "your-passwd"; // not compulsory only if your broker needs authentication

// your ca root certificate in base64 format. This is the certificate of 'ISRG Root X1' used by Let's Encrypt.
// Take care when copying that you do not introduce any whitespaces!
static const char digicert[] PROGMEM = ...
```

The 'wifi_ssid' and 'wifi_pass' strings allow the application to connect to your local wifi. 'mqtt_server' and 'port' define the domain name and the port the target mqtt broker is running. Furthermore, for authentication of the client, the 'mqtt_user' and 'mqtt_pass' define the user credentials. Due to the ssl/tls encryption of the connection providing the authentication finally makes sense. Compared to that, in an unencrypted connection, the username and password are transfered as plain text and, thus, do not really increase security.

The final task for the configuration is setting the correct public root ca certificate. This step guarantees that the client always communicates with your server and significantly complicates man-in-the-middle attacks. However, for the client to authenticate the server, it must be able to verify the certificate. If you want to run your own mqtt broker you need to provide a certificate that is trustworthy. To generate this trust, the certificate is signed by some certificate authority (CA) that is trustworthy (e.g. governments). This leads to a certificate chain such that only one especially trusty root certificate is required to show the authenticity of the mqtt broker (also all intermediate certificates can be used for verification purposes. However, keep in mind that intermediate certificates do expire faster than root certificates.). This certificate should be provided by your broker cloud service or, in the case you run your own broker, is the public key of your key-pair (In the case that you use self-signed certificates you have to directly trust your certificate as it is not part of a chain). These keys can be provided in various formats. For the application to work, the public key of the root certificate must be in the base64 format. Hint: this certificate can also be exported by many web browsers like chrome and firefox. Just update the 'digicert' variable. It currently contains the 'ISRG Root X1' certificate that is also used by Let's encrypt.

If all the configuration is done, the application is ready to work and transmit your data encrypted via SSL/TLS.

Keep Secure...

## Comments

If you find any bugs or improvements don't hasitate to open an issue and/or to fork the repo.