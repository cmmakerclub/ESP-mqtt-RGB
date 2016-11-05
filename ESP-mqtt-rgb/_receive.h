PubSubClient::callback_t on_message_arrived =
[&](const MQTT::Publish & pub) -> void {
  String topic = pub.topic();
  String payload = pub.payload_string();
  String text = topic + " => " + payload;

  Serial.println(text);

  if (payload == "r" || payload == "R")  {
    statusRGB = true;
  } else {
    statusRGB = false;

  }

  if (statusRGB == true) {
    rainbow(20);
  } else if (statusRGB == false) {
    pixels.setPixelColor(0, pixels.Color(128, 128, 128));
    pixels.show();
  }
};




