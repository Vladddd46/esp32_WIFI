<h1> Working with WiFi on ESP32 </h1>
<p><b>t00</b> - add support of connect command in <b>CLI</b>. <i>connect ssid(wifi_name) password</i> - connects to wifi access point. If connection is successful, ssid and pass are wrote in
nvs (non-volatile storage). When esp32 restarts, wifi ssid and pass is getting read from nvs and esp32 tries to perform autoconnection.</p>
