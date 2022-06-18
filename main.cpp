#include "mbed.h"
#include "Motor.h"
#include "TextLCD.h"

I2C i2c_lcd(I2C_SDA, I2C_SCL);

//I2C Portexpander PCF8574
TextLCD_I2C lcd(&i2c_lcd, 0x4e, TextLCD::LCD20x4); // I2C bus, PCF8574 Slaveaddress, LCD Type Original = 0x42, Usando I2C scanner = 0x4e

// DECLARANDO TODOS AS ENTRADAS DE FIM DE CURSO
InterruptIn fim_de_curso_inicial_x(D9);
InterruptIn fim_de_curso_final_x(D8);
InterruptIn fim_de_curso_inicial_y(D7);
InterruptIn fim_de_curso_final_y(D6);
InterruptIn fim_de_curso_inicial_z(D5);
InterruptIn fim_de_curso_final_z(D4);

// DECLARANDO BOTOES DE INTERAÇÃO
InterruptIn botao_ok(PB_13);
InterruptIn botao_confirma(PB_15);
InterruptIn botao_voltar(PB_14);

// BOTAO DE EMERGENCIA
InterruptIn botao_emergencia(PB_2);

//DECLARANDO JOYSTICK
AnalogIn EixoYJoyStick(A0);

// DECLARANDO BOTAO DE CONFIRMAR
InterruptIn botao_x(D13);
InterruptIn botao_y(D11);
InterruptIn botao_z(D12);

DigitalOut reset_motors(PB_1);

// DECLARANDO MOTORES
Motor motor1(PA_12);
Motor motor2(PA_11);
Motor motor3(PB_12);

// DECLARANDO ENABLES
DigitalOut enable_motor_1(PC_8);
DigitalOut enable_motor_2(PC_6);
DigitalOut enable_motor_3(PC_5);

// PIPETA
DigitalOut aciona_pipeta(PC_4);

// DECLARANDO STEP
DigitalOut gerador_step_driver(PB_11);

// VARIAVEIS DO DO REFERENCIAMENTO
int tempo = 1;
int aciona_motor_automatico = 0;

bool referenciamento = 0;
int contador = 0;
int contador_x = 0;
int contador_y = 0;
int contador_z = 0;
int valor_inicial_x = 0;
int valor_final_x = 45000;
int pos_atual_x;
int valor_inicial_y = 0;
int valor_final_y = 17000;
int valor_inicial_z = 0;
int valor_final_z = 7500;
int pos_atual_y;
int pos_atual_z;
int etapa = 2;
int finaliza_referenciamento = 0;
int conta_index;
int ultima_posicao_x;
int ultima_posicao_y;
int ultima_posicao_z;
int tamanho;
int contador_auto_x;
int contador_auto_y;
int contador_auto_z;

int y;

int lista_pos_x[10];
int lista_pos_y[10];
int lista_pos_z[10];
int lista_ml[10];

int flag_jogManual = 1;
int flag_jogAutomatico = 1;
int flag_referenciamento_x = 1;
int flag_referenciamento_y = 1;
int flag_referenciamento_z = 1;

int eixo_x_finalizado = 0;
int eixo_y_finalizado = 0;
int flag_emergencia = 0;

int execucao = 0;
int numero_de_soltas = 1;
int conta_posicoes = 0;
int mls = 0;
int quantidade_mls = 0;
int etapa_pos_setup = 100;
bool autoriza_etapa = 0;

Timer timer;
Timer debounce;

