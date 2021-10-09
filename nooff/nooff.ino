// get the input val (is on or is off)
const uint8_t c_in  = 18; 
// will give the input value the current
const uint8_t c_out = 19; 

int credit_conut50 = 0; 

bool on_off;  


void IRAM_ATTR coinINPUT(){
  //Serial.print("We got an new Coin");
  //credit_conut50++;
  on_off = true; 
  Serial.println("Hallo"); 
  //Serial.println("Credit balance: " + String(credit, 2));
}

void power_off(){
  if(on_off){
    on_off = false; 
  }
}





void setup(){
  // Serial
  Serial.begin(9600);
  
  // init bought
  pinMode(c_in, INPUT); 
  pinMode(c_out, OUTPUT); 
  
  attachInterrupt(digitalPinToInterrupt(c_in), coinINPUT, RISING); // fot interrupt_Mode


  
}

void loop(){
 
  
  //c_state(c_in, c_out); 

  Serial.println(on_off); 

  if(on_off){
    digitalWrite(c_out, HIGH); 
  }else{
    digitalWrite(c_out, LOW); 
  }
  power_off();
  delay(1000);
   
  
}





bool c_state(uint8_t in, uint8_t out){
    if(digitalRead(in)){
      Serial.println("Hallo es liegt nicht an"); 
      digitalWrite(out, LOW); 
      return true; 
    }else{
      Serial.println("Hallo es liegt etwas an"); 
      digitalWrite(out, HIGH);
      return false;
    }
  }
   
