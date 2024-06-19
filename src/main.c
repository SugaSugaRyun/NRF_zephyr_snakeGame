#include <zephyr/kernel.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "gpios.h"
#include "led.h"

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

// add for joystick
int32_t preX = 0 , perY = 0;
static const int ADC_MAX = 1023;
// static const int ADC_MIN = 0;
static const int AXIS_DEVIATION = ADC_MAX / 2;
int32_t nowX = 0;
int32_t nowY = 0;

//TODO sholud be replaced with Joystick event!
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define PAUSE 112
#define ESC 27
int key;
//TODO ==============================================

// thread stack size
#define STACK_SIZE 1024

// thread priority
#define PRIORITY 7

// thread stack memory allocation
// K_THREAD_STACK_DEFINE(thread_stack_area, STACK_SIZE);

// // thread data structure
// struct k_thread my_thread_data;

//map size setted by display size
#define MAP_WIDTH 16
#define MAP_HEIGHT 8
//for map state
// #define EMPTY 0
#define SNAKE 1
#define APPLE 2
//for game mode
#define PLAY 0
#define SCORE 1
//initial values
#define INIT_LENGTH 3
#define INIT_SPEED 200
//for BGM
#define NOTE_CNT 3

int map[MAP_HEIGHT*MAP_WIDTH]; //whole map state. 0: empty / 1: snake / 2: apple
int x[128], y[128]; //save the position of snake
int apple_x, apple_y; //save the position of apple
int length; //current length of snake. can be changed by scoring
int speed; //current speed of game. can be changed by rotary encoder event
int dir; //current direction of snake. can be changed by joystick event

//for game logic
int idx(int x, int y);
void print_map();
void move(int dir); 
void create_apple(); 
//for gpio setting and interrupt setting
void setting_io();
//for button interrupt
void play_routine();  
void score_routine();
bool isChange(void);
void control_joystick();

int mode = SCORE;
int pause_flag = 0;
int mute_flag = 0;
int change_note_flag= 0;
int note_type; //type of BGM. 0~2
int notes[NOTE_CNT];//notes

// ========INTERRUPT FUNCS============================
//start-stop game
void btn1_pushed(){
    if(mode == PLAY) score_routine();
    else if(mode == SCORE) play_routine();
}

//pause game
void btn2_pushed(){
    if(pause_flag) pause_flag = 0;
    else pause_flag = 1;
}

//mute-unmute BGM
void btn3_pushed(){
    if(mute_flag) mute_flag = 0;
    else mute_flag = 1;
}

//change BGM
void btn4_pushed(){
   note_type = (note_type+1)%NOTE_CNT; 
   change_note_flag = 1;
}

//TODO change speed
void rotary_encoder_moved(){
    int s = /*value*/ 100;
    if(s < 0) speed = 1000; //minimum speed
    if(s > 99999 /*borderline*/) speed = 10; //maximum speed 
}

//=======================================================


int main(){
    pwm_init();
    decoder_init();
    setting_io();
    speed = INIT_SPEED;  
    //TODO setting interrupts
    // score_routine();
    dir = LEFT;
    printk("hallo night\n");
    // k_tid_t my_tid = k_thread_create(&my_thread_data, thread_stack_area,
    //                                  K_THREAD_STACK_SIZEOF(thread_stack_area),
    //                                  control_joystick, NULL, NULL, NULL,
    //                                  PRIORITY, 0, K_NO_WAIT);

    // // 스레드 이름 설정 (디버깅 용도로 유용)
    // k_thread_name_set(my_tid, "my_thread");

    while(1){
        printk("dir:%d\n", dir);
    };

    return 0;

}

// play the snake game
void play_routine(){
    mode = PLAY;

    //init map state
    for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++) map[i] = 0;

    //reset values
    dir = RIGHT; 
    length = INIT_LENGTH;  

    //set initial body position
    for(int i = 0; i < length; i++){  
        x[i]=MAP_WIDTH/2+i;
        y[i]=MAP_HEIGHT/2;
        map[idx(x[i], y[i])] = 1;
    }map[idx(x[0], y[0])] = 1;

    //create new apple
    create_apple();

    //start playing
    while(1){

        //TODO shuld be replaced with Joystick event ==================
        //get moving event, using key now... 
        // if(kbhit()) do{key=getch();} while(key==224); 
        // //if joystick area is changed...
        // if(key == LEFT || key == RIGHT || key == UP || key == DOWN){
        //     if((dir==LEFT&&key!=RIGHT)||(dir==RIGHT&&key!=LEFT)||(dir==UP&&key!=DOWN)||(dir==DOWN&&key!=UP)) dir=key; 
        //     key=0; 
        // }

        //TODO should be replaced with button1 interrupt
        if(key == ESC){
            btn1_pushed(); 
            key = 0;
        }

        //TODO should be replaced with button2 interrupt
        if(key == PAUSE){
            // printf("PAUSESDFSDFSDFSDF\n");
            btn2_pushed(); 
            key = 0;
        }
        //TODO ===========================================================

        if(pause_flag == 0) move(dir); 
        // Sleep(speed); //should be replaced... UNIX? idk
        k_sleep(K_MSEC(speed));
    }//while
}