void lcd_show(int estado){
    switch (estado) {
        case 0:
            lcd.cls();
            lcd.printf("Bem vindo! \n");
            lcd.printf("Pressione 'ENTER' \n");
            lcd.printf("para iniciar o \n");
            lcd.printf("referenciamento");
            execucao = 1;
            break;
        case 1:
            lcd.cls();
            lcd.printf("Referenciando... \n");
            break;

        case 2:
            lcd.cls();
            lcd.printf("Referenciamento \n");
            lcd.printf("concluido \n");
            lcd.printf("Deseja selecionar a posicao de pega ? \n");
            break;
        case 3:
            lcd.cls();
            lcd.printf("Botao de emergencia acionado \n");
            break;
        case 4:
            lcd.cls();
            lcd.printf("X de coleta:%4d\n", pos_atual_x);
            lcd.printf("Y de coleta:%4d\n", pos_atual_y);
            lcd.printf("Z de coleta:%4d\n", pos_atual_z);
            lcd.printf("Pressione 'OK'");
            break;
        case 5:
            lcd.cls();
            lcd.printf("Qtd de pontos\n");
            lcd.printf("De despejo: %3d\n", numero_de_soltas);
            lcd.printf("Pressione 'ENTER' \n");
            lcd.printf("Para confirmar");
            break;
        case 6:
            lcd.cls();
            lcd.printf("Aperte ENTER para \n");
            lcd.printf("iniciar a selecao\n");
            lcd.printf("das posicoes de \n");
            lcd.printf("SOLTA \n");
            break;
        case 7:
            lcd.cls();
            lcd.printf("Aperte ENTER para \n");
            lcd.printf("iniciar o ciclo\n");
            lcd.printf("automatico \n");
            break;
        case 8:
            lcd.cls();
            lcd.printf("Voltando para HOME \n");
            break;
        case 9:
            lcd.cls();
            lcd.printf("Iniciando ciclo\n");
            lcd.printf("automatico \n");
            break;
        case 10:
            lcd.cls();
            lcd.printf("Qts ml para a\n");
            lcd.printf("posicao: %3d\n", mls);
            break;
    }
}

void referenciar_x(int aciona_motor){
    while(aciona_motor == 1){
        contador += 1;
        if(flag_referenciamento_x == 1){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_1 = 0;
            motor1.gira_motor_sentido_horario();
            
        } else{
            contador = 0;
            
            while(1){
                gerador_step_driver = !gerador_step_driver;
                wait_ms(tempo);
                motor1.gira_motor_sentido_antihorario();
                contador += 1;
                if(contador > 600){
                    enable_motor_1 = 1;
                    aciona_motor = 2;
                    flag_referenciamento_x = 3;
                    break;
                }   
            }
        }        
    }            
}

void referenciar_y(int aciona_motor){
    while(aciona_motor == 1){        
        contador += 1;
        if(flag_referenciamento_y == 1){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_2 = 0;
            motor2.gira_motor_sentido_antihorario();
            
        } else{
            contador = 0;
            
            while(1){
                gerador_step_driver = !gerador_step_driver;
                wait_ms(tempo);
                motor2.gira_motor_sentido_horario();
                contador += 1;
                if(contador > 400){
                    enable_motor_2 = 1;
                    aciona_motor = 3;
                    flag_referenciamento_y = 2;
                    break;
                }   
                }
            }
            
                
    } 
}

void referenciar_z(int aciona_motor){
    enable_motor_3 = 1;
    while(aciona_motor == 1){
        if(flag_referenciamento_z == 1){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_3 = 0;
            motor3.gira_motor_sentido_antihorario();
        } else {
            contador = 0;
            
            while(1){
                gerador_step_driver = !gerador_step_driver;
                wait_ms(tempo);
                motor3.gira_motor_sentido_horario();
                contador += 1;
                if(contador > 600){
                    enable_motor_3 = 1;
                    aciona_motor = 3;
                    flag_referenciamento_z = 2;
                    break;
                }
            } 
        }
    }
}

void valida_inicio_x(){
    if(debounce.read_ms() > 150){
        if(flag_referenciamento_x == 1){
            flag_referenciamento_x += 1;
        }
        printf("Estado flag ref inicio x: %d \n \r", flag_referenciamento_x);
    }
    debounce.reset();
}
    
void valida_fim_x(){
    if(debounce.read_ms() > 150){
        if(flag_referenciamento_x == 2){
            flag_referenciamento_x += 1;
        }
        printf("Estado flag ref fim x : %d \n \r", flag_referenciamento_x);
    }
    
    debounce.reset();
}

void valida_inicio_y(){
    if(debounce.read_ms() > 150){
        if(flag_referenciamento_y == 1){
            flag_referenciamento_y += 1;
        }
        printf("Estado flag ref inicio y: %d \n \r", flag_referenciamento_y);
    }
    debounce.reset();
}
    
