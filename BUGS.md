### Open bugs

### Closed bugs
src/WebHandler/ServeCategories.cpp: In static member function 'static void ServeCategories::handlePostCategories(AsyncWebServerRequest*, uint8_t*, size_t, size_t, size_t)':
src/WebHandler/ServeCategories.cpp:104:48: warning: 'bool ArduinoJson::V730PB22::JsonObject::containsKey(const TString&) const [with TString = char [3]; typename ArduinoJson::V730PB22::detail::enable_if<ArduinoJson::V730PB22::detail::IsString<TString>::value, int>::type <anonymous> = 0]' is deprecated: use obj[key].is<T>() instead [-Wdeprecated-declarations]
             if (!categoryToAdd.containsKey("id") || categoryToAdd["id"] == 0) {
                                                ^
In file included from lib/ArduinoJson/src/ArduinoJson.hpp:30,
                 from lib/ArduinoJson/src/ArduinoJson.h:9,
                 from src/WebHandler/WebHandler.h:8,
                 from src/WebHandler/ServeCategories.cpp:5:
lib/ArduinoJson/src/ArduinoJson/Object/JsonObject.hpp:168:8: note: declared here
   bool containsKey(const TString& key) const
        ^~~~~~~~~~~