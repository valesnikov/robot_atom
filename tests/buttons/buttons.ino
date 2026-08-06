constexpr uint8_t SAMPLES = 50;

void setup() {
    Serial.begin(115200);
}

void loop() {
    static uint64_t sum = 0;
    static uint32_t samples = 0;
    sum += analogRead(A2);
    samples += 1;
    if (samples > 0) {
        Serial.println((uint32_t)(sum/samples));
    }    
    delay(20);
}


/*
0 -
1 -
2 -
3 -
4 -
5 -
6 -
*/