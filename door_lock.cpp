// LCD1602 to Arduino Uno connection example
//initially servo is at 90 degree
#include <EEPROM.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#define entrance_servo 11
#define main_gate_servo 10
Servo servo1;
Servo servo2;

unsigned char val=EEPROM[1000];
unsigned char lock_status=EEPROM[600];
static unsigned char copy_password[34];
static int i=0; // indexing for the password entered
bool lock_bit=1;
#define c1 5
#define c2 6
#define c3 7
#define r1 2
#define r2 3
#define r3 4
#define enter 8
#define erase 9
#define reset_password A0
#define lock A1
bool col1,col2,col3;
bool read_status;
unsigned char password_check(unsigned char *);
void locked(void);
void change_password(void);
void new_password(void);
void col_check(bool ,unsigned char,unsigned char,unsigned char,unsigned char);
void write_eeprom(bool ,unsigned char,unsigned char,unsigned char,unsigned char);
void password_correct(void);
void password_incorrect(void);
void password_not_entered(void);

LiquidCrystal lcd(13,12,A2,A3,A4,A5);
//***********************************************SETUP_FUNCTION_START***********************************************************//
void setup() {
  servo1.attach(entrance_servo);
  servo2.attach(main_gate_servo);
  lcd.begin(16, 2);
  Serial.begin(9600);
    pinMode(c1,INPUT_PULLUP);
    pinMode(c2,INPUT_PULLUP);
    pinMode(c3,INPUT_PULLUP);
    pinMode(enter, INPUT_PULLUP);
    pinMode(erase, INPUT_PULLUP);
    pinMode(lock, INPUT_PULLUP);
    pinMode(reset_password, INPUT_PULLUP);
    pinMode(r3,OUTPUT);
    pinMode(r2,OUTPUT);
    pinMode(r1,OUTPUT);
}
//***********************************************SETUP_FUNCTION_END***********************************************************//

//***********************************************MAIN_PROGRAM_START***********************************************************//
void loop() {
  servo1.write(0);
  //delay(1000);// need to add in real hardware implementation

digitalWrite(r3,LOW);
digitalWrite(r2,LOW);
digitalWrite(r1,LOW);
col1=digitalRead(c1);
col2=digitalRead(c2);
col3=digitalRead(c3);
//FOR STORING THE DEFAULT PASSWORD IN EEPROM DURING FIRST TIME BOOT WHEN DEFAULT VALUES OF EEPROM IS 255
	//Excute only one time
if(val==255){// when we are turing on first time
  EEPROM.put(1000,0); //update the value so next time it wont enter
  static unsigned char password[34]="1111";
  static unsigned char* copy=password;
   int i=0;
   while(*(copy)!='\0'){
     EEPROM[i]=*(copy);
     copy++;
     i++;
   }
      EEPROM[i]='\0';     
  
  val=EEPROM.get(1000,val); // update the value so next time it wont enter
  
  }
//-----------------------------------------------------------------------------------------------------
	//Excute only one time
if(lock_status==255){//when we turn on first time
  lcd.print("  __LOCKED__ ");
  EEPROM.put(600,0); // update the value so next time it wont enter
  lock_status=EEPROM.get(600,lock_status);
  read_status= digitalRead(erase);
  // STUCK IN LOOP UNTIL WE PRESS THE ERASE BUTTON
  while(read_status==1){
   read_status= digitalRead(erase);
  }
  lcd.clear();
}
	
//-----------------------------------------------------------------------------------------------------
//lock_bit is used to enter into this if condition only once 
//because we are running in void loop function which will excute continously
// Door is in locked state and power off to on state	
if(EEPROM[600]==0 && lock_bit){// when we turn on second time(power off and on time) in hardware time
       lcd.print("  __LOCKED__ ");
         read_status= digitalRead(erase);
  while(read_status==1){
   read_status= digitalRead(erase);
  }
  lcd.clear();
  lock_bit=0; // update the value so next time it wont enter
}
//----------------------------------------------------------------------------------------------------	
//WHEN THE ENTERED PASSWORD IS CORRECT AND WHEN THE DOOR IS IN OPEN STATE DURING THE POWER OFF TIME
if(EEPROM[600]==1){ // this value is updated by password_correct() function
     if(EEPROM[600]==1) { //door opened status
            servo1.write(90);
            delay(1000);// need to add in real hardware implementation
       lcd.clear();
      lcd.print("    OPENED   ");
      
      lcd.setCursor(0,1);
      lcd.print("****WELCOME****");
      read_status=digitalRead(lock);
      while(read_status==1){
           read_status=digitalRead(lock);
      }
              read_status=digitalRead(lock);
              delay(2000);
               read_status=digitalRead(lock);
            locked();
          
     }
}
//-----------------------------------------------------------------------------------------------------	
//WHEN WE WANT TO CHANGE THE PASSWORD
  read_status=digitalRead(reset_password);
  if(read_status==0){
        change_password();
  }
//-----------------------------------------------------------------------------------------------------	
//WHEN WE PRESS THE ENTER BUTTON
read_status=digitalRead(enter);
    if(read_status==0){
        copy_password[i]='\0';
      unsigned char a = password_check(copy_password);
        if(a==2){
           password_not_entered();
        }
        else if(a==1){
            password_correct();
        }
        else if(a==0){
            password_incorrect();
        }

    }
//----------------------------------------------------------------------------------------------------	
   //WHEN WE WANT TO SUDDENLY ERASE THE ENTERED PASSWORD	
    read_status=digitalRead(erase);
    if(read_status==0){
      lcd.clear();
      i=0; //reset the index
    }
//----------------------------------------------------------------------------------------------------	
	// PASSWORD ENTERING MECHANISM
if(col1==0)
    col_check(col1,c1,'1','4','7');
    else if(col2==0)
    col_check(col2,c2,'2','5','8');
    else if(col3==0)
    col_check(col3,c3,'3','6','9');  
     read_status=digitalRead(erase);
    if(read_status==0){
      lcd.clear();
      i=0;
    }
}
//***********************************************MAIN_PROGRAM_END***********************************************************//



