#ifndef WIFI_SERIAL_OTA_H
#define WIFI_SERIAL_OTA_H

#include <Preferences.h>

Preferences prefs;

void set_hostname(const String& new_id) {
    prefs.begin("TME69", false); // Open namespace, false = read/write
    
    // Truncate to 16 chars
    String truncated = new_id.substring(0, 16);
    
    // Write directly using a key. No offsets needed.
    prefs.putString("hostname", truncated);
    prefs.end();
}

String get_hostname() {
    prefs.begin("TME69", true); // true = read-only
    
    // Read the key. If it doesn't exist, return empty string
    String stored_id = prefs.getString("hostname", "");
    prefs.end();
    
    if (stored_id.length() > 0) {
        return stored_id;
    }

    // Fallback to eFuse MAC
    uint64_t chip_mac = ESP.getEfuseMac();
    char default_id[13];
    snprintf(default_id, sizeof(default_id), "%012llX", (unsigned long long)chip_mac);
    return String(default_id);
}

void set_nvram_brightness(const uint8_t brightness) {
    prefs.begin("TME69", false); // Open namespace, false = read/write
    
    prefs.putUChar("brightness", brightness);
    prefs.end();
}

uint8_t get_nvram_brightness() {
    prefs.begin("TME69", true); // true = read-only
    
    // Read the key. The second argument (64) is the default value 
    // returned if the key does not exist yet (e.g., first boot).
    uint8_t brightness = prefs.getUChar("brightness", 64);
    
    prefs.end();
    
    return brightness;
}


// ###########################################################################################
// End of EEPROM, Wifi/OTA Start
// ###########################################################################################

#if 0
How do you find your device after it boots?

sauron:~# avahi-browse -rt _arduino._tcp
+  wlan0 IPv4 P1.825-64BY64                                 _arduino._tcp        local
+  wlan0 IPv4 Marc NFL-3                                    _arduino._tcp        local
+  wlan0 IPv4 P4-64BY32X2                                   _arduino._tcp        local
+  wlan0 IPv4 Marc NFL                                      _arduino._tcp        local
=  wlan0 IPv4 P1.825-64BY64                                 _arduino._tcp        local
   hostname = [P1\.825-64BY64.local]
   address = [192.168.205.187]
   port = [3232]
   txt = ["board=adafruit_matrixportal_esp32s3" "tcp_check=no" "ssh_upload=no" "auth_upload=no"]
=  wlan0 IPv4 Marc NFL-3                                    _arduino._tcp        local
   hostname = [Marc\032NFL-3.local]
   address = [192.168.205.194]
   port = [3232]
   txt = ["board=waveshare_esp32_s3_zero" "tcp_check=no" "ssh_upload=no" "auth_upload=no"]
=  wlan0 IPv4 P4-64BY32X2                                   _arduino._tcp        local
   hostname = [P4-64BY32X2.local]
   address = [192.168.205.203]
   port = [3232]
   txt = ["board=adafruit_matrixportal_esp32s3" "tcp_check=no" "ssh_upload=no" "auth_upload=no"]
=  wlan0 IPv4 Marc NFL                                      _arduino._tcp        local
   hostname = [Marc\032NFL.local]
   address = [192.168.205.218]
   port = [3232]
   txt = ["board=waveshare_esp32_s3_zero" "tcp_check=no" "ssh_upload=no" "auth_upload=no"]
#endif

#include "wifi_secrets.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>

// you define WIFI_HOSTNAME before doing #include "wifi_serial_ota.h"
// Also define WIFI_SSID and WIFI_PASSWORD before you include
// However, this is now only left in case you want to rip out EEPROM support
// as hostname is stored in EEPROM so it can be changed at runtime on each device
#ifndef WIFI_HOSTNAME
#define WIFI_HOSTNAME "CHANGEME WIFI_HOSTNAME"
#endif

#ifndef WIFI_OTA_PWD
#define WIFI_OTA_PWD` "CHANGEME"
#endif

#ifndef TELNET_PORT
#define TELNET_PORT 23
#endif


template <typename T>
class GenericSerialSplitter : public Stream {
private:
  T* hw;
  WiFiServer* server;
  WiFiClient client;
  bool serverStarted = false; 
  NetworkUDP udpServer;
  bool udpStarted = false;
  uint16_t udpPort;
  
  // Cooldown timer to prevent UDP polling from starving the network stack
  unsigned long lastUdpCheck = 0;
  const unsigned long UDP_COOLDOWN_MS = 100; // 10Hz limit

public:
  GenericSerialSplitter(T& hardwareSerial, uint16_t port = 23) {
    hw = &hardwareSerial;
    server = new WiFiServer(port);
    udpPort = port;
  }

  // Pass the initialization through to the underlying serial interface
  void begin(unsigned long baud) {
    hw->begin(baud);
  }

    // Boolean operator so "if (Serial)" and "while (!Serial)" evaluate cleanly
  operator bool() const {
    return hw ? (bool)(*hw) : false;
  }

  void restart() {
    Serial.println("Inside SerialSplitter.restart()");
    if (serverStarted) {
        server->end();
        serverStarted = false;
        Serial.println("SerialSplitter.restart(): end Splitter");
    }
    if (client) {
        client.stop();
        Serial.println("SerialSplitter.restart(): stop client");
    }
    if (udpStarted) {
        udpServer.stop(); 
        udpStarted = false;
    }
  }

  void handle() {
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA) {
        return; 
    }

    if (!serverStarted) {
        Serial.println("SerialSplitter: (re)start");
        server->begin();
        serverStarted = true;
    }

