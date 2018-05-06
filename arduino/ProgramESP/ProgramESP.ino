void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("AT+UART_DEF=38400,8,1,0,0");
}

void loop() {
  // put your main code here, to run repeatedly:

}