void valida_fim_y(){
    if(debounce.read_ms() > 150){
        if(flag_referenciamento_y == 2){
            flag_referenciamento_y += 1;
        }
        printf("Estado flag ref fim y: %d \n \r", flag_referenciamento_y);
    }
    
    debounce.reset();
}

void valida_inicio_z(){
    if(debounce.read_ms() > 50){
        flag_referenciamento_z += 1;
        printf("Estado flag ref inicio z: %d \n \r", flag_referenciamento_z);
    }
    debounce.reset();
}
    
void valida_fim_z(){
    if(debounce.read_ms() > 50){
        flag_referenciamento_z -= 1;
        printf("Estado flag ref fim z: %d \n \r", flag_referenciamento_z);
    }
    
    debounce.reset();
}

void muda_eixo_x(){
    if(debounce.read_ms() > 150){
        flag_jogManual = 1;
        printf("Estado flag jog: %d \n \r", flag_jogManual);
    }
    debounce.reset();
}

void muda_eixo_y(){
    if(debounce.read_ms() > 150){
        flag_jogManual = 2;
        printf("Estado flag jog: %d \n \r", flag_jogManual);
    }
    debounce.reset();
}

void muda_eixo_z(){
    if(debounce.read_ms() > 150){
        flag_jogManual = 3;
        printf("Estado flag jog: %d \n \r", flag_jogManual);
    }
    debounce.reset();
}

int ml_de_despejo(){
    lcd.cls();
    lcd_show(10);
    mls = 1;
    autoriza_etapa = 0;
    while(1){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 1);
            lcd.printf("para a posicao: %3d\n", mls);
            while(y < 300){
                wait(0.3);
                y = EixoYJoyStick.read() * 1000;
                    mls += 1;
                    printf("Mls: %d \n \r", mls);
            }
                
            while(y > 600){
                wait(0.3);
                y = EixoYJoyStick.read() * 1000;
                if(mls < 0){
                    mls = 0;
                } else {
                    mls -= 1;
                    printf("Mls: %d \n \r", mls);
                }
                
                
            }

            if(autoriza_etapa == 1){
                return mls;
                break;
            }
            
    }   
}

