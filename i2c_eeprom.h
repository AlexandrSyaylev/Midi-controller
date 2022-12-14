
#include <avr/io.h>
#include <util/delay.h>


#define false 0
#define true 1

//#define slaveF_SCL 100000 //100 Khz
#define slaveF_SCL 400000 //400 Khz

#define slaveAddressConst 0b1010 //?????????? ????? ?????? ???????? ??????????
#define slaveAddressVar 0b000 //?????????? ????? ?????? ???????? ??????????

//??????? ??????????? ???????? ??????
#define READFLAG 1 //??????
#define WRITEFLAG 0 //??????


// TWSR values (not bits)
// (taken from avr-libc twi.h - thank you Marek Michalkiewicz)
// Master
#define TW_START                    0x08
#define TW_REP_START                0x10
// Master Transmitter
#define TW_MT_SLA_ACK               0x18
#define TW_MT_SLA_NACK              0x20
#define TW_MT_DATA_ACK              0x28
#define TW_MT_DATA_NACK             0x30
#define TW_MT_ARB_LOST              0x38
// Master Receiver
#define TW_MR_ARB_LOST              0x38
#define TW_MR_SLA_ACK               0x40
#define TW_MR_SLA_NACK              0x48
#define TW_MR_DATA_ACK              0x50
#define TW_MR_DATA_NACK             0x58
// Slave Transmitter
#define TW_ST_SLA_ACK               0xA8
#define TW_ST_ARB_LOST_SLA_ACK      0xB0
#define TW_ST_DATA_ACK              0xB8
#define TW_ST_DATA_NACK             0xC0
#define TW_ST_LAST_DATA             0xC8
// Slave Receiver
#define TW_SR_SLA_ACK               0x60
#define TW_SR_ARB_LOST_SLA_ACK      0x68
#define TW_SR_GCALL_ACK             0x70
#define TW_SR_ARB_LOST_GCALL_ACK    0x78
#define TW_SR_DATA_ACK              0x80
#define TW_SR_DATA_NACK             0x88
#define TW_SR_GCALL_DATA_ACK        0x90
#define TW_SR_GCALL_DATA_NACK       0x98
#define TW_SR_STOP                  0xA0
// Misc
#define TW_NO_INFO                  0xF8
#define TW_BUS_ERROR                0x00




void eeInit(void); //????????? ????????? TWI
uint8_t eeWriteByte(uint16_t address,uint8_t data); //?????? ????? ? ?????? ?????? EEPROM
uint8_t eeReadByte(uint16_t address); //?????? ????? ?? ?????? ?????? EEPROM



void eeInit(void)
{
    /*??????????? ????????? ???????? ?????*/
    TWBR = (F_CPU/slaveF_SCL - 16)/(2 * /* TWI_Prescaler= 4^TWPS */1);
    
/*
???? TWI ???????? ? ??????? ??????, ?? ???????? TWBR ?????? ???? ?? ????? 10. ???? ???????? TWBR ?????? 10, ?? ??????? ?????????? ???? ????? ???????????? ???????????? ??????? ?? ?????? SDA ? SCL ?? ????? ???????? ?????.
*/
    if(TWBR < 10)
        TWBR = 10;

    /*
????????? ???????????? ? ???????? ??????? ????? ??????????.
????????? ???? TWPS0 ? TWPS1 ???????? ???????, ???????????? ??? ?????, ???????? ???????????? = 1.
    */
    TWSR &= (~((1<<TWPS1)|(1<<TWPS0)));
}

