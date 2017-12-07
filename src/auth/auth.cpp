#include "auth.h"
#include "file/file.h"

using namespace json11;
using namespace std;
using namespace cpr;

const string CLIENT_ID  = "f1f6e59f3f6f8ffecde29d34ad18f673";
const string AUTH_URL   = "https://argus.metis.wtf/v1/users/login/";

void Auth::authenticate() {
  string email;
  string password;

  cout << "Please enter email: ";
  cin >> email;

  password = Auth::password_input("Please enter password: ");

  Url url = Url{AUTH_URL};
  
  Payload payload = Payload{
    {"email",       email}, 
    {"password",    password},
    {"client_id",   CLIENT_ID},
    {"grant_type",  "password"}
  };

  Header headers = Header{{"Content-Type", "application/x-www-form-urlencoded"}};

  auto r = Post(url, payload, headers);

  if (r.status_code >= 400) {
    cerr << "Error [" << r.status_code << "] making request" << endl;
  } else {
    string err;
    Json json = Json::parse(r.text, err);
    
    Auth::write_credentials(json);
  }
}

string Auth::password_input(string prompt) {
  char *rawPassword = getpass(prompt.c_str());
  string str(rawPassword);
  return str;
}

void Auth::write_credentials(Json object) {
  cout << "Writing credentials..." << endl;

  struct passwd *pw   = getpwuid(getuid());
  const char *homedir = pw->pw_dir;
  string str(homedir);
  string destination = ".metis";

  cout << "HOME: " << str << endl;

  FileManager::create_path(0777, str, destination);

  Json creds = Json::object{
    {"token", object["token"]}
  };

  string creds_string = creds.dump();
  string filename     = str + "/" + destination + "/credentials.json";

  cout << "Writing to: " << filename << endl;

  FileManager::write(filename, creds_string);
}