void jogManual(int index){
    conta_index = 0;
    ultima_posicao_x = 0;
    ultima_posicao_y = 0;
    contador_x = 0;
    contador_y = 0;
    contador_z = 0;

    if(execucao >= 7){
        conta_index = 1;
        lcd.cls();
        lcd.printf("Agora selecione a \n");
        lcd.printf("posicao %3d \n", conta_index);
        wait(2);
    }

    if(execucao >= 7){
        while(conta_index <= index){
        
        lcd_show(4);
        
        // EIXO X
        contador_x = pos_atual_x;
        while(flag_jogManual == 1){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 0);
            lcd.printf("X de coleta:%4d\n", pos_atual_x);
            while(y > 600 && flag_jogManual == 1){
                y = EixoYJoyStick.read() * 1000;
                if(contador_x >= valor_final_x){
                    enable_motor_1 = 1;
                    contador_x = valor_final_x;
                    // printf("Posicao X = %4d \n \r", contador_x);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_1 = 0;
                    motor1.gira_motor_sentido_antihorario();
                    contador_x += 1;
                    // printf("Posicao X = %4d \n \r", contador_x); 
                }
                
            }
            while(y < 300 && flag_jogManual == 1){
                y = EixoYJoyStick.read() * 1000;
                if(contador_x <= valor_inicial_x){
                    enable_motor_1 = 1;
                    contador_x = valor_inicial_x;
                    // printf("Posicao X = %4d \n \r", contador_x);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_1 = 0;
                    motor1.gira_motor_sentido_horario();
                    contador_x -= 1;
                    // printf("Posicao X = %4d \n \r", contador_x);
                }
            }
            pos_atual_x = contador_x;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
        }

        // EIXO Y
        contador_y = pos_atual_y;
        while(flag_jogManual == 2){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 1);
            lcd.printf("Y de coleta:%4d\n", pos_atual_y);
            
            while(y > 600 && flag_jogManual == 2){
                y = EixoYJoyStick.read() * 1000;
                if(contador_y >= valor_final_y){
                    enable_motor_2 = 1;
                    contador_y = valor_final_y;
                    // printf("Posicao Y = %4d \n \r", contador_y);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_2 = 0;
                    motor2.gira_motor_sentido_horario();
                    contador_y += 1;
                    // printf("Posicao Y = %4d \n \r", contador_y); 
                }
                
            }
            while(y <  300 && flag_jogManual == 2){
                y = EixoYJoyStick.read() * 1000;
                if(contador_y <= valor_inicial_y){
                    enable_motor_2 = 1;
                    contador_y = valor_inicial_y;
                    // printf("Posicao Y = %4d \n \r", contador);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_2 = 0;
                    motor2.gira_motor_sentido_antihorario();
                    contador_y -= 1;
                    // printf("Posicao Y = %4d \n \r", contador);
                }
            }

            pos_atual_y = contador_y;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
        }

        contador_z = pos_atual_z;
        while(flag_jogManual == 3){
            y = EixoYJoyStick.read() * 1000;
            
            lcd.locate(0, 2);
            lcd.printf("Z de coleta:%4d\n", pos_atual_z);
            
            while(y > 600 && flag_jogManual == 3){
                y = EixoYJoyStick.read() * 1000;
                if(contador_z >= valor_final_z){
                    enable_motor_3 = 1;
                    contador_z = valor_final_z;
                    // printf("Posicao Y = %4d \n \r", contador_y);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_3 = 0;
                    motor3.gira_motor_sentido_horario();
                    contador_z += 1;
                    // printf("Posicao Y = %4d \n \r", contador_y); 
                }
                
            }
            while(y < 300 && flag_jogManual == 3){
                y = EixoYJoyStick.read() * 1000;
                if(contador_z <= valor_inicial_z){
                    enable_motor_3 = 1;
                    contador_z = valor_inicial_z;
                    // printf("Posicao Y = %4d \n \r", contador);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_3 = 0;
                    motor3.gira_motor_sentido_antihorario();
                    contador_z -= 1;
                    // printf("Posicao Y = %4d \n \r", contador);
                }
            }

            pos_atual_z = contador_z;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1;
        }

        while(flag_jogManual == 4){
            quantidade_mls = ml_de_despejo();
            lista_pos_x[conta_index] = pos_atual_x;
            lista_pos_y[conta_index] = pos_atual_y;
            lista_pos_z[conta_index] = pos_atual_z;
            lista_ml[conta_index] = quantidade_mls;
            ultima_posicao_x = pos_atual_x;
            ultima_posicao_y = pos_atual_y;
            ultima_posicao_z = pos_atual_z;
            printf("Posicao salva x: %d \n \r", lista_pos_x[conta_index]);
            printf("Posicao salva y: %d \n \r", lista_pos_y[conta_index]);
            printf("Posicao salva z: %d \n \r", lista_pos_z[conta_index]);
            printf("Mls salvo: %d \n \r", lista_ml[conta_index]);
            printf("Valor do contador %d \n \r", conta_index);
            printf("Valor do index %d \n \r", index);
            conta_index += 1;
            if(execucao >= 7){
                if(conta_index > index){
                    flag_jogManual = 5;
                    break;
                } else {
                    flag_jogManual = 1;
                    lcd.cls();
                    lcd.printf("Agora selecione a\n" );
                    lcd.printf("posicao %3d \n", conta_index);
                    printf("Selecione a proxima posicao \n \r");
                    wait(2);
                    break;
                }
            } else {
                if(conta_index >= index){
                    flag_jogManual = 5;
                    break;
                } else {
                    flag_jogManual = 1;
                    lcd.cls();
                    lcd.printf("Agora selecione a posicao %3d \n", conta_index);
                    printf("Selecione a proxima posicao \n \r");
                    wait(2);
                    break;
                }
            }
            
        }

        if(flag_jogManual == 5){
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1; 
            printf("JogManual finalizado \n \r");
            break;
        }


        enable_motor_1 = 1;
        enable_motor_2 = 1;
        enable_motor_3 = 1;

        }
    }
    
    while(conta_index <= index){
        
        lcd_show(4);
        
        // EIXO X
        contador_x = pos_atual_x;
        while(flag_jogManual == 1){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 0);
            lcd.printf("X de coleta:%4d\n", pos_atual_x);
            while(y > 600 && flag_jogManual == 1){
                y = EixoYJoyStick.read() * 1000;
                if(contador_x >= valor_final_x){
                    enable_motor_1 = 1;
                    contador_x = valor_final_x;
                    // printf("Posicao X = %4d \n \r", contador_x);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_1 = 0;
                    motor1.gira_motor_sentido_antihorario();
                    contador_x += 1;
                    // printf("Posicao X = %4d \n \r", contador_x); 
                }
                
            }
            while(y < 300 && flag_jogManual == 1){
                y = EixoYJoyStick.read() * 1000;
                if(contador_x <= valor_inicial_x){
                    enable_motor_1 = 1;
                    contador_x = valor_inicial_x;
                    // printf("Posicao X = %4d \n \r", contador_x);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_1 = 0;
                    motor1.gira_motor_sentido_horario();
                    contador_x -= 1;
                    // printf("Posicao X = %4d \n \r", contador_x);
                }
            }
            pos_atual_x = contador_x;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
        }

        // EIXO Y
        contador_y = pos_atual_y;
        while(flag_jogManual == 2){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 1);
            lcd.printf("Y de coleta:%4d\n", pos_atual_y);
            
            while(y > 600 && flag_jogManual == 2){
                y = EixoYJoyStick.read() * 1000;
                if(contador_y >= valor_final_y){
                    enable_motor_2 = 1;
                    contador_y = valor_final_y;
                    // printf("Posicao Y = %4d \n \r", contador_y);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_2 = 0;
                    motor2.gira_motor_sentido_horario();
                    contador_y += 1;
                    // printf("Posicao Y = %4d \n \r", contador_y); 
                }
                
            }
            while(y <  300 && flag_jogManual == 2){
                y = EixoYJoyStick.read() * 1000;
                if(contador_y <= valor_inicial_y){
                    enable_motor_2 = 1;
                    contador_y = valor_inicial_y;
                    // printf("Posicao Y = %4d \n \r", contador);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_2 = 0;
                    motor2.gira_motor_sentido_antihorario();
                    contador_y -= 1;
                    // printf("Posicao Y = %4d \n \r", contador);
                }
            }

            pos_atual_y = contador_y;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
        }

        contador_z = pos_atual_z;
        while(flag_jogManual == 3){
            y = EixoYJoyStick.read() * 1000;
            
            lcd.locate(0, 2);
            lcd.printf("Z de coleta:%4d\n", pos_atual_z);
            
            while(y > 600 && flag_jogManual == 3){
                y = EixoYJoyStick.read() * 1000;
                if(contador_z >= valor_final_z){
                    enable_motor_3 = 1;
                    contador_z = valor_final_z;
                    // printf("Posicao Y = %4d \n \r", contador_y);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_3 = 0;
                    motor3.gira_motor_sentido_horario();
                    contador_z += 1;
                    // printf("Posicao Y = %4d \n \r", contador_y); 
                }
                
            }
            while(y < 300 && flag_jogManual == 3){
                y = EixoYJoyStick.read() * 1000;
                if(contador_z <= valor_inicial_z){
                    enable_motor_3 = 1;
                    contador_z = valor_inicial_z;
                    // printf("Posicao Y = %4d \n \r", contador);
                    continue;
                } else {
                    gerador_step_driver = !gerador_step_driver;
                    wait_ms(tempo);
                    enable_motor_3 = 0;
                    motor3.gira_motor_sentido_antihorario();
                    contador_z -= 1;
                    // printf("Posicao Y = %4d \n \r", contador);
                }
            }

            pos_atual_z = contador_z;
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1;
        }

        while(flag_jogManual == 4){
            lista_pos_x[conta_index] = pos_atual_x;
            lista_pos_y[conta_index] = pos_atual_y;
            lista_pos_z[conta_index] = pos_atual_z;
            ultima_posicao_x = pos_atual_x;
            ultima_posicao_y = pos_atual_y;
            ultima_posicao_z = pos_atual_z;
            printf("Posicao salva x: %d \n \r", lista_pos_x[conta_index]);
            printf("Posicao salva y: %d \n \r", lista_pos_y[conta_index]);
            printf("Posicao salva z: %d \n \r", lista_pos_z[conta_index]);
            printf("Valor do contador %d \n \r", conta_index);
            printf("Valor do index %d \n \r", index);
            conta_index += 1;
            if(execucao >= 7){
                if(conta_index > index){
                    flag_jogManual = 5;
                    break;
                } else {
                    flag_jogManual = 1;
                    lcd.cls();
                    lcd.printf("Agora selecione a posicao %3d \n", conta_index);
                    printf("Selecione a proxima posicao \n \r");
                    wait(2);
                    break;
                }
            } else {
                if(conta_index >= index){
                    flag_jogManual = 5;
                    break;
                } else {
                    flag_jogManual = 1;
                    lcd.cls();
                    lcd.printf("Agora selecione a posicao %3d \n", conta_index);
                    printf("Selecione a proxima posicao \n \r");
                    wait(2);
                    break;
                }
            }
            
        }

        if(flag_jogManual == 5){
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1; 
            printf("JogManual finalizado \n \r");
            break;
        }


        enable_motor_1 = 1;
        enable_motor_2 = 1;
        enable_motor_3 = 1;

    }

    
}