//display the last score
void score_routine(){  
    mode = SCORE;

    //TODO change screen with score got

    // printf("----------------------------------\n");
    // printf("\n\nSCORE: %d\n\n", length - INIT_LENGTH); //for debug
    // printf("----------------------------------\n");

    //TODO sholud be replaced.... with interrupt handling... 
    // Sleep(500);
    //     while (kbhit()) getch();
    // key=getch();
    btn1_pushed();
    //TODO ===================================================
}

//move using dir 
void move(int dir){
    int i;

    //eat apple case
    if(x[0] == apple_x && y[0] == apple_y){  
        create_apple();  
        length++;  
        x[length-1] = x[length-2];  
        y[length-1] = y[length-2];
    }
    //blocked by wall case
    if(x[0] == 0 || x[0] == MAP_WIDTH-1 || y[0]==0 || y[0] == MAP_HEIGHT-1){  
        score_routine();
        return;  
    }
    //blocked by body case
    for(i = 1; i < length; i++){  
        if(x[0] == x[i] && y[0] == y[i]){
            score_routine();
            return;
        }
    }

    //erase last tail
    map[idx(x[length-1], y[length-1])] = 0;
    for(i = length-1; i > 0; i--){  
        x[i] = x[i-1];
        y[i] = y[i-1];
    }
    //draw new snake
    map[idx(x[0], y[0])] = 1;
    if(dir == LEFT) --x[0];  
    if(dir == RIGHT) ++x[0];
    if(dir == UP) --y[0]; 
    if(dir == DOWN) ++y[0];     
    map[idx(x[i], y[i])] = 1;
    // print_map(); 

}

//create new apple
void create_apple(){
    int i;
    int dup_flag =0; 

    while(1){            
        //select next apple place
        apple_x = (rand() % (MAP_WIDTH-2)) + 1;     
        apple_y = (rand() % (MAP_HEIGHT-2)) + 1;
        
        dup_flag = 0;    
        for(i = 0; i < length; i++){   
            if(apple_x == x[i] && apple_y == y[i]){
                dup_flag = 1;  
                break;
            }
        }
        if(dup_flag == 1) continue; 

        //draw apple DROP     
        map[idx(apple_x, apple_y)] = 2;
        break;
        
    }
}

//calculate index for map using 
int idx(int x, int y){ 
    return (( y * 16 ) + x); 
}

//print current map state for debug
//TODO add the part for display at dotted metrics
void print_map(){
    // printf("-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -\n");
    // for(int i = 0; i < MAP_HEIGHT*MAP_WIDTH; i++){
    //     if(i%16 == 0) printf("|");
    //     if(map[i] == SNAKE) printf(" S ");
    //     else if(map[i] == APPLE) printf(" A ");
    //     else printf("   ");
    //     if(i%16 == 15) printf("|\n");
    // }
    // printf("-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -\n");
    // printf("SCORE: %d | pause: %d\n\n", length - INIT_LENGTH, pause_flag);
}

void setting_io(){
    //TODO setting I/O devices and interrupt
}

//play the BGM using buzzer - using note
void play_BGM(){

}

bool isChange(void)
{
	if((nowX < (preX - 50)) || nowX > (preX+50)){
		preX = nowX;
		return true;
	}

	if((nowY < (perY - 50)) || nowY > (perY+50)){
		perY = nowY;
		return true;
	}
	return false;
}
//endif
void control_joystick()
{
	int err;
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return;
		}
	}

	led_init();

	while (1) {
		printk("ADC reading[%u]: ", count++);

		(void)adc_sequence_init_dt(&adc_channels[0], &sequence);
		err = adc_read(adc_channels[0].dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		}

		nowX = (int32_t)buf;

		(void)adc_sequence_init_dt(&adc_channels[1], &sequence);
		err = adc_read(adc_channels[1].dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		}

		nowY = (int32_t)buf;

        printk("Joy X: %" PRIu32 ", ", nowX);
		printk("Joy Y: %" PRIu32 ", ", nowY);

		if (nowX >= 65500 || nowY >= 65500){
			printk("Out of Range\n");
			k_sleep(K_MSEC(100));
			continue;
		}

		bool checkFlag = isChange();
		if(!checkFlag){
            printk("No Change\n");
			k_sleep(K_MSEC(100));
			continue;
		} 
        // else {
		// 	led_off_all();
		// }
//  if((dir==LEFT&&key!=RIGHT)||(dir==RIGHT&&key!=LEFT)||(dir==UP&&key!=DOWN)||(dir==DOWN&&key!=UP)) dir=key; 
		if (nowX == ADC_MAX && nowY == ADC_MAX){
			// led_on_center();
			printk("Center");
		} else if (nowX < AXIS_DEVIATION && nowY == ADC_MAX){
			// led_on_left();
			printk("Left");
            if(dir != RIGHT) dir = LEFT;
		} else if (nowX > AXIS_DEVIATION && nowY == ADC_MAX){
			// led_on_right();
			printk("Right");
            if(dir != LEFT) dir = RIGHT;
		} else if (nowY > AXIS_DEVIATION && nowX == ADC_MAX){
			// led_on_up();
			printk("Up");
            if(dir != DOWN) dir = UP;
		} else if (nowY < AXIS_DEVIATION && nowX == ADC_MAX){
			// led_on_down();
			printk("Down");
            if(dir != UP) dir = DOWN;
		}

        printk("\n");

		k_sleep(K_MSEC(100));
	}
	return;
}