uint8_t eeWriteByte(uint16_t address,uint8_t data)
{

/*****????????????? ????? ? ???????********/

    do
    {
//????????????? ???????? ?????????? ????? ? ????? ??????????
/*????? ??????? ???????? ?????? ?????????? ???????????? ?.?. ??????? ??????. ? ????????? ????? ????? SCL ? SDA ????????? ?? ??????? ??????. ??????? ?????????? (?????????? AVR ? ????? ???????), ??????? ????? ?????? ???????? ??????, ???????? ????????? ????? SDA ? ??????? ??????. ??? ? ???? ??????? ?????? ???????? ??????.*/
        
/*
?)????? ????? ?????????? TWINT (???? TWINT ???????????? ?????????? ????? ?????? ? ???? ?????????? 1) ??? ?????????? ?????? ????? ???????? ?????? 
?)???. ??? ??????? ?????
?)???. ??? ?????????? ?????? TWI
*/
        TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

//????, ???? ???? ???? ????? (????????, ????? ???? ??? ??????, ????)
//TWINT ??? ??????????????? ?????????, ???? TWI ????????? ??????? ??????? ? ??????? ??????? ?????????
        while(!(TWCR & (1<<TWINT)));

        /*????????? ??????? ???????, ? ?????? ???? TWS3-7,
        ??????? ???????? ?????? ??? ??????. ??? ???? ????? 
        ???????? ????????? ????. TWS2-0 "????????" ? ??????? ???????? "? 	0xF8". ???? TWS7-3 = 0x08, ?? ????? ??? ????????.*/
        if((TWSR & 0xF8) != TW_START)
            return false;

/*? ???? I2C ????? ???? ?????????? ????????? ??????????? ????????? (? ???????, ????? ????????? ??????? ?????? EEPROM). ??? ????, ????? ??? ?????????? ? ?????????? ?????, ?? ???? ? ???? ?????????? ??????????, ? ????????? ??????????? ????????? ??????? ?????????. ? ?????? ??????????, ??????????????? ??? ?????? ? I2C, ?? ?????? "?????" ???????????? ?????. ?? ???? ????? ???????? ?? ???? ????, ?.?. ???? ???????. ?????? ??????? ???????? ???? ????? ? ???????, ???? ??? ??? ??? ?????. ???? ???, ?? ? ?????, ?? ???? ????? ???????? ?????????? AVR. ??? ??? ? ?????????? "???????????" ????? ??????? ? ???????.*/

/*??? ???, ?? ????? ???????? ? ??????????? ?????? 24LC64, ??????? ?? ???? ??? ???? ???????? ?? ?????. ??? ?????? ???? ?????, ? ????? ?????, ??? ?????? ?? ?????? ?????????? ?????? ??. ? ????????? ??????????, ???? ??? ????, ??? ?????? ????? ?????? ????????????.*/

/*?????????? ????? ?????? 24LC64 - 1010 (??. ??????? ?? 24XX64), 3 ???? - ?????????? (???? ????? ?? ??????? ?????????? ????????? ?????????? ????????? c ??????????? ?????????? ????????, ??? ??????????; ? ????(?????) ?????? ?????????? ????), ????? ??? 0 - ???? ????? ?????????? ? ?????? ??? 1 - ???? ?????? ?????? ?? ?????? I2C EEPROM*/
        
	//TWDR = 0b1010'000'0;    
        TWDR = (slaveAddressConst<<4) + (slaveAddressVar<<1) + (WRITEFLAG);

/*??????? ???????? ??????????, ??? ?? ????? ???????? ??????, ???????????? ? ???????? ?????? TWDR*/
        TWCR=(1<<TWINT)|(1<<TWEN);

        //???? ????????? ???????? ??????
        while(!(TWCR & (1<<TWINT)));
    
/*???? ??? ????????????? ?? ????????, ?????? ??? ??-????? (???? ????????? ? ??????, ???? ???????? ? ????? ??????? ???).
???? ?? ????????????? ?????????, ?? ??????? ??????? ????????? ???? ? 0x18=TW_MT_SLA_ACK (? ?????? ??????) ??? 0x40=TW_MR_SLA_ACK (? ?????? ??????).
????? ??????, ???? TW_MT_SLA_ACK, ?? ??????? "???????" ???, ??? ??? ????? ??? ??? 1010'000 ? ?? ????? ??? ?????? (??????, ???? TW_MR_SLA_ACK).*/
    }while((TWSR & 0xF8) != TW_MT_SLA_ACK);
        
/*????? ????? ??? ???????? ????????, ??? ??????? ? ??????? ???? ????? ????? ? ????????. ??????? ?????? ????? ?????????? ??????, ?? ?????? ?????? ?? ????? ???????? ???? ??????*/
    

/*****???????? ????? ??????********/
    
/*?????????? ? ??????? ?????? ??????? ?????? ?????? (????? 16-??????, uint16_t))..*/
    TWDR=(address>>8);

    //..? ???????? ???
    TWCR=(1<<TWINT)|(1<<TWEN);

    //???? ????????? ????????
    while(!(TWCR & (1<<TWINT)));

/*????????? ??????? ???????, ?????? ?? ??????? ??????. ???? ??????? ?????? ??????, ?? ?? ???????? "?????????????", ???????????? SDA ? ?????? ???????. ???? ??????????, ? ???? ???????, ????????? ?????????????, ? ?????????? ? ??????? ??????? 0x28= TW_MT_DATA_ACK. ? ????????? ?????? 0x30= TW_MT_DATA_NACK */
    if((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return false;

    //????? ???? ????? ??? ???????? ??????? ??????
    TWDR=(address);
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return false;


/*****?????????? ???? ??????********/

    //??????????, ??? ? ?????????? ?????, ???????? ???? ??????
    TWDR=(data);
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return false;

    /*????????????? ??????? ?????????? ???????? ?????? (????)
    (????????????? ??? ??????? ????)*/
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    
    //???? ????????? ??????? ????
    while(TWCR & (1<<TWSTO));
	
	_delay_ms(10);

    return true;
}

uint8_t eeReadByte(uint16_t address)
{
    uint8_t data; //??????????, ? ??????? ??????? ??????????? ????

//????? ????? ?? ????? ????, ??? ? ? eeWriteByte...
/*****????????????? ????? ? ???????********/
    do
    {
        TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
        while(!(TWCR & (1<<TWINT)));

        if((TWSR & 0xF8) != TW_START)
            return false;

        TWDR = (slaveAddressConst<<4) + (slaveAddressVar<<1) + WRITEFLAG;        
        TWCR=(1<<TWINT)|(1<<TWEN);

        while(!(TWCR & (1<<TWINT)));
    
    }while((TWSR & 0xF8) != TW_MT_SLA_ACK);
        

/*****???????? ????? ??????********/
    TWDR=(address>>8);
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return false;

    TWDR=(address);
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return false;


/*****??????? ? ????? ??????********/
/*?????????? ????? "?????????" ? ???????, ?.?. ????? ?? ???????? ???????? ????? (slaveAddressConst<<4) + (slaveAddressVar<<1) + WRITEFLAG, ????? ???????? ????? ?????? ????? ??????. ? ?????? ????? ??????? ? ????? ?????? (?? ?? ????? ????????? ???? ??????), ??? ????? ???????? ????? ????? (slaveAddressConst<<4) + (slaveAddressVar<<1) + READFLAG.*/
    
    //?????? ??????? ?????? ????????
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
//???? ?????????? ??????? ????????
    while(!(TWCR & (1<<TWINT)));

/*????????? ??????. ??????? ??????? ?????? ???????? (0x10=TW_REP_START) ?????? ?????????????*/
    if((TWSR & 0xF8) != TW_REP_START)
        return false;

    /*?????????? ????? ???????? (7 ?????) ? ? ????? ??? ?????? (1)*/
    //TWDR=0b1010'000'1;    
    TWDR = (slaveAddressConst<<4) + (slaveAddressVar<<1) + READFLAG;        

//??????????..
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

/*?????????, ??????? ?? ??????? ? ??????? 1010'000 ? ????? ?? ?? ???????? ?? ??????*/
    if((TWSR & 0xF8) != TW_MR_SLA_ACK)
        return false;


/*****????????? ???? ??????********/

/*???????? ????? ?????? ? ??????? ??????? ????? ?????????? TWINT. ???????? ???? ???????????? ? ??????? TWDR.*/
    TWCR=(1<<TWINT)|(1<<TWEN);

    //???? ????????? ??????..
    while(!(TWCR & (1<<TWINT)));

/*????????? ??????. ?? ?????????, ????? ?????? ?????? ???????????? ??? ????????????? ?? ??????? ???????? (TW_MR_DATA_NACK = 0x58)*/
    if((TWSR & 0xF8) != TW_MR_DATA_NACK)
        return false;

    /*??????????? ?????????? data ????????, ????????? ? ??????? ?????? TWDR*/
    data=TWDR;

    /*????????????? ??????? ?????????? ???????? ?????? (????)*/
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    
    //???? ????????? ??????? ????
    while(TWCR & (1<<TWSTO));
	
	_delay_ms(10);

    //?????????? ????????? ????
    return data;
}

