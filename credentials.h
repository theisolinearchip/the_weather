// NETWORK STUFF
// the network you're connecting to DO STUFF
#define NETWORK_NAME    "Network Name"
#define NETWORK_PASS    "network-password"

// OPENWEATHER CUSTOM SERVICE CONST
// the remote script to fetch data (maybe an openweather direct API call or maybe an external script to prepare your data)
static const char* openweather_service_host = "example.com";
static const int openweather_service_host_size = 11; // size of openweather_service_host
static const char* openweather_service_endpoint = "/script/script_name";
static const int openweather_service_endpoint_size = 19; // size of openweather_service_endpoint
static const char* openweather_service_secret = "xxxxxxxxxx"; // some secret shared between the remote script and the board
static const int openweather_service_secret_size = 10; // size of openweather_service_secret
