// Red local wifi
const char * wifi_ssid = "YOUR_LOCAL_SSID";
const char * wifi_pass = "YOUR_LOCAL_PASS";

// Node.js api server
const String api_host = "YOUR_API_HOST";
const String api_port = "YOUR_API_PORT";
const String api_login = "/path/to/your/api/login";
const String api_new_sensor = "/path/to/your/api/sensor";

// user api:
const String api_user_email = "YOUR_API@EMAIL.COM";
const String api_user_pass = "YOUR_API_PASSWORD";

// dinamic
static const char * api_user_token;
static const char * sensor_saved_id;
static String response;

// Sector
const String _placa   = "Placa_A";
const String _sector  = "EJE";
const String _pin   = "2";
bool _logged = false;
long _startMills;
int _vcc;
 
// Sensores
const int deviceCount = 2;
float temp[deviceCount];
// Turorial como obtener las direcciones:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
uint8_t sr1[8] = { 0x28, 0xD8, 0xE2, 0xCE, 0x03, 0x00, 0x00, 0xEA };
uint8_t sr2[8] = { 0x28, 0x5D, 0xE8, 0xCE, 0x03, 0x00, 0x00, 0x56 };
String sr1_prefix = "E01";
String sr2_prefix = "E02";
