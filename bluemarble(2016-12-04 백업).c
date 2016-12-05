#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // time
#include <unistd.h> // Sleep, 사용자 함수 kbhit 구현
#include <termio.h> // 사용자 함수 getch 구현
#include <termios.h> // 사용자 함수 kbhit 구현
#include <fcntl.h> // 사용자 함수 kbhit 구현

#define BOARD 28  // 보드의 칸의 갯수
#define PLAYER 2 // 플레이어 수

// 키보드 상수 값 설정
#define SPACE 32; // 키보드 상수 값
#define ENTER 13;
#define UP 72;
#define DOWN 80;

#define DEFAULT printf("%c[%dm", 0x1B, 0)
#define WHITE printf("\x1b[37m") // YES or NO
#define YELLOW printf("\x1b[33m") // 플레이어1
#define MAGENTA printf("\x1b[35m") // 화살표
#define RED printf("\x1b[31m") // 플레이어2
#define BLUE printf("\x1b[36m") // 공지
#define GREEN printf("\x1b[32m") // 주사위
#define BLACK printf("\x1b[30m")



typedef struct { // 플레이어 정보 구조체
char name[20]; // 플레이어의 이름
int money; // 플레이어의 소지금
int rest; // 무인도로 인한 휴식 처음에 0을 주고 황금열쇠에서 무인도에 걸리면 2를 주는거
int position; // 플레이어의 위치(몇번째 도시에 해당하는가)
char icon[4]; // 플레이어의 아이콘
}Player;



Player player[PLAYER] = { // 플레이어의 정보 초기화 // 여기부분도 수정
//  이름   소지금 무인도 위치 아이콘
{"플레이어1", 200, 0, 0, "★"}, // 아이콘 색 노란색 세우는 건물도 노란색
{"플레이어2", 200, 0, 0, "♥"} // 아이콘 색 빨간색 세우는 건물도 빨간색
}; // 플레이어의 정보 초기화



typedef struct { // 도시 정보 구조체 선언
char name[100]; // 도시이름
int development[4]; // 도시에 건물 값 , 0 도시구매가, 1 주택, 2 빌라, 3 아파트
int development_level; // 맨처음에 도시의 발전도 0 구매 -> 1 빌라 -> 2면 아파트 세울 수 있음
char owner[20]; // 도시 소유주 이름
int x[2]; // 플레이어1,2의 말의 x좌표
int y[2]; // 플레이어1,2의 말의 y좌표 // 플레이어 1과 2가 어느곳에 말을 두게 될지
int building_x; // 건물 x좌표
int building_y; // 건물 y좌표 // 플레이어가 세우는 건물이 어느위치에 생기는지
}City;



