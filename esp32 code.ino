#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

const char* WIFI_SSID = "Maniox_2.4";
const char* WIFI_PASS = "#Hackmenow8*$";
 
WebServer server(80);

const int LED_FLASH_PIN = 4;  // GPIO pin for the built-in LED flash
 
static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);

void serveJpg()
{
  // Turn on the LED flash
  //digitalWrite(LED_FLASH_PIN, LOW);

  // Capture the image
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");

    // Turn off the LED flash (in case of failure)
    //digitalWrite(LED_FLASH_PIN, HIGH);
    return;
  }

  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  // Send the captured image to the client
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);

  // Turn off the LED flash after sending the image
  digitalWrite(LED_FLASH_PIN, LOW);
}
 
void handleJpgLo()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}
 
void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}
 
void handleJpgMid()
{
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}
 
void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize the LED flash pin
  pinMode(LED_FLASH_PIN, OUTPUT);
  //digitalWrite(LED_FLASH_PIN, HIGH);  // Ensure the LED is off initially

  // Camera configuration
  using namespace esp32cam;
  Config cfg;
  cfg.setPins(pins::AiThinker);
  cfg.setResolution(hiRes);
  cfg.setBufferCount(2);
  cfg.setJpeg(80);

  bool ok = Camera.begin(cfg);
  Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");

  // Connect to Wi-Fi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    digitalWrite(LED_FLASH_PIN, LOW);
  }
  
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");

  // Define routes for different resolutions
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid.jpg", handleJpgMid);

  server.begin();
}
 
void loop() {
  // Handle client requests
  server.handleClient();
  
}
