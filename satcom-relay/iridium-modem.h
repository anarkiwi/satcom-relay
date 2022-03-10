class IridiumModem
{
  public:
    void begin(Uart *uart,  byte wakeup_pin, byte rx_pin, byte tx_pin);
    void wakeup();
    void check();
    void sendJSON(const DynamicJsonDocument &doc);
    Uart *modemUart;
  private:
    bool wakeup_state;
    byte wakeup_pin;
};

void IridiumModem::begin(Uart *modemUart, byte wakeup_pin, byte rx_pin, byte tx_pin) {
  this->modemUart = modemUart;
  this->wakeup_state = false;
  this->modemUart->begin(57600);
  this->wakeup_pin = wakeup_pin;

  pinMode(this->wakeup_pin, OUTPUT);
  digitalWrite(this->wakeup_pin, this->wakeup_state);
  // Assign IRIDIUM_INTERFACE pins SERCOM functionality
  pinPeripheral(rx_pin, PIO_SERCOM);
  pinPeripheral(tx_pin, PIO_SERCOM);
}

void IridiumModem::wakeup() {
   this->wakeup_state = !wakeup_state;
   digitalWrite(this->wakeup_pin, this->wakeup_state);
   // Give the modem a chance to wakeup to receive the message.
   // TODO: the modem could also verify JSON to make sure it got a complete message and ask for a retry if necessary.
   delay(1000);
}

// If the Iridium Interface MCU misses the initial message because it was sleeping it will send a \n to request a resend
void IridiumModem::check() {
  bool sawNewline = false;
  while (this->modemUart->available()) {
    if (this->modemUart->read() == '\n') {
      sawNewline = true;
    }
  }
  if (sawNewline) {
    Serial.println("Received newline/resend request from Iridium Interface");
    // TODO check this before sending
    //serializeJson(doc, IridiumInterfaceSerial);
  }
}

void IridiumModem::sendJSON(const DynamicJsonDocument &doc) {
   serializeJson(doc, *(this->modemUart));
   this->modemUart->println();
}
