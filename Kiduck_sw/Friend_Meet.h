#define DECODE_NEC 
#include <IRremote.h>

#ifndef FRNDMEET

#define MAX_FRIEND 20   //최대로 만날 수 있는 친구
#define SEND_PIN 3      //송신기 3번 핀
#define RECV_PIN 10     // 수신기 10번 핀

int met_friend[MAX_FRIEND]; //만났던 친구 저장하는 배열
int today_met_count = 0; //지금까지 만난 친구

void Irsetup(){
  IrSender.begin(SEND_PIN, ENABLE_LED_FEEDBACK); // 송신기 작동 시작
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); // 수신기 작동 시작
  Serial.println("Start");
}

//comand 및 address 정의
#define S_ADDRESS 0x0002

#define COM_CMD     0x01
#define SUCC_CMD    0x02
#define DISCON_CMD  0x03
#define C_ACK_CMD   0x04
#define S_ACK_CMD   0x05
#define D_ACK_CMD   0x06
#define NACK        0x07

//적외선 센서 송신 함수
void send_ir_data(uint8_t Command, uint8_t Repeats) {
    // clip repeats at 4
    Repeats = (Repeats>4)?4:Repeats;
    IrSender.sendNEC(S_ADDRESS, Command, Repeats);
}

void looped_send_ir_data(uint8_t Command, uint8_t Repeats, int loop, int dt){
    for(int a=0;a<loop;a++){
        send_ir_data(Command, Repeats); 
        delay(dt);
    }
}

int recvIr(bool startSend, bool con_cmd, uint8_t sendCom, int sendNum, bool check_new, uint8_t cmd1, uint8_t cmd2){
    //수신 시작
    IrReceiver.resume();
    for(int i = startSend?1:0;;i++){
        delay(500);
        //connect command 다시 전송
        if(i%20==0 && con_cmd){
            Serial.println("over");
            send_ir_data(sendCom, sendNum);
        }
        //적외선 신호 수신
        if(!IrReceiver.decode()) continue;

        if(check_new?(IrReceiver.decodedIRData.address!=S_ADDRESS)
                    :(IrReceiver.decodedIRData.address==met_friend[today_met_count-1])){
            if(IrReceiver.decodedIRData.command == cmd1) return cmd1;
            else if(IrReceiver.decodedIRData.command == cmd2) return cmd2;
        }
        //내가 받아야할 신호가 아니면 다시 수신
        IrReceiver.resume();
    }
}

bool alreadyMet(){
    int recv_cmd = IrReceiver.decodedIRData.address;
    for (int j = 0; j < MAX_FRIEND; j++) {
        if(recv_cmd == met_friend[j]) return true;
    }
    return false;
}

#define SEND_MODE 0
#define RECV_MODE 1

int connectComm(int mode){
    switch(mode){
        case SEND_MODE: //1. Connect command 전송
            if(recvIr(true, true, COM_CMD, 2, true, C_ACK_CMD, NACK)==NACK){
                Serial.println("You already met this friend today");
                return -1;
            } break;

        case RECV_MODE: //1. Connect command 기다림
            recvIr(false, false, -1, -1, true, COM_CMD, -1);
            //이미 만났던 친구인지 확인       
            if(alreadyMet()){
                Serial.println("You already met this friend today");
                looped_send_ir_data(NACK, 2, 4, 200);
                return -1;
            }
            //Connect ack 전송 이후 delay(10000)을 기다리기 때문에 너무 오랜 시간 기다리는 것 방지, 이후 Delay(10000)
            looped_send_ir_data(C_ACK_CMD, 2, 4, 200); break;

        default: break;
    }
    met_friend[today_met_count++] = IrReceiver.decodedIRData.address; // 주소 저장 & 친구 count + 1 
    Serial.println("Connect!!");
    return 0;
}

void successComm(int mode){
    switch(mode){
        case SEND_MODE: //2. Success command 전송
            recvIr(true, true, SUCC_CMD, 2, false, S_ACK_CMD, -1); break;

        case RECV_MODE: //2. Success command 기다림
            recvIr(false, true, C_ACK_CMD, 2, false, SUCC_CMD, -1);
            send_ir_data(S_ACK_CMD, 2); break;

        default: break;
    }Serial.println("success!!");
}

void disconnectCom(int mode){
    switch(mode){
        case SEND_MODE: //3. Disconnect command 전송
            recvIr(true, true, DISCON_CMD, 2, false, D_ACK_CMD, -1);  break;
        
        case RECV_MODE: //3. Disconnect command 기다림
            recvIr(false, true, S_ACK_CMD, 1, false, DISCON_CMD, -1);
            send_ir_data(D_ACK_CMD, 4);
            looped_send_ir_data(D_ACK_CMD, 2, 5, 200); break;
        
        default: break;
    }Serial.println("disconnect!!");
}

int meet(int mode){
    //delay를 주어 다음 신호 전송까지 시간 간격을 둠, 너무 빨리 통신이 이루어지는 것 방지
    if(connectComm(mode)==-1) return -1; delay(10000);
    successComm(mode); delay(1000);
    disconnectCom(mode);
    Serial.println("finish");   //친구 만나기 종료
    return 0;
}

#define FRNDMEET
#endif