//**********************************************FUNCTIONS_DEFINATIONS**********************************************************//
void col_check(bool x,unsigned char y,unsigned char a,unsigned char b,unsigned char c){
digitalWrite(r3,HIGH);
digitalWrite(r2,HIGH);
digitalWrite(r1,HIGH);
digitalWrite(r1,LOW);
x=digitalRead(y);
if(x==0)

   {    copy_password[i]=a;
        i++;
        a=a-'0'; // for converting Ascii value to its original value
      lcd.print(a);
      
       
   while(x==0){
        x=digitalRead(y);
   }

   }
digitalWrite(r1,HIGH); 
digitalWrite(r2,LOW);
x=digitalRead(y);
if(x==0)
{ copy_password[i]=b;
        i++;
    b=b-'0';
   lcd.print(b);
   
while(x==0){
        x=digitalRead(y);
   }

}

digitalWrite(r2,HIGH);
digitalWrite(r3,LOW);
x=digitalRead(y);
if(x==0)
{   copy_password[i]=c;
        i++;   
    c=c-'0';
       lcd.print(c); 

      
    while(x==0){
        x=digitalRead(y);
   }

}
digitalWrite(r3,HIGH);
}
//**********************************************************************************
unsigned char password_check(unsigned char *a){
    unsigned char i=0;

                if(*a=='\0')
                  return 2;
                  while((*a)!='\0' && EEPROM.read(i)!='\0'){
						if((*a)!=EEPROM.read(i))
						return 0;
							a++;
							i++;
		
	}
	if(*(a)==EEPROM[i])
	return 1;
  else
  return 0;

}
//*******************************************************************************
void password_correct(void){
  lcd.clear();
          lcd.print("     ACCESS  ");
           lcd.setCursor(0,1);
          lcd.print("  ***GRANTED*** ");
          EEPROM[600]=1; // update the eeprom value for password correct
          delay(1000);
          
         
       
          //motor running
   /* read_status=digitalRead(erase);
    while(read_status==1){
      read_status=digitalRead(erase);
    }
*/

}
//***************************************************************************
void password_incorrect(void){
   lcd.clear();
          lcd.print("     ACCESS  ");
           lcd.setCursor(0,1);
          lcd.print("   !!DENIED!! ");
           read_status=digitalRead(erase);
            while(read_status==1){
      read_status=digitalRead(erase);
    }


}
//**************************************************************************
void password_not_entered(void){
          lcd.clear();
          lcd.print("  PLEASE  ");
          lcd.setCursor(0,1);
          lcd.print("ENTER PASSWORD");
           read_status=digitalRead(erase);
 while(read_status==1){
      read_status=digitalRead(erase);
    }


}
//***************************************************************************
void change_password(void){
  i=0; //RESETTING THE INDEX
  lcd.clear();
  lcd.print("ENTER OLD PASS:");
  delay(1500);// used for waiting to reduce the errors
  lcd.setCursor(0,1);
  
  read_status=digitalRead(reset_password);
	//*********************PASSWORD RESETTING MODE**********************//
  while(read_status==1){
	  
    
   digitalWrite(r3,LOW);
digitalWrite(r2,LOW);
digitalWrite(r1,LOW);
col1=digitalRead(c1);
col2=digitalRead(c2);
col3=digitalRead(c3);
if(col1==0)
    col_check(col1,c1,'1','4','7');
    else if(col2==0)
    col_check(col2,c2,'2','5','8');
    else if(col3==0)
    col_check(col3,c3,'3','6','9');
    //AFTER ENTERING THE PASSWORD
          read_status=digitalRead(enter);
          if(read_status==0){ // WHEN WE PRESS ENTER BUTTON
             copy_password[i]='\0';
  unsigned char a = password_check(copy_password); //check the password
                  if(a==1){
                    new_password(); //if the entered password is correct 
                    return;
                  }
                  else if(a==2){
                    password_not_entered();
                
  lcd.clear();
  lcd.print("ENTER OLD PASS:");
  lcd.setCursor(0,1);
  i=0;
			  
                  }
                  else if(a==0){
                    lcd.clear();
                    lcd.print("PASSWORD INVALID");
                
                    read_status=digitalRead(erase);//checking for erase pin
  while(read_status==1){
     read_status=digitalRead(erase);
  }
    lcd.clear();
    lcd.print("ENTER OLD PASS:");
    lcd.setCursor(0,1);
    i=0;
                  }
		  
          }

    read_status=digitalRead(reset_password);


	  
  }
	//*********************PASSWORD RESETTING MODE END**********************//

	
  lcd.clear();
  lcd.print("EXIT FROM");
  lcd.setCursor(0,1);
  lcd.print("RESET MODE");
  read_status=digitalRead(erase);
  while(read_status==1){
     read_status=digitalRead(erase);
  }
   i=0;   
  lcd.clear();
return ;
}
//*****FOR UPDATING THE NEW PASSWORD
void new_password(void){
i=0;
lcd.clear();
lcd.print("NEW PASSWORD:");
delay(1000);
lcd.setCursor(0,1);
read_status=digitalRead(enter);
while(read_status==1){
 digitalWrite(r3,LOW);
digitalWrite(r2,LOW);
digitalWrite(r1,LOW);
col1=digitalRead(c1);
col2=digitalRead(c2);
col3=digitalRead(c3);
if(col1==0)
    write_eeprom(col1,c1,'1','4','7');
    else if(col2==0)
    write_eeprom(col2,c2,'2','5','8');
    else if(col3==0)
    write_eeprom(col3,c3,'3','6','9');
   
    read_status=digitalRead(enter);
    
    }
    EEPROM[i]='\0';
    lcd.clear();
    lcd.print("PASSWORD SAVED");
    lcd.setCursor(0,1);
    lcd.print("  SUCESSFULLY ");
    delay(1000);
    return ;

}
//****************************************************************************************
void write_eeprom(bool x,unsigned char y,unsigned char a,unsigned char b,unsigned char c){
digitalWrite(r3,HIGH);
digitalWrite(r2,HIGH);
digitalWrite(r1,HIGH);
digitalWrite(r1,LOW);
x=digitalRead(y);
if(x==0)

   {    EEPROM[i]=a;
        i++;
        a=a-'0'; 
      lcd.print(a);
      
       
   while(x==0){
        x=digitalRead(y);
   }

   }
digitalWrite(r1,HIGH); 
digitalWrite(r2,LOW);
x=digitalRead(y);
if(x==0)
{ EEPROM[i]=b;
        i++;
    b=b-'0';
   lcd.print(b);
   
while(x==0){
        x=digitalRead(y);
   }

}

digitalWrite(r2,HIGH);
digitalWrite(r3,LOW);
x=digitalRead(y);
if(x==0)
{   EEPROM[i]=c;
        i++;   
    c=c-'0';
       lcd.print(c); 

      
    while(x==0){
        x=digitalRead(y);
   }

}
digitalWrite(r3,HIGH);
}
//******************************************************************************
void locked(){
        servo1.write(0);
        delay(1000); //need to add in real hardware implementation
        lcd.clear();
        
        lcd.print("  __LOCKED__  ");

        EEPROM[600]=0;
        
        //motor rotate
        read_status=digitalRead(erase);
        while(read_status==1){
           read_status=digitalRead(erase); 
        }
        lcd.clear();
        i=0;
        return ;
}
//*******************************************************************************