void jog_automatico_x_y(int target_value_x, int target_value_y){
    contador_auto_x = ultima_posicao_x;
    printf("Contador atual x %d \n \r", contador_auto_x);
    contador_auto_y = ultima_posicao_y;
    printf("Contador atual y%d \n \r", contador_auto_y);

    printf("Target atual x %d \n \r", target_value_x);
    printf("Target atual y %d \n \r", target_value_y);

    eixo_x_finalizado = 0;
    eixo_y_finalizado = 0;
    while(1){
        gerador_step_driver = !gerador_step_driver;
        wait_ms(tempo);
        if(contador_auto_x > target_value_x){
            // gerador_step_driver = !gerador_step_driver;
            
            enable_motor_1 = 0;
            motor1.gira_motor_sentido_horario();
            contador_auto_x -= 1;
        } else if(contador_auto_x == target_value_x){
            // printf("Posicionado no target value x \n \r");
            enable_motor_1 = 1;
            ultima_posicao_x = contador_auto_x;
            eixo_x_finalizado = 1;
        } else if(contador_auto_x < target_value_x){
            // gerador_step_driver = !gerador_step_driver;
            // wait_ms(tempo);
            enable_motor_1 = 0;
            motor1.gira_motor_sentido_antihorario();
            contador_auto_x += 1;
        }

        if(contador_auto_y > target_value_y){
            // gerador_step_driver = !gerador_step_driver;
            // wait_ms(tempo);
            enable_motor_2 = 0;
            motor2.gira_motor_sentido_antihorario();
            contador_auto_y -= 1;
        } else if(contador_auto_y == target_value_y){
            // printf("Posicionado no target value y \n \r");
            enable_motor_2 = 1;
            ultima_posicao_y = contador_auto_y;
            eixo_y_finalizado = 1;
        } else if(contador_auto_y < target_value_y){
            
            // wait_ms(tempo);
            enable_motor_2 = 0;
            motor2.gira_motor_sentido_horario();
            contador_auto_y += 1;
        }

        if(eixo_x_finalizado && eixo_y_finalizado){
            // enable_motor_1 = 1;
            // enable_motor_2 = 1;
            break;
        }
        
    }
}