City city[BOARD] = {
// 도시 24개
// 도시이름, 가격[4], 발전도, 소유주, 플x좌표[2], 플y좌표[2], 도x좌표, 도y좌표
{"시작", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 0 시작
{"제주", {50, 13, 16, 20}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 1 제주
{"서울", {80, 29, 36, 43}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 2 서울
{"가평", {10, 4, 6, 8}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 3 가평
{"용인", {60, 17, 22, 30}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 4 용인
{"파주", {8, 1, 2, 3}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 5 파주
{"인천", {24, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 6 인천
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 7 황금열쇠
{"춘천", {30, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 8 춘천
{"사회복지기금 접수처", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 9 사회복지기금 접수처
{"평창", {37, 15, 13, 9}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 10 평창
{"강원", {15, 4, 6, 8}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 11 강원
{"강릉", {11, 4, 6, 8}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 12 강릉
{"속초", {29, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 13 속초
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 14 황금열쇠
{"태안", {22, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 15 태안
{"사회복지기금 수령처", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 16 사회복지기금 수령처
{"단양", {5, 1, 2, 3}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 17 단양
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 18 황금열쇠
{"대전", {51, 17, 22, 30}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 19 대전
{"대구", {40, 9, 13, 15}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 20 대구
{"무인도", {0, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 21 무인도
{"부산", {80, 29, 36, 43}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 22 부산
{"경주", {23, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 23 경주 
{"독도", {200, 0, 0, 0}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 24 독도는 건물을 세울 수 없음
{"여수", {28, 6, 8, 10}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 25 여수
{"전주", {55, 17, 22, 30}, 0, "없음", {0, 0}, {0, 0}, 0, 0}, // 26 전주
{"안동", {38, 9, 13, 15}, 0, "없음", {0, 0}, {0, 0}, 0, 0} // 27 안동
};



int welfare = 50; // 사회복지기금



int dice_cast() { // 주사위 숫자 생성해주는 함수
int dice_num;

srand(time(NULL)); // 시간에 따라 변화하는 랜덤 함수
dice_num = rand()%6 + 1; // 1~6 사이 난수 생성

return dice_num;
} // int dice_case()



void gotoxy(int x, int y) { // 좌표 설정 함수
printf("\033[%d;%df",y,x);
fflush(stdout);
} // void gotoxy(int x, int y)



int getch(void) { // getch
int ch;
struct termios buf;
struct termios save;

tcgetattr(0, &save);
buf = save;
buf.c_lflag &= ~(ICANON | ECHO);
buf.c_cc[VMIN] = 1;
buf.c_cc[VTIME] = 0;
tcsetattr(0, TCSAFLUSH, &buf);
ch = getchar();
tcsetattr(0, TCSAFLUSH, &save);

return ch;
} // int getch(void)



/*int kbhit(void) {
struct termios oldt, newt;
int ch;
int oldf;

tcgetattr(STDIN_FILENO, &oldt);
newt = oldt;
newt.c_lflag &= ~(ICANON | ECHO);
tcsetattr(STDIN_FILENO, TCSANOW, &newt);
oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

ch = getchar();

tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
fcntl(STDIN_FILENO, F_SETFL, oldf);

if(ch != EOF) {
ungetc(ch, stdin);
return 1;
}

return 0;
} // int kbhit(void)
*/


void insert_playername(int a) { // 시작 시 플레이어 이름 입력
int i;
for (i=0 ; i<PLAYER ; i++){
printf("플레이어%d 이름 입력 : ", i+1);
scanf("%s", player[i].name);
} // for
} // void insert_playername



void finish_bonus(int a) { // 시작점에 도착해서 완주 시
// gotoxy(공지 뜨는 위치)
BLUE; // 공지 파란색으로 뜸
printf("[%s]님 완주!\n", player[a].name); // 플레이어의 이름 출력
// gotoxy(공지 뜨는 위치 y축 한칸 아래)
printf("완주 보너스 10만원 지급!\n");
player[a].money += 10; // 소지금 + 10
} // void finish_bonus(int a)



void player_state() { // 게임 시작 시 셋팅
int i;
for (i=0 ; i<PLAYER ; i++) {
if(i==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(i==1)
RED;

printf("%s\n", player[i].icon);
printf("[%s]님 정보\n", player[i].name);
printf("소지금 : [%d만원]\n", player[i].money);

int j; // 도시 돌림
int city_count = 0;
for(j=0 ; j<BOARD ; j++) {
if(i == 0) { // 플레이어 1의 소유 도시 수 세기
if(!strcmp(city[j].owner, player[0].name))
city_count++;
} // if(a==0)

else {
if(!strcmp(city[j].owner, player[1].name))
city_count++;
}
} // for BOARD의 수 만큼 for문 돌림
printf("소유 도시 갯수 : %d개\n", city_count);
} // for 플레이어 수
} // void player_state();



void icon_print(int a) { // 플레이어의 말이 위치한 말판 위에 플레이어의 말을 출력하는 함수
// gotoxy(city[player[a].position].x[a], city[player[a].position].y[a]);
// gotoxy(플레이어가 위치한 몇번째 말판.x좌표, 플레이어가 위치한 몇번째 말판.y좌표)

if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

printf("%s\n", player[a].icon);
} // void icon_print(int a)



void development_mark(int a) { // 건물 구입 시 표시하는 함수 (= 발전도)
//gotoxy(city[player[a].position].building_x, city[player[a].position].building_y);
// gotoxy(플레이어의 말이 위치한 몇번째 발판.건물표시 x위치, 플레이어의 말이 위치한 몇번째 발판.건물표시 y위치)

int i;
for(i=0 ; i<=city[player[a].position].development_level ; i++) { // 플레이어의 말이 있는 도시의 발전도만큼 반복
if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

printf("█ "); // 플레이어의 말이 있는 도시의 발전도 만큼 건물아이콘을 출력함
} //for
printf("\n");
} // void development_mark(int a)


// 테스트 완료
void clear() { // 글자 지우기
// gotoxy(글자나오는데);
// printf("      "); // 그 글자나오는데의 칸만큼 삭제
} // void clear()



void icon_movement(int a) { // 주사위 굴리고, 말판위에 말 움직이는 함수
int d; // 주사위
// gotoxy(공지뜨는데);
if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

printf("[%s]님의 차례입니다.\n", player[a].name);
// gotoxy(바로 아랫줄)

printf("주사위를 굴려주세요.\n");
// getch();
sleep(1); // 1초 후에
GREEN; // 주사위 색깔
d = dice_cast(); // 주사위 굴리는 함수
printf("%d",d); // 주사위 출력

int i;
for(i=0 ; i<d ;  i++) {
//gotoxy(city[player[a].position].x[a], city[player[a].position].y[a]); // 플레이어의 말이 뜨는 위치에 가서
printf(" "); // 말 지나간 자리 지우기

if(player[a].position == 25) { // 플레이어가 마지막 말판에 도착하게 되면 // 이부분 나중에 말판 갯수로 고치기
player[a].position = 0; // 시작 위치로 돌아오고
finish_bonus(a); // 완주 보너스 지급
sleep(2);
icon_print(a); // 플레이어의 말 출력
}//if

else {
player[a].position += 1;
icon_print(a);
} // else
} // for
} // void icon_movement(int a)



void gold_key(int a) {
int d; // 주사위

BLUE;
printf("[황금열쇠]칸에 도착하였습니다.\n");
printf("주사위를 굴려주세요.\n");
sleep(1); // 1초 후에
// 키 입력 받으면 던져지는 걸로 바꾸기
d = dice_cast(); // 주사위 던짐
sleep(1); // 1초 후에
printf("[%d]가 나왔습니다.\n", d);

switch(d) { // 주사위값
case 1: // 1. 사회복지기금 수령처로 이동 (플레이어 소지금액 + 사회복지기금)
printf("[%d]는 [사회복지 기금 수령처] 입니다.\n", d);
player[a].position = 16; // 위치를 사회복지기금 수령처로
icon_print(a); // 플레이어의 말 출력
printf("사회복지기금 수령처에 모금 된 돈을 받습니다.\n");
player[a].money += welfare; // 사회복지기금을 받고
welfare = 0; // 탈탈
break;

case 2: // 2. 무인도 (3턴 고립)
printf("[%d]는 [무인도] 입니다.\n", d);
player[a].position = 21; // 위치를 무인도로
icon_print(a); // 플레이어의 말 출력
printf("무인도에 고립되어, 3턴 동안 플레이를 하지 못하게 됩니다.\n");
player[a].rest = 3;
island(a);
break;

case 3: // 3. 복권 당첨 (플레이어 소지금액 +15)
printf("[%d]는 [복권 당첨] 입니다.\n", d);
printf("15만원을 받습니다.\n");
player[a].money += 15;
break;

case 4: // 4. 생일축하 (상대플레이어게서 10만원 받기) , 플레이어의 소지금액 +10, 상대 플레이어 소지금액 -10
printf("[%d]는 [생일축하] 입니다.\n", d);
printf("다른플레이어들에게 10만원 씩 받습니다.\n");
if(a == 0){
player[1].money -= 10;
player[0].money += 10;
} // if

if (a == 1)
player[1].money += 10;
player[0].money -= 10;
break;

case 5: // 5. 건물수리 (아파트 *-10, 빌라*-6, 주택*-3)
printf("[%d]는 [건물수리] 입니다.\n", d);
printf("소지한 건물들을 수리합니다.\n");

// for문이 돌면서 도시소유주가 누구인걸찾아서
int i; // 도시 수 for 돌릴 거
int total_building_fix; // 수리비 총액
int building_fix; // 건물상태에 따른 수리비 정의
building_fix = 0;
total_building_fix = 0;

for(i=0 ; i <BOARD ; i++) {
if(!strcmp(city[i].owner, player[a].name)) {
if(city[i].development_level == 2) {
building_fix = city[i].development[1]*0.2;
} // if (도시의 발전도가 2이면) {building_fix = 주택가격 *0.2}

else if(city[i].development_level == 3) {
building_fix = city[i].development[1]*0.2 + city[i].development[2]*0.3;
}// else if (도시의 발전도가 3이면) {building_fix = 주택가격*0.1 빌리가격*0.3}

else if(city[i].development_level == 4) {
building_fix = city[i].development[1]*0.2 + city[i].development[2]*0.3 + city[i].development[3]*0.5;
}// else if (도시의 발전도가 4이면) {building_fix = 주택가격*0.1 빌라가격*0.3 아파트가격*0.5}
// 도시의 발전도에 따른 각 도시의 수리비용 정의 끝
} // if 도시의 소유주가 자신인걸 찾아서
total_building_fix += building_fix;
} // for 도시 돌림
// bulding_fix_sum은 총얼마인ㅂ니다 프린트에프하고 니 의사와는 상관없이 건물을 고친다!!! 함
printf("소유지한 도시의 건물을 수리하시는 가격은 총 [%d만원]입니다.\n", total_building_fix);
player[a].money -= total_building_fix;// 플레이어[a].money -= bulding_fix_sum;
break;

case 6: // 6. 과속운전 (플레이어 소지금액 -5)
printf("[%d]는 [과속운전] 입니다.\n", d);
printf("벌금으로 5만원을 냅니다.\n");
player[a].money -= 5;
break;
} // switch
}// void gold_key(int a)



void trade(int a) { // 도시구매 // 주택, 빌라, 아파트 건설 // 통행료 납부 거래
BLUE; // 공지 색
// 1. 플레이어의 위치가 시작
if(!strcmp(city[player[a].position].name, "시작")) { // 플레이어가 위치한 칸의 이름이 시작이면
return;
} // if 시작

// 2. 플레이어의 위치가 황금열쇠
else if (!strcmp(city[player[a].position].name, "황금열쇠")) {
gold_key(a); // 황금열쇠 함수 호출
} // else if 황금열쇠

// 3. 플레이어의 위치가 무인도
else if(!strcmp(city[player[a].position].name, "무인도")) {

if(player[a].rest == 0){
return;
}

else
island(a);
} // else if 무인도

// 4. 플레이어의 위치가 사회복지기금 접수처
else if(!strcmp(city[player[a].position].name, "사회복지기금 접수처")) {
player[a].money -= 20; // 20만원 삥뜯
welfare += 20; // 사회복지기금으로 20만원 들어옴
} // else if 사회복지기금 접수처

// 5. 플레이어의 위치가 사회복지기금 수령처
else if(!strcmp(city[player[a].position].name, "사회복지기금 수령처")) {
// 근데 사회복지기금이 0원이면 억울하니까...
if(welfare == 0) {
player[a].money += 20;
} // 사회복지기금 모은게 0원이면

else {
player[a].money += welfare; // 플레이어가 사회복지기금을 수령받고
welfare = 0; // 사회복지기금 탈탈 
} // 네
} // else if 사회복지기금 접수처


// 6. 플레이어의 위치가 독도일 경우
else if(!strcmp(city[player[a].position].name, "독도")) {
// 1) 독도의 소유자가 없는 상태일 경우
if(!strcmp(city[player[a].position].owner, "없음")) { // 독도의 소유권이 없는 상태라면
printf("[독도]가 비어있습니다.\n", city[player[a].position].name);
printf("[%d만원]을 지불하고 독도를 구매하겠습니까?\n", city[player[a].position].development[0]); // 독도구매 금액

//if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++ // 발전도가 1이니까 한개생길듯
player[a].money -= city[player[a].position].development[0]; // 구매
//} // if
} // 독도 소유주 없을 때 if끝

// 2) 독도에 도착했을때, 독도가 자신의 소유지라면
else if(!strcmp(city[player[a].position].owner, player[a].name)) { // 독도가 자신의 소유지이면
printf("독도는 [%s]님의 섬 입니다.\n", player[a].name);
printf("독도는 건물을 세우지 못합니다.\n");
} // else if 독도가 자신의 소유지 일 경우 

// 3) 그외, 독도에 도착했는데 독도가 자신의 소유지가 아니라면 통행료를 내야한다
else {
// 독도가 누구의 소유지임을 알림
if(a == 1)
printf("독도는 [%s]님의 섬입니다.\n", city[player[0].position].owner);
else
printf("독도는 [%s]님의 섬입니다.\n", city[player[1].position].owner);

printf("독도를 지나가기 위해서는 통행료를 납부해야합니다.\n");

int dokdo_pass_price;
dokdo_pass_price = city[player[a].position].development[0]*0.9; // 독도 통행료는 독도인수가격 * 0.9

printf("통행료는 [%d만원]을 납부합니다.\n", dokdo_pass_price);

player[a].money -= dokdo_pass_price;

if(a == 0) // 플레이어1인 경우
player[1].money += dokdo_pass_price; // 플레이어 1에게 통행료가 들어와야함 
else
player[0].money += dokdo_pass_price;
} // else 독도가 빈 상태도 아니고 자신의 소유지도 아닐 경우 통행료를 내야함 
// 그 외의 경우 통행료 내야함
} // 독도

// 7. 플레이어가 위치 한 곳이 도시 이름일 경우, 도시의 소유자가 없을 때
else if(!strcmp(city[player[a].position].owner, "없음")) { // 그 외의 경우, 도시 이름이 적힌 말판에 도착, 소유주가 없는 땅 일 경우
printf("도시 [%s]가 비어있습니다.\n", city[player[a].position].name);
printf("[%d만원]을 지불하고 도시를 구매하겠습니까?\n", city[player[a].position].development[0]); // 0 구매가격

//if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[0];
//} // if
} // else if 소유주가 없는 땅에 도착

// 8. 플레이어가 위치 한 곳이 도시 이름일 경우, 자신이 소유한 땅에 도착 했을때
else if(!strcmp(city[player[a].position].owner, player[a].name)) { // 자신이 소유한 땅에 도착하면
printf("도시 [%s]는 당신의 도시입니다.\n", city[player[a].position].name);

if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구입 한 상태라면
printf("[%d만원]을 지불하고, 주택을 세우시겠습니까?\n", city[player[a].position].development[1]);

//if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[1];
//} // 선택지
} // 발전도가 1이면, 도시만 구입한 상태라면

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 상태라면
printf("[%d만원]을 지불하고, 빌라를 세우시겠습니까?\n", city[player[a].position].development[2]);

//if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[2];
// } // 선택지
} // 발전도가 2, 주택을 세운 상태라면

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 상태라면
printf("[%d만원]을 지불하고, 아파트를 세우시겠습니까?\n", city[player[a].position].development[3]);

// if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[3];
// } // 선택지
} // 발전도가 3, 빌라를 세운 상태라면

else if(city[player[a].position].development_level == 4) { // 발전도 만랩, 건물 못세움
printf("도시 [%s]는 더이상 건물을 세우지 못합니다.\n", city[player[a].position].name);
} // 발전도가 4, 만랩 건물 못세움
} // else if 자신이 소유한 땅에 도착


// 9. 플레이어가 위치 한 곳이 도시 이름일 경우, 소유주가 남인 땅에 도착 했을때, 통행료를 납부해야한다.
else { // else 소유주가 남인 땅에 도착, 통행료 납부

if(a == 1)
printf("[%s]님의 도시입니다.\n", city[player[1].position].owner);
else
printf("[%s]님의 도시입니다\n.", city[player[0].position].owner);

printf("지나가기 위해서는 통행료를 납부해야합니다.\n");

int pass_price; // 통행료
if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구매한 상태 일 경우
pass_price = city[player[a].position].development[0]*0.5; // 통행료로 도시 가격의 0.5배를 냄
}// 발전도가 1, 도시만 구매한 상태 일 경우

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배를 냄
}

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2 
+ city[player[a].position].development[2]*0.3; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배 + 빌라가격의 0.3를 냄
}

else if(city[player[a].position].development_level == 4) { // 발전도가 4, 아파트를 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2 
+ city[player[a].position].development[2]*0.3 + city[player[a].position].development[3]*0.4; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배 + 빌라가격의 0.3 + 아파트가격의 0.4배를 냄
} // 발전도가 3, 빌라를 세운 경우

printf("통행료 [%d만원]을 납부합니다.\n", pass_price);

player[a].money -= pass_price;
if(a == 0) // 플레이어1인 경우
player[1].money += pass_price; // 플레이어 1에게 통행료가 들어와야함 
else
player[0].money += pass_price;
// printf("[%s]님의 도시입니다.\n", city[player[1].position].owner);

mna(a); // 인수
} // else 소유주가 남인 땅에 도착
} // void trade(int a)



void player_state_update() { // 플레이어의 잔고, 보유 건물 수 업데이트하는 함수
int i;
for (i=0 ; i<PLAYER ; i++) {
if(i==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(i==1)
RED;

printf("%s\n", player[i].icon);
printf("[%s]님 정보\n", player[i].name);
printf("밟고 있는 도시 이름 : [%s]\n", city[player[i].position].name);
printf("소지금 : [%d만원]\n", player[i].money);

int j; // 도시 돌림
int city_count = 0;
for(j=0 ; j<BOARD ; j++) {
if(i == 0) { // 플레이어 1의 소유 도시 수 세기
if(!strcmp(city[j].owner, player[0].name))
city_count++;
} // if(a==0)

else {
if(!strcmp(city[j].owner, player[1].name))
city_count++;
}
} // for BOARD의 수 만큼 for문 돌림
printf("소유 도시 갯수 : %d개\n", city_count);
} // for 플레이어 수
} // void player_state_update() 



void bankruptcy() { // 파산 검사, 도시 매각 함수
BLUE; // 공지

int count = 0;

int j; // 플레이어 수 for
for (j=0 ; j<PLAYER ; j++) {
if (player[j].money < 0) {
// 공지위치
printf("[%s]님은 파산 위기에 쳐했습니다.\n", player[j].name);
printf("소유중인 도시를 처분합니다.\n");

int i; // 도시 수 for
for(i=0 ; i<BOARD ; i++) { // 플레이어의 소유 도시 파악
if(!strcmp(city[i].owner, player[j].name)) {
count++; // 자신이 소유한 도시를 세서 카운트
printf("[%s]를 처분하시겠습니까?\n", city[i].name);

// yes_or_no 선택해서
sleep(1);
int back_price;
if(city[i].development_level == 1) { // 도시의 발전도가 1이라면
back_price = city[i].development[0]*0.9;
} // if 도시의 발전도가 1이라면

else if(city[i].development_level == 2) { // 도시의 발전도가 2이라면, 주택까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9;
}

else if(city[i].development_level == 3) { // 도시의 발전도가 3이라면, 빌라까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9 + city[i].development[2]*0.9;
}

else if(city[i].development_level == 4) { // 도시의 발전도가 4이라면, 아파트까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9 + city[i].development[2]*0.9 + city[i].development[3]*0.9;
}

player[j].money += back_price;
//gotoxy(플레이어가 소지한 도시의 발전도 마크가 뜨는데로 가서)
// city[i].buliding_x, city[i].buliding_y
// printf("          \n");
strcpy(city[i].owner, "없음");
city[i].development_level = 0;
} // if 자신이 소유한 도시를 세서 카운트
} // for 플레이어의 소유 도시 파악
} // if
} // for
} // void bankruptcy() 



void mna(int a) { // 도시 인수
int mna_price;

if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구매한 상태 일 경우
mna_price = city[player[a].position].development[0]*1.1; // 인수가격으로 도시 가격의 1.1배를 냄
}// 인수가격으로 도시가격*1.1

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 경우
mna_price = city[player[a].position].development[0]*1.1 + city[player[a].position].development[1]*1.1; 
// 인수가격으로 (도시가격 + 주택가격)1*1
}

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 경우
mna_price = city[player[a].position].development[0]*1.1 + city[player[a].position].development[1]*1.1 + city[player[a].position].development[2]*1.1; 
// 인수가격으로 (도시 가격+ 주택가격 + 빌라가격)* 1.1
} // 인수가격 정의에 대한 if문 끝

if(player[a].money >= mna_price && city[player[a].position].development_level < 4) { 
// 소지금액이 인수 금액보다 많고 발전도가 4가 아니면

BLUE;
printf("[%s]님의 도시 [%s]를 인수 할 수 있습니다.\n",city[player[a].position].owner, city[player[a].position].name);
printf("[%d만원]을 주고 [%s]를 인수하시겠습니까?", mna_price, city[player[a].position].name);

//if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name);
development_mark(a);

if(a == 0) { // 플레이어 1이면
player[1].money += mna_price;
player[0].money -= mna_price;
} // if 플레이어 1

else { // 플레이어2
player[1].money -= mna_price;
player[0].money += mna_price;
} // 플레이어 2이면
//} // if yes_or_no(a)
} // if 소지금액이 인수 금액보다 많고 발전도가 4가 아니면
} // void mna(int a)


void island(int a){ // island // 플레이어의 위치가 island일때 호출됨
if (player[a].rest >= 1) {
printf("[%s]님은 무인도에 갖혔습니다.\n", player[a].name);
printf("주사위를 두 번 던져서 같은 수가 나오면,\n무인도를 탈출 할 수 있습니다.\n");

int island_dice1, island_dice2;
island_dice1 = dice_cast();
island_dice2 = dice_cast();

printf("첫 번째 주사위 : %d\n", island_dice1);
printf("두 번째 주사위 : %d\n", island_dice2);

if (island_dice1 == island_dice2) {
player[a].rest = 0;
printf("[%s]님이 무인도를 탈출하셨습니다!\n", player[a].name);
}// if island_dice1 == island_dice2

else
player[a].rest--;
printf("[%d턴] 남았습니다.\n", player[a].rest);
}// if(player[a].rest >= 1)
} // void island(int a)



/*
// 이건 나중에 해보자
int yes_or_no() {

} // int yes_or_no(int a)

*/



void opening() {
printf("***************************\n");
printf("시스템 프로그래밍 팀 프로젝트\n");
printf("        주제 : BLUE MARBLE\n");
printf("           20143270 안소정\n");
printf("           20143248 김다슬\n");
printf("***************************\n");
} // void opening()



void ending(int a) {
system("cls");

printf("[%s]님이 승리하셨습니다!\n", player[a].name);
printf("재산 : [%d만원]!\n", player[a].money);

int i; // 도시 세는 함수
int city_count;
for(i=0 ; i<BOARD ; i++) {
if(a == 0) { // 플레이어 1가 승리
if(!strcmp(city[i].owner, player[0].name))
city_count++;
} // if(a==0)

else {
if(!strcmp(city[i].owner, player[1].name))
city_count++;
} // else 플레이어 2가 승리
}
printf("소유한 도시 : %d개\n", city_count);
//printf("** 소유 도시 목록 **\n");
// 여건되면 도시 뭐를 소유했는지도 올리기
/*
int j;// if a가 뭐무녀을 넣어야하나....
for(j=0 ; i<BOARD ; i++) {
if(!strcmp(city[i].owner, player[a].name))
city_count++;
}*/
}


void main() { // 메인
BLUE;
int i = 0;
opening();
BLUE;
printf("********** 시작할 때 이름을 입력 받음 **********\n");
insert_playername(i);

BLUE;
printf("\n********** 플레이어 1과 2의 초기 셋팅을 함 **********\n");
player_state();

while(1) {
BLUE;
printf("\n********** 플레이어 1과 2의 상태 출력 **********\n");
player_state_update();

BLUE;
printf("\n********** 주사위를 돌리고, 말판위의 말을 움직임(지금 말판 움직이는거 x) **********\n");
icon_movement(i);

BLUE;
printf("\n********** 플레이어 1과 2의 주사위를 돌린 후 상태 출력 **********\n");
player_state_update();

BLUE;
printf("\n********** 거래중 **********\n");
trade(i);

BLUE;
printf("\n********** 거래 후 파산검사중 **********\n");
bankruptcy(i);

BLUE;
printf("\n********** 현재 말판 위의 도시 소유주와 발전도 출력 **********\n");
print_information(i);

BLUE;
printf("\n\n********** 5초 후에 자동으로 턴이 바뀝니다. **********\n\n");
sleep(5);

if(i==1) // 순서 바꾸기
i=0;
else
i++;
} // while(1) -> 무한루프

}

void print_information(int a) { // 도시이름 도시소유자 도시발전도 사회복지기금
int i;
for(i=0 ; i<BOARD ; i++) {
printf("[%s]\n", city[i].name);
printf("도시 소유자 : %s\n", city[i].owner);
printf("도시 발전도 : %d\n", city[i].development_level);
} //for(i=0 ; i<BOARD ; i++)
printf("여러분의 열화와 같은 성원에 모금한 사회복지기금 : %d만원\n\n", welfare);
} // city_information(int a)
// ㅅ누서 랜덤으로 정하는거
// ㅅ예 아니요 선택하는거
// 파산검사후 엔딩
// 그 뭐냐 무인도 어디에 있어야하지 ㅅㅂ..
// 메인


