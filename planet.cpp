#include<stdio.h>
#include<graphics.h>
#include <conio.h>
#include<time.h>
#pragma comment( lib, "MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")
enum {
	WIDTH = 1080,
	HEIGHT = 793,
	BULLET_NUM = 15,
	ENEMY_NUM = 7,
	Normal_monsters,
	Elite_monsters,
	Boss,
	ENEMY_BULLET_NUM = 15
};
struct plane {
	double x;
	double y;
	bool live;
	int width;
	int height;
	int hp;
	int type;
}Character_state, BULLET[BULLET_NUM], enemy[ENEMY_NUM],ENEMY_BULLET[ENEMY_BULLET_NUM];
int score = 0;
int Bullet_Speed = 500;
int kill_enemy_num = 0;
IMAGE background;
IMAGE Character;
IMAGE BengBeng;
IMAGE Enemy[3];
IMAGE Enemy_Bullet[3];
IMAGE Character_HP;
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg);//贴图函数
void loadimg();//加载图片
void printimg();//输出图片
void title();//显示分数
void gameInit();//初始化游戏
void creatBULLET();//创建角色子弹
void BULLET_MOVE();//角色子弹移动
void Character_Move(double speed);//角色移动&&子弹延迟
void enemyHP(int i);//初始化敌机信息
void Creat_Enemy();//创建敌人
void Enemy_MOVE(double speed);//敌人移动
bool timer(int ms);//敌人延时出现
void Hit_enemy();//攻击敌人
void Extra_points(int i);//加分政策
void judge_HP();//判断血量
void Hit_Character();//敌人攻击自己
void music();//加载音乐
void enemy_bullet_create();//敌人子弹生成
void enemy_bullet_move();//敌人子弹移动
void judge_bullet_speed();//增益效果判断(根据分数提高生命/速度)
int main() {
	gameInit();
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	BeginBatchDraw();//制造图片缓存区
	music();
	mciSendString(_T("play bkmusic"), NULL, 0, NULL);
	mciSendString(_T("play klee"), NULL, 0, NULL);
	while (1) {
		printimg();
		FlushBatchDraw();
		printimg();
		Character_Move(0.5);
		judge_bullet_speed();
		BULLET_MOVE();
		if (timer(500))
			Creat_Enemy();
		Enemy_MOVE(0.1);
		Hit_enemy();
		Hit_Character();
		enemy_bullet_move();
	}
	EndBatchDraw();
	return 0;
}

