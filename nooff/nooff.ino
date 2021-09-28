// get the input val (is on or is off)
const uint8_t c_in  = 18; 
// will give the input value the current
const uint8_t c_out = 19; 



void setup(){
  // init bought 
  pinMode(c_in, INPUT); 
  pinMode(c_out, OUTPUT); 

  
}

void loop(){
  
  c_state(c_in, c_out); 

  delay(10);
  
}



bool c_state(uint8_t in, uint8_t out){
    if(digitalRead(in)){
      digitalWrite(out, LOW); 
      return true; 
    }else{
      digitalWrite(out, HIGH);
      return false;
    }
  }
   
