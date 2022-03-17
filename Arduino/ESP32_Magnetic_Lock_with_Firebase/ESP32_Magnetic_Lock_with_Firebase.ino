//Be sure to have FIREBASE ESP32 Client installed, This is Method 1
#include <FirebaseESP32.h>
#include <WiFi.h>

#define FIREBASE_HOST "YOUR REALTIME DATABASE URL" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR DATABASE SECRET PASS"

#define WIFI_SSID "YOUR WIFI NAME"
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"


// Define Firebase Data Object
FirebaseData firebaseData;

// Root Path
String path = "/ledStatus";

const int ledPin = 2;
const int RELAY_PIN = 3;

void setup() {

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(ledPin, LOW);

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

}

void loop() {

  if (Firebase.getDouble(firebaseData, path + "/LockOff"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.print("VALUE: ");
    printResult(firebaseData);
    Serial.println("------------------------------------");
    Serial.println();

    if (firebaseData.intData() == 1)
    {
      digitalWrite(ledPin, HIGH);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Locker Open");
    }

    else
    {
      Serial.println("Locker Locked");
      }
  }

   if (Firebase.getDouble(firebaseData, path + "/LockON"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.print("VALUE: ");
    printResult(firebaseData);
    Serial.println("------------------------------------");
    Serial.println();

    if (firebaseData.intData() == 1)
    {
      digitalWrite(ledPin, LOW);
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Locker Locked");

    }
    else
    {
      Serial.println("Locker Open");
      }
  }

  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Push integer test...");

}


void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else
  {
    Serial.println(data.payload());
  }
}