void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //新版png
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
void loadimg() {
	loadimage(&background, "./Genshin Impact images/background.png");
	loadimage(&Character_HP, "./Genshin Impact images/HP.png");
	loadimage(&Character, "./Genshin Impact images/klee.png");
	loadimage(&BengBeng, "./Genshin Impact images/bengbeng.png");
	loadimage(&Enemy[0], "./Genshin Impact images/enemy_1.png");
	loadimage(&Enemy[1], "./Genshin Impact images/enemy.png");
	loadimage(&Enemy[2], "./Genshin Impact images/enemy_3.png");
	loadimage(&Enemy_Bullet[0], "./Genshin Impact images/enemy_bullet_1.png");
	loadimage(&Enemy_Bullet[1], "./Genshin Impact images/enemy_bullet_2.png");
	loadimage(&Enemy_Bullet[2], "./Genshin Impact images/enemy_bullet_3.png");
}
void printimg() {
	putimage(0, 0, &background);//背景贴图
	title();//分数贴图
	if (Character_state.live)
		transparentimage(NULL, Character_state.x, Character_state.y, &Character);
	else
		return ;//角色贴图
	judge_HP();//角色生命贴图
	for (int i = 0; i < BULLET_NUM; i++) {
		if (BULLET[i].live) {
			transparentimage(NULL, BULLET[i].x, BULLET[i].y, &BengBeng);
		}
	}//我方子弹贴图
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (enemy[i].live) {
			if (enemy[i].type == Boss) {
				transparentimage(NULL, enemy[i].x, enemy[i].y, &Enemy[2]);
			}
			else if (enemy[i].type == Elite_monsters) {
				transparentimage(NULL, enemy[i].x, enemy[i].y, &Enemy[1]);
			}
			else {
				transparentimage(NULL, enemy[i].x, enemy[i].y, &Enemy[0]);
			}
		}
	}//敌方贴图
	for (int i = 0; i < BULLET_NUM; i++) {
		if (ENEMY_BULLET[i].live) {
			if(enemy[i].type==Normal_monsters)
			transparentimage(NULL, ENEMY_BULLET[i].x, ENEMY_BULLET[i].y, &Enemy_Bullet[0]);
			if (enemy[i].type == Elite_monsters)
				transparentimage(NULL, ENEMY_BULLET[i].x, ENEMY_BULLET[i].y, &Enemy_Bullet[1]);
			if (enemy[i].type == Boss)
				transparentimage(NULL, ENEMY_BULLET[i].x, ENEMY_BULLET[i].y, &Enemy_Bullet[2]);
		}
	}//敌方子弹贴图
}
void gameInit() {
	loadimg();
	Character_state.x = WIDTH / 2;
	Character_state.y = HEIGHT - 140;
	Character_state.live = true;
	Character_state.hp = 3;
	//初始化人物
	for (int i = 0; i < BULLET_NUM; i++) {
		BULLET[i].live = false;
		BULLET[i].x = 0;
		BULLET[i].y = 0;
	}
	//初始化人物子弹
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].live = false;
	}//初始化敌人
	for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
		ENEMY_BULLET[i].live = false;
		ENEMY_BULLET[i].x = -100;
		ENEMY_BULLET[i].y = 0;
	}//初始化敌人子弹
}
void creatBULLET() {
	for (int i = 0; i < BULLET_NUM; i++) {
		if (!BULLET[i].live) {
			BULLET[i].live = true;
			BULLET[i].x = Character_state.x + 15;
			BULLET[i].y = Character_state.y - 25;
			break;//使子弹可以多发
		}
	}//给每个子弹赋位置
}
void BULLET_MOVE() {
	for (int i = 0; i < BULLET_NUM; i++) {
		if (BULLET[i].live) {
			BULLET[i].y -= 1;
			if (BULLET[i].y < 0) {
				BULLET[i].live = false;
			}
		}
	}//子弹以1像素的速度移动，如果超过屏幕则判定死亡
}
void Character_Move(double speed) {
#if 0 {
	char key = _getch();
	switch (key)
	{
	case 'W':
	case 'w':
		Character_state.y -= speed;
		break;
	case 'S':
	case 's':
		Character_state.y += speed;
		break;
	case 'A':
	case 'a':
		Character_state.x -= speed;
		break;
	case 'D':
	case 'd':
		Character_state.x += speed;
		break;

	default:
		break;
	}
	}