void jog_automatico_y(int target_value){
    contador_auto_y = ultima_posicao_y;
    printf("Contador atual y%d \n \r", contador_auto_y);
    while(1){
        if(contador_auto_y > target_value){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_2 = 0;
            motor2.gira_motor_sentido_antihorario();
            contador_auto_y -= 1;
        } else if(contador_auto_y == target_value){
            printf("Posicionado no target value y \n \r");
            enable_motor_2 = 1;
            ultima_posicao_y = contador_auto_y;
            break;
        } else if(contador_auto_y < target_value){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_2 = 0;
            motor2.gira_motor_sentido_horario();
            contador_auto_y += 1;
        }
        
    }
}

void jog_automatico_z(int target_value){
    contador_auto_z = ultima_posicao_z;
    printf("Contador atual z %d \n \r", contador_auto_z);
    enable_motor_1 = 1;
    enable_motor_2 = 1;
    while(1){
        if(contador_auto_z > target_value){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_3 = 0;
            motor3.gira_motor_sentido_antihorario();
            contador_auto_z -= 1;
        } else if(contador_auto_z == target_value){
            printf("Posicionado no target value z \n \r");
            ultima_posicao_z = contador_auto_z;
            enable_motor_3 = 1;
            
            break;
        } else if(contador_auto_z < target_value){
            gerador_step_driver = !gerador_step_driver;
            wait_ms(tempo);
            enable_motor_3 = 0;
            motor3.gira_motor_sentido_horario();
            contador_auto_z += 1;
        }
        
    }
}