    // Enabling this in the past caused it to eat UDP broadcast from the master
    // hopefully UDP_COOLDOWN_MS will fix this and allow enabling safely
    if (!udpStarted) {
        Serial.println("SerialSplitter: (re)start UDP");
        if (udpServer.begin(udpPort)) {
            udpStarted = true;
        }
    }

    if (server->hasClient()) {
      if (!client || !client.connected()) {
        if (client) client.stop();
        // Fixed Core 3.x deprecation: converted available() to accept()
        client = server->accept(); 
        client.println("\n--- Connected to ESP32 Serial Splitter ---");
      } else {
        // Fixed Core 3.x deprecation: converted available() to accept()
        WiFiClient reject = server->accept();
        reject.stop(); 
      }
    }
  }

  // --- STREAM/PRINT OUTPUT OVERRIDES ---
  
  size_t write(uint8_t c) override {
    size_t n = hw->write(c);         
    if (client && client.connected()) {
      client.write(c);               
    }
    return n;
  }

  size_t write(const uint8_t *buffer, size_t size) override {
    size_t n = hw->write(buffer, size);
    if (client && client.connected()) {
      client.write(buffer, size);
    }
    return n;
  }

  // --- STREAM INPUT OVERRIDES (TWO-WAY COMMS) ---

  int available() override { 
    int total = hw->available();
    if (client) {
      total += client.available();
    }
    if (udpStarted) {
        // RATE LIMITING: Only check for new UDP packets every 100ms
        if (millis() - lastUdpCheck > UDP_COOLDOWN_MS) {
            lastUdpCheck = millis();
            if (udpServer.available() == 0) {
                udpServer.parsePacket();
            }
        }
        total += udpServer.available();
    }
    return total;
  }

  int read() override { 
    if (client && client.available()) {
      return client.read();
    }
    if (udpStarted && udpServer.available()) {
      return udpServer.read();
    }
    return hw->read(); 
  }

  int peek() override { 
    if (client && client.available()) {
      return client.peek();
    }
    if (udpStarted && udpServer.available()) {
      return udpServer.peek();
    }
    return hw->peek(); 
  }

  void flush() override { 
    hw->flush(); 
    // Fixed Core 3.x deprecation: converted flush() to clear()
    if(client) client.clear(); 
  }
};

// 2. Instantiate our splitter using decltype(Serial) to automatically match the architecture
GenericSerialSplitter<decltype(Serial)> Splitter(Serial, TELNET_PORT);

// 3. THE HIJACK: Replace the word "Serial" with our Splitter for the rest of this file
#undef Serial
#define Serial Splitter


// ###########################################################################################
// End of Serial hijack, WIFI start
// ###########################################################################################

volatile bool ota_in_progress = false;

void wifi_loop() {
  ArduinoOTA.handle();
  if (ota_in_progress) return;

  Splitter.handle();
}

// we need a callback because it calls the matrix we can't access here
void wifi_setup(void (*ota_start_callback)() = nullptr, void (*ota_wifi_connect_callback)() = nullptr) {
  Serial.begin(115200);
  //while (!Serial) {}
  Serial.println("Start");

  // Wi-Fi Event Callbacks
  WiFi.onEvent([ota_wifi_connect_callback](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.printf("\n[WiFi] Connected! IP: %s | Hostname: %s.local | Telnet ready port %d (use nc, not telnet)\n", 
                  WiFi.localIP().toString().c_str(), 
                  get_hostname().c_str(), TELNET_PORT);
		  ota_wifi_connect_callback();
  }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    uint8_t reason = info.wifi_sta_disconnected.reason;
    Serial.printf("\n[WiFi] Disconnected. Reason: %d\n", reason);
    
    // Force a reconnect if the driver gives up (e.g., AUTH_FAIL or NO_AP_FOUND)
    if (reason == 202 || reason == 201 || reason == 4) {
        Serial.println("[WiFi] Forcing manual reconnect...");
        // Use WiFi.begin rather than WiFi.reconnect() to fully reset the station state
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  Serial.print("Wifi: ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // I think this is default now?
  WiFi.setAutoReconnect(true);

  ArduinoOTA.onStart([ota_start_callback]() {
    Serial.println("\r\n[OTA] Update started. Freeing network resources and locking loop.");
    ota_in_progress = true;
    
    // 1. Disable Wi-Fi sleep to prevent TCP dropped packets during flash erase CPU stalls
    WiFi.setSleep(false);
    
    // 2. Kill the Telnet server and all clients to free LwIP TCP memory buffers
    // But let's not doo that, it's nice to be able to watch the OTA being received over serial
    //Splitter.restart();

    // Execute the injected matrix code from the main file
    if (ota_start_callback) ota_start_callback();
  });

  ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("[OTA] Error[%u] - Transfer failed. Releasing loop lock.\r\n", error);
      ota_in_progress = false; 
      //WiFi.setSleep(true); // Restore default Wi-Fi power save
  });

  ArduinoOTA.onEnd([]() {
      // kill the telnet connection since we're about to reboot
      Splitter.restart();
      Serial.println("\r\n[OTA] Transfer Complete.");
      ota_in_progress = false;
  });

  Serial.println("Setup ArduinoOTA. Remote Wifi Updates Supported!");
  //ArduinoOTA.setHostname(WIFI_HOSTNAME);
  ArduinoOTA.setHostname(get_hostname().c_str());
  ArduinoOTA.setPassword(WIFI_OTA_PWD);
  ArduinoOTA.begin();
  Serial.printf(">>>>>>>>>>>>>>>>>> OTA Ready (%s/%s)\n", get_hostname().c_str(), WIFI_OTA_PWD);
}

// ###########################################################################################
// End of WIFI
// ###########################################################################################

void(* rebootESP32) (void) = 0; // jump to 0 to cause a sofware reboot

#endif // WIFI_SERIAL_OTA_H