//一卡一卡的运动设计
#endif 1{
if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {
	if (Character_state.y > 0)
		Character_state.y -= speed;
}
if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
	if (Character_state.y + 140 < HEIGHT)
		Character_state.y += speed;
}
if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
	if (Character_state.x + 32 > 0)
		Character_state.x -= speed;
}
if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
	if (Character_state.x + 32 < WIDTH)
		Character_state.x += speed;
}
//流畅的运动
static DWORD t1 = 0, t2 = 0;
if (GetAsyncKeyState(VK_SPACE) && t2 - t1 > Bullet_Speed) {//50替换为int值(可以考虑增益来加快子弹速度)
	creatBULLET();
	t1 = t2;
}
t2 = GetTickCount();
//根据空格发射子弹，通过计时器间隔控制子弹速度
}
	void enemy_bullet_create() {
		for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
			if (!ENEMY_BULLET[i].live) {
				ENEMY_BULLET[i].live = true;
				ENEMY_BULLET[i].x = enemy[i].x + 15;
				ENEMY_BULLET[i].y = enemy[i].y - 25;
			}
		}//同角色
	}
	void enemy_bullet_move() {
		double f = rand() % 10;
		f /= 10;
		for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
			if (ENEMY_BULLET[i].live) {
				if (enemy[i].type == Normal_monsters)
					ENEMY_BULLET[i].y += f;
				if (enemy[i].type == Elite_monsters)
					ENEMY_BULLET[i].y += f+0.1;
				if (enemy[i].type == Boss)
					ENEMY_BULLET[i].y += f+0.3;
				if (ENEMY_BULLET[i].y > 800) {
					ENEMY_BULLET[i].live = false;
				}
			}
		}//同角色，其他的让敌方的子弹随机移动0-1像素，精英怪子弹速度更快
	}
	void enemyHP(int i) {
		if (rand() % 100 == 0) {
			enemy[i].type = Boss;
			enemy[i].hp = 30;
			enemy[i].width = 172;
			enemy[i].height = 153;
			ENEMY_BULLET[i].width = 69;
			ENEMY_BULLET[i].height = 73;
		}
		else if (rand() % 10 == 0) {
			enemy[i].type = Elite_monsters;
			enemy[i].hp = 5;
			enemy[i].width = 144;
			enemy[i].height = 206;
			ENEMY_BULLET[i].width = 106;
			ENEMY_BULLET[i].height = 51;
		}
		else {
			enemy[i].type = Normal_monsters;
			enemy[i].hp = 1;
			enemy[i].width = 83;
			enemy[i].height = 143;
			ENEMY_BULLET[i].width = 12;
			ENEMY_BULLET[i].height = 43;
		}
		//初始化敌人的属性以及子弹的属性
	}
	void Creat_Enemy() {
		int num = rand() % 1000;
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (!enemy[i].live) {
				enemy[i].live = true;
				enemy[i].x = num;
				enemy[i].y = 0;
				enemyHP(i);
				break;
			}
		}//随机位置创建敌人
	}
	void Enemy_MOVE(double speed) {
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].live) {
				enemy[i].y += speed;
				if (enemy[i].y > HEIGHT) {
					enemy[i].live = false;
				}
			}//敌人移动的速度，超过边界判断死亡
		}
		double move = rand() % 10;
		move = move / 100;
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].x <= 100 && enemy[i].x >= 0)
				enemy[i].x += move;
			if (enemy[i].x >= 100 && enemy[i].x <= 200)
				enemy[i].x -= move;
			if (enemy[i].x >= 500 && enemy[i].x <= 700)
				enemy[i].x -= move;
			if (enemy[i].x >= 900 && enemy[i].x <= 1080)
				enemy[i].x += move;
		}
		//使敌人左右移动
		static DWORD t1 = 0, t2 = 0;
		if (t2 - t1 > 1000) {
			enemy_bullet_create();
			t1 = t2;
		}
		t2 = GetTickCount();
		//间隔创建敌人子弹
	}
	bool timer(int ms) {
		static DWORD t1, t2;
		if (t2 - t1 > 500) {
			t1 = t2;
			return true;
		}
		t2 = clock();
		return false;
		//计时器
	}
	void Hit_enemy() {
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (!enemy[i].live)
				continue;
			for (int j = 0; j < BULLET_NUM; j++) {
				if (!BULLET[j].live)
					continue;
				if (BULLET[j].x > enemy[i].x && BULLET[j].x < enemy[i].x + enemy[i].width && BULLET[j].y>enemy[i].y && BULLET[j].y < enemy[i].y + enemy[i].height) {
					BULLET[j].live = false;
					enemy[i].hp--;
				}
			}
			if (enemy[i].hp <= 0) {
				enemy[i].live = false;
				ENEMY_BULLET[i].live = false;
				Extra_points(i);
			}
			//攻击敌人并加分
		}
	}
	void Hit_Character() {
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].live) {
				if (Character_state.x + 64 > enemy[i].x && Character_state.x < enemy[i].x + enemy[i].width && Character_state.y + 140>enemy[i].y && Character_state.y < enemy[i].y + enemy[i].height) {
					Character_state.hp--;
					enemy[i].hp -= 10;
					//enemy[i].y = 0;
					mciSendString(_T("play hurt"), NULL, 0, NULL);
				}
			}
			if (ENEMY_BULLET[i].live) {
				if (Character_state.x + 64 > ENEMY_BULLET[i].x && Character_state.x < ENEMY_BULLET[i].x + ENEMY_BULLET[i].width && Character_state.y + 140>ENEMY_BULLET[i].y && Character_state.y < ENEMY_BULLET[i].y + ENEMY_BULLET[i].height) {
					Character_state.hp--;
					enemy[i].hp -= 10;
					//enemy[i].y = 0;
					ENEMY_BULLET[i].live = false;
					//ENEMY_BULLET[i].y = 0;
					mciSendString(_T("play hurt"), NULL, 0, NULL);
				}
			}
			if (Character_state.hp <= 0) {
				Character_state.live = false;
				mciSendString(_T("stop hurt"), NULL, 0, NULL);
				mciSendString(_T("stop bkmusic"), NULL, 0, NULL);
				mciSendString(_T("play die"), NULL, 0, NULL);
			}
			//判断角色是否接触敌人子弹/敌人,否则HP-1
		}
	}
	void title() {
		char str[] = { "Score:" };
		settextstyle(20, 0, "楷体");
		outtextxy(10, 10, str);
		char num[20];
		sprintf(num, "%d", score);
		outtextxy(70, 10, num);
		if(Character_state.live==false){
			settextstyle(100, 0, "楷体");
			char a[] = { "Game Over" };
			outtextxy(0, 0, a);
		}
		//输出分数
	}
	void Extra_points(int i) {
		if (enemy[i].type == Normal_monsters) {
			score++;
			kill_enemy_num++;
		}
		if (enemy[i].type == Elite_monsters) {
			score += 5;
			kill_enemy_num++;
		}
		if (enemy[i].type == Boss) {
			score += 50;
			kill_enemy_num++;
		}
		//计算打死不同怪的分数
	}
	void judge_HP() {
		if (Character_state.hp == 3) {
			transparentimage(NULL, Character_state.x + 10, Character_state.y - 8, &Character_HP);
			transparentimage(NULL, Character_state.x + 25, Character_state.y - 8, &Character_HP);
			transparentimage(NULL, Character_state.x + 40, Character_state.y - 8, &Character_HP);
		}
		if (Character_state.hp == 2) {
			transparentimage(NULL, Character_state.x + 10, Character_state.y - 8, &Character_HP);
			transparentimage(NULL, Character_state.x + 25, Character_state.y - 8, &Character_HP);
		}
		if (Character_state.hp == 1) {
			transparentimage(NULL, Character_state.x + 10, Character_state.y - 8, &Character_HP);
		}
		//显示血量
	}
	void music() {
		mciSendString(_T("open die.mp3 alias die"), NULL, 0, NULL);
		mciSendString(_T("open hurt.mp3 alias hurt"), NULL, 0, NULL);
		mciSendString(_T("open fight.mp3 alias bkmusic"), NULL, 0, NULL);
		mciSendString(_T("open klee.mp3 alias klee"), NULL, 0, NULL);
		//加载音乐
	}
	void judge_bullet_speed() {
		if (score >= 10 && score <= 20) {
			Bullet_Speed = 300;
		}
		if (score >= 20 && score <= 50) {
			Bullet_Speed = 200;
		}
		if (score >= 50 && score <= 100) {
			Bullet_Speed = 100;
		}
		if (score >= 100 && score <= 500) {
			Bullet_Speed = 50;
		}if (score >= 500) {
			Bullet_Speed = 50;
		}
		if (kill_enemy_num % 50 == 0 && Character_state.hp < 3)
			Character_state.hp++;
		//根据分数不同获得增益
	}