void handle_ok(){
    if(debounce.read_ms() > 650){
        flag_jogManual = 4;
        printf("Ok \n \r");
    }
    debounce.reset();
}

void handle_confirma(){
    if(debounce.read_ms() > 500){ 
        execucao += 1;   
        printf("Variavei exec %d \n \r", execucao);
    }
    autoriza_etapa = 1;
    wait(2);
    debounce.reset(); 
}

void handle_voltar(){
    if(debounce.read_ms() > 150){
        
        if(execucao == 3 || execucao == 4) {
            flag_referenciamento_x = 1;
            flag_referenciamento_y = 1;
            flag_referenciamento_z = 1;
            execucao = 0;
        } else if(execucao == 7 || execucao == 8){
            execucao = 3;
        } else if (execucao > 9){
            execucao = 7;
        }
        printf("Valor do exec %d \n \r", execucao);

    }
    debounce.reset();
}

void handle_emergency(){
    reset_motors = 0;
    enable_motor_1 = 1;
    enable_motor_2 = 1;
    enable_motor_3 = 1;
    NVIC_SystemReset();
    
}

int seleciona_pontos_de_solta(){
    lcd.cls();
    lcd_show(5);
    while(1){
            y = EixoYJoyStick.read() * 1000;
            lcd.locate(0, 1);
            lcd.printf("De despejo: %3d\n", numero_de_soltas);
            while(y < 300){
                wait(0.3);
                y = EixoYJoyStick.read() * 1000;
                if(numero_de_soltas >= 9){
                    numero_de_soltas = 9;
                    printf("Numero de soltas: %d \n \r", numero_de_soltas);
                    continue;
                } else {
                    numero_de_soltas += 1;
                    printf("Numero de soltas: %d \n \r", numero_de_soltas);
                }
                
            }
            while(y > 600){
                wait(0.3);
                y = EixoYJoyStick.read() * 1000;
                if(numero_de_soltas <= 1){
                    numero_de_soltas = 1;
                    printf("Numero de soltas: %d \n \r", numero_de_soltas);
                    continue;
                } else {
                    numero_de_soltas -= 1;
                    printf("Numero de soltas: %d \n \r", numero_de_soltas);
                }
            }

            if(execucao == 7){
                break;
            }
            
        }
}

