const int pwmPin = 9;  // Predefined PWM output pin
const int directionPin = 10;  // Predefined pin for controlling direction
String buffer = ""; // String to store incoming data

void setup() {
  pinMode(pwmPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  if (Serial.available() > 0) {
    char incomingChar = Serial.read(); // Read incoming character from serial buffer
    
    if (incomingChar == ';') {
      // End of data sequence reached, process the data
      processBuffer();
      buffer = ""; // Clear buffer after processing
    } else {
      // Append incoming character to buffer
      buffer += incomingChar;
    }
  }
}

void processBuffer() {
  // Extract last two characters separated by ';'
  int separatorIndex = buffer.lastIndexOf(';');
  if (separatorIndex != -1 && separatorIndex < buffer.length() - 1) {
    // Extract last two characters
    String lastTwoChars = buffer.substring(separatorIndex + 1);
    
    // Ensure buffer has at least two characters
    if (lastTwoChars.length() >= 2) {
      // Call pwmFromChar for the two characters
      pwmFromChar(lastTwoChars.charAt(0));
      pwmFromChar(lastTwoChars.charAt(1));
    }
  }
}

void pwmFromChar(char inputValue) {
  // Calculate absolute value of input
  int absValue = abs(inputValue);
  
  // Determine direction
  if (inputValue < 0) {
    // Set direction pin HIGH for negative direction
    digitalWrite(directionPin, HIGH);
  } else {
    // Set direction pin LOW for positive direction
    digitalWrite(directionPin, LOW);
  }

  // Convert absolute char value to PWM value (0 to 100 mapped to 0 to 255)
  int pwmValue = map(absValue, 0, 100, 0, 255);
  
  // Output PWM value
  analogWrite(pwmPin, pwmValue);
}