int main() {
    
    debounce.start();
    fim_de_curso_inicial_x.rise(&valida_fim_x);
    fim_de_curso_final_x.rise(&valida_inicio_x);
    fim_de_curso_inicial_y.rise(&valida_inicio_y);
    fim_de_curso_final_y.rise(&valida_fim_y);
    fim_de_curso_inicial_z.rise(&valida_inicio_z);
    fim_de_curso_final_z.rise(&valida_fim_z);
    botao_x.rise(&muda_eixo_x);
    botao_y.rise(&muda_eixo_y);
    botao_z.rise(&muda_eixo_z);
    botao_ok.rise(&handle_ok);
    botao_confirma.rise(&handle_confirma);
    botao_voltar.rise(&handle_voltar);
    botao_emergencia.fall(&handle_emergency);

    reset_motors = 1;
    enable_motor_1 = 1;
    enable_motor_2 = 1;
    enable_motor_3 = 1;
    gerador_step_driver = 1;
    motor1.gira_motor_sentido_horario();
    motor2.gira_motor_sentido_horario();
    motor3.gira_motor_sentido_horario();
    
    aciona_pipeta = 1;

    lcd.setBacklight(TextLCD::LightOn);
    lcd.setCursor(TextLCD::CurOff_BlkOn);

    if(botao_emergencia == 0){
        lcd_show(3);
        enable_motor_1 = 1;
        enable_motor_2 = 1;
        enable_motor_3 = 1;
        gerador_step_driver = 0;
        while(botao_emergencia == 0){}

    }

    while(1){
        enable_motor_1 = 1;
        enable_motor_2 = 1;
        enable_motor_3 = 1;
        if(execucao == 0){
            lcd_show(0);
            execucao = 1;
        } 
        else if(execucao == 2){
            lcd_show(1);
            referenciar_z(1);
            referenciar_x(1);
            referenciar_y(1);
            execucao = 3;
        }
        else if(execucao == 3){
            lcd_show(2);
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1;
            gerador_step_driver = 0;
            execucao = 4;
        }
        else if(execucao == 5){
            jogManual(1);
            execucao = 6;
        } 
        else if(execucao == 6){
            seleciona_pontos_de_solta();
        }
        else if(execucao == 7){
            lcd_show(6);
            execucao = 8;
        }
        else if(execucao == 9){
            flag_jogManual = 1;
            printf("Numero de posicoes de solta %d \n \r", numero_de_soltas+1);
            jogManual(numero_de_soltas);
            etapa_pos_setup = 9 + numero_de_soltas;
            execucao = etapa_pos_setup;
        }
        else if(execucao == etapa_pos_setup){
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1;
            gerador_step_driver = 1;
            lcd_show(7);
            autoriza_etapa = 0;
            execucao = etapa_pos_setup + 1;
        }
        else if(execucao == etapa_pos_setup + 1 && autoriza_etapa == 1){
            lcd_show(8);
            jog_automatico_z(valor_inicial_z);
            execucao = etapa_pos_setup + 2;
        }
        else if(execucao == etapa_pos_setup + 2){
            execucao = 14;
            lcd_show(9);
            for(int i = 1; i <= numero_de_soltas; i++){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Ciclo para posicao %d \n", i);

                printf("Ciclo para posicao %d \n \r", i);
                wait(1);
                contador = 0;
                lcd.locate(0,1);
                lcd.printf("Iteracao numero %d \n", contador);
                
                if(i >= 0){
                    quantidade_mls = lista_ml[i];
                    while(contador < quantidade_mls){
                        lcd.locate(0,1);
                        lcd.printf("Iteracao numero %d \n", contador);
                        printf("Iteração numero %d \n \r", contador);
                        jog_automatico_z(valor_inicial_z);
                        jog_automatico_x_y(lista_pos_x[0], lista_pos_y[0]);
                        jog_automatico_z(lista_pos_z[0]);
                        aciona_pipeta = 0;
                        wait(2);
                        aciona_pipeta = 1;
                        wait(4);
                        // wait(5);
                        jog_automatico_z(valor_inicial_z);
                        
                        jog_automatico_x_y(lista_pos_x[i], lista_pos_y[i]);
                        jog_automatico_z(lista_pos_z[i]);

                        aciona_pipeta = 0;
                        wait(2);
                        aciona_pipeta = 1;
                        wait(4);
                        contador += 1;
                    }
                }             
            }
            jog_automatico_z(valor_inicial_z);
            jog_automatico_x_y(valor_inicial_x, valor_inicial_y);
            lcd.cls();
            lcd.printf("Ciclos finalizados");
            enable_motor_1 = 1;
            enable_motor_2 = 1;
            enable_motor_3 = 1;
        }
        else if(execucao == 14){
            execucao = etapa_pos_setup;
        }
            
        
    }
    
}
