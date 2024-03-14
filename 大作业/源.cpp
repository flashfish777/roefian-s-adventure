#include<graphics.h>
#include<string>
#include<vector>
#include<math.h>
#include<iostream>
#include<stdlib.h>

//地图链表的节点
typedef struct map {
	int number;
	struct map* up;
	struct map* down;
	struct map* left;
	struct map* right;
	struct map* next;
	LPCTSTR background;
	int enemy1, enemy2, enemy3, enemy4;
	int enemy1a, enemy2a, enemy3a, enemy4a, boss1a, boss2a;
	bool aword;
	POINT enemy1_pos, enemy2_pos, enemy3_pos;
}mapnode;

const int WINDOW_WIDTH = 1700;//窗口宽度
const int WINDOW_HEIGHT = 1000;//窗口高度

const int BUTTON_WIDTH_START = 250;
const int BUTTON_HEIGHT_START = 50;
const int BUTTON_WIDTH_QUIT = 30;
const int BUTTON_HEIGHT_QUIT = 30;

int player1_red = 440;
int player2_red = 440;
int player1_blue = 290;
int player2_blue = 290;

//音乐
#pragma comment(lib, "Winmm.lib")

//玩家透明
#pragma comment(lib, "MSIMG32.LIB")

bool running = true;
bool game = true;
bool is_game_started = false;
bool is_choose_start = false;
bool is_achievement = false;

bool is_1_alive = true;
bool is_2_alive = true;

int boss1xxx = 1;
int boss2xxx = 1;

bool pause = false;

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 225, AC_SRC_ALPHA });
}

//动画所使用的图集
class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Atlas()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i];
	}

public:
	std::vector<IMAGE*> frame_list;
};

Atlas* atlas_player1_left;
Atlas* atlas_player2_left;
Atlas* atlas_player1_right;
Atlas* atlas_player2_right;
Atlas* atlas_player1_left_st;
Atlas* atlas_player2_left_st;
Atlas* atlas_player1_right_st;
Atlas* atlas_player2_right_st;
Atlas* atlas_enemy1_left;
Atlas* atlas_enemy2_left;
Atlas* atlas_enemy1_right;
Atlas* atlas_enemy2_right;
Atlas* atlas_enemy3_left;
Atlas* atlas_enemy3_right;
Atlas* atlas_boss1_left;
Atlas* atlas_boss1_right;
Atlas* atlas_boss2;
Atlas* atlas_bullet_left;
Atlas* atlas_bullet_right;
Atlas* atlas_enemybullet;

Atlas* atlas_botton;

//处理个体动画的类
class Animation
{
public:
	Animation(Atlas* atlas, int interval)
	{
		anim_atlas = atlas;
		interval_ms = interval;
	}

	~Animation() = default;

	void Play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}

private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	int interval_base = 0;

private:
	Atlas* anim_atlas;
};

//玩家
class Player
{
public:
	const int PLAYER1_WIDTH = 60;//玩家宽高
	const int PLAYER1_HEIGHT = 65;
	const int PLAYER2_WIDTH = 60;
	const int PLAYER2_HEIGHT = 65;
	bool facing_left1 = false;
	bool facing_left2 = false;
	bool player1_alive = true;
	bool player2_alive = true;
	POINT player1_pos = { 200, 300 };//玩家出生位置
	POINT player2_pos = { 200, 300 };
	bool is_move_up = false;//玩家移动规则
	bool is_move_down = true;
	bool is_move_left = false;
	bool is_move_right = false;
	bool is_move_w = false;
	bool is_move_s = true;
	bool is_move_a = false;
	bool is_move_d = false;

public:
	Player()
	{
		anim_left_player1_st = new Animation(atlas_player1_left_st, 45);
		anim_right_player1_st = new Animation(atlas_player1_right_st, 45);
		anim_left_player2_st = new Animation(atlas_player2_left_st, 45);
		anim_right_player2_st = new Animation(atlas_player2_right_st, 45);
		anim_left_player1 = new Animation (atlas_player1_left, 45);
		anim_right_player1 = new Animation (atlas_player1_right, 45);
		anim_left_player2 = new Animation (atlas_player2_left, 45);
		anim_right_player2 = new Animation (atlas_player2_right, 45);
	}

	~Player()
	{
		delete anim_left_player1_st;
		delete anim_right_player1_st;
		delete anim_left_player2_st;
		delete anim_right_player2_st;
		delete anim_left_player1;
		delete anim_right_player1;
		delete anim_left_player2;
		delete anim_right_player2;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				if (player2_alive)
				{
					if (is_move_up == false && is_move_down == false)
					{
						player2_now = player2_pos;
						is_move_down = false;
						is_move_up = true;
					}
				}
				break;
			case VK_LEFT:
				if (player2_alive)
					is_move_left = true;
				break;
			case VK_RIGHT:
				if (player2_alive)
					is_move_right = true;
				break;
			case 0x57:
				if (player1_alive)
				{
					if (is_move_w == false && is_move_s == false)
					{
						player1_now = player1_pos;
						is_move_s = false;
						is_move_w = true;
					}
				}
				break;
			case 0x41:
				if (player1_alive)
					is_move_a = true;
				break;
			case 0x44:
				if (player1_alive)
					is_move_d = true;
				break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			case 0x41:
				is_move_a = false;
				break;
			case 0x44:
				is_move_d = false;
				break;
			}
			break;
		}
	}

	void Move()
	{
		if (player2_alive)
		{
			if (is_move_up) player2_pos.y -= jumping_speed2;
			if (player2_pos.y == player2_now.y - 105)
			{
				jumping_speed2 = 10;
				fall_speed2 = 15;
			}
			if (player2_pos.y == player2_now.y - 255)
			{
				jumping_speed2 = 5;
				fall_speed2 = 10;
			}
			if (player2_pos.y == player2_now.y - 300)
			{
				is_move_up = false;
				is_move_down = true;
				jumping_speed2 = 15;
				fall_speed2 = 5;
			}
			if (is_move_down) player2_pos.y += fall_speed2;
			if (is_move_left) player2_pos.x -= PLAYER_SPEED;
			if (is_move_right) player2_pos.x += PLAYER_SPEED;
		}
		if (player1_alive)
		{
			if (is_move_w) player1_pos.y -= jumping_speed1;
			if (player1_pos.y == player1_now.y - 105)
			{
				jumping_speed1 = 10;
				fall_speed1 = 15;
			}
			if (player1_pos.y == player1_now.y - 255)
			{
				jumping_speed1 = 5;
				fall_speed1 = 10;
			}
			if (player1_pos.y == player1_now.y - 300)
			{
				is_move_w = false;
				is_move_s = true;
				jumping_speed1 = 15;
				fall_speed1 = 5;
			}
			if (is_move_s) player1_pos.y += fall_speed1;
			if (is_move_a) player1_pos.x -= PLAYER_SPEED;
			if (is_move_d) player1_pos.x += PLAYER_SPEED;
		}
		//玩家不能超出窗口边界
		if (player1_pos.x < 0) player1_pos.x = 0;
		if (player1_pos.y < 0) player1_pos.y = 0;
		if (player2_pos.x < 0) player2_pos.x = 0;
		if (player2_pos.y < 0) player2_pos.y = 0;
		if (player1_pos.x + PLAYER1_WIDTH > WINDOW_WIDTH) player1_pos.x = WINDOW_WIDTH - PLAYER1_WIDTH;
		if (player1_pos.y + PLAYER1_HEIGHT > WINDOW_HEIGHT) player1_pos.y = WINDOW_HEIGHT - PLAYER1_HEIGHT;
		if (player2_pos.x + PLAYER2_WIDTH > WINDOW_WIDTH) player2_pos.x = WINDOW_WIDTH - PLAYER2_WIDTH;
		if (player2_pos.y + PLAYER2_HEIGHT > WINDOW_HEIGHT) player2_pos.y = WINDOW_HEIGHT - PLAYER2_HEIGHT;
	}

	void Draw1(int delta)
	{
		int dir_x = is_move_d - is_move_a;
		if (dir_x < 0)
			facing_left1 = true;
		else if (dir_x > 0)
			facing_left1 = false;

		if (facing_left1) 
		{
			if (dir_x == 0) anim_left_player1_st->Play(player1_pos.x, player1_pos.y, delta);
			else anim_left_player1->Play(player1_pos.x, player1_pos.y, delta);
		}	
		else
		{
			if (dir_x == 0) anim_right_player1_st->Play(player1_pos.x, player1_pos.y, delta);
			else anim_right_player1->Play(player1_pos.x, player1_pos.y, delta);
		}	
	}

	void Draw2(int delta)
	{
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left2 = true;
		else if (dir_x > 0)
			facing_left2 = false;
		
		if (facing_left2)
		{
			if (dir_x == 0) anim_left_player2_st->Play(player2_pos.x, player2_pos.y, delta);
			else anim_left_player2->Play(player2_pos.x, player2_pos.y, delta);
		}
		else
		{
			if (dir_x == 0) anim_right_player2_st->Play(player2_pos.x, player2_pos.y, delta);
			else anim_right_player2->Play(player2_pos.x, player2_pos.y, delta);
		}
	}

	const POINT& GetPosition1() const
	{
		return player1_pos;
	}

	const POINT& GetPosition2() const
	{
		return player2_pos;
	}

private:
	const int PLAYER_SPEED = 5;//玩家行走速度
	int jumping_speed1 = 15;
	int jumping_speed2 = 15;
	int fall_speed1 = 5;
	int fall_speed2 = 5;

private:
	Animation* anim_left_player1;
	Animation* anim_right_player1;
	Animation* anim_left_player2;
	Animation* anim_right_player2;
	Animation* anim_left_player1_st;
	Animation* anim_right_player1_st;
	Animation* anim_left_player2_st;
	Animation* anim_right_player2_st;
	POINT player1_now;
	POINT player2_now;
};

//子弹
class Bullet
{
public:
	const int BULLET_WIDTH = 60;
	const int BULLET_HEIGHT = 30;
	POINT bullet_pos;
	POINT bullet_begin;
	bool bullet_facing_left = false;
	bool alive = true;

public:
	Bullet()
	{
		anim_bullet_left = new Animation(atlas_bullet_left, 45);
		anim_bullet_right = new Animation(atlas_bullet_right, 45);	
	}
	~Bullet()
	{
		delete anim_bullet_left;
		delete anim_bullet_right;
	}

	void Move(const Player& player)
	{
		if (bullet_facing_left) bullet_pos.x -= 10;
		else bullet_pos.x += 10;

		if (bullet_pos.x >= bullet_begin.x + 500) alive = false; 
		if (bullet_pos.x <= bullet_begin.x - 600) alive = false;
		if (bullet_pos.x <= 0) alive = false;
		if (bullet_pos.x >= 1700) alive = false;
	}

	void Draw(int delta)
	{
		if (bullet_facing_left) anim_bullet_left->Play(bullet_pos.x, bullet_pos.y, delta);
		else anim_bullet_right->Play(bullet_pos.x, bullet_pos.y, delta);
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	Animation* anim_bullet_right;
	Animation* anim_bullet_left;
	bool Draw_bullet = false;
};

class EnemyBullet
{
public:
	const int BULLET_WIDTH = 20;
	const int BULLET_HEIGHT = 20;
	POINT bullet_pos;
	POINT bullet_begin;
	POINT end_pos;
	bool boss2 = false;
	bool boss22 = false;
	bool alive = true;
	double dir_x;
	double dir_y;

public:
	EnemyBullet()
	{
		anim_enemybullet = new Animation(atlas_enemybullet, 45);
	}
	~EnemyBullet()
	{
		delete anim_enemybullet;
	}

	void Move()
	{	
		double dir_xx = dir_x;
		double dir_yy = dir_y;
		if (!boss2 && !boss22)
		{
			if (fabs(dir_xx) >= 80 && fabs(dir_yy) >= 80)
			{
				dir_xx = dir_xx / fabs(dir_yy);
				bullet_pos.x += BULLET_SPEED * dir_xx;
			}
			if (fabs(dir_yy) >= 80)
			{
				dir_yy = dir_yy / fabs(dir_yy);
				bullet_pos.y += BULLET_SPEED * dir_yy;
			}
			if (fabs(dir_yy) <= 80 && fabs(dir_xx) >= 80 && fabs(dir_yy))
			{
				dir_xx = dir_xx / fabs(dir_xx);
				bullet_pos.x += BULLET_SPEED * dir_xx;
			}

			if (bullet_pos.x >= bullet_begin.x + 800) alive = false;
			if (bullet_pos.x <= bullet_begin.x - 800) alive = false;
			if (bullet_pos.y >= bullet_begin.y + 600) alive = false;
			if (bullet_pos.y <= bullet_begin.y - 600) alive = false;
			if (bullet_pos.x <= 0) alive = false;
			if (bullet_pos.x >= 1700) alive = false;
			if (bullet_pos.y <= 0) alive = false;
			if (bullet_pos.y >= 1000) alive = false;
		}
		else if (boss2 && !boss22)
		{
			double x = end_pos.x - bullet_begin.x;
			double y = end_pos.y - bullet_begin.y;
			double dir1 = y / x;
			double dir2 = x / y;
			if (end_pos.x == 0 || end_pos.x == 1700)
			{
				bullet_pos.x += BULLET_SPEED_BOSS * (x / fabs(x));
				bullet_pos.y += BULLET_SPEED_BOSS * fabs(dir1) * (y / fabs(y));
				//printf("%f %f\n ", x, y);
			}
			if (end_pos.y == 0 || end_pos.y == 1000)
			{
				
				bullet_pos.y += BULLET_SPEED_BOSS * (y / fabs(y));
				bullet_pos.x += BULLET_SPEED_BOSS * fabs(dir2) * (x / fabs(x));
			}

			if (bullet_pos.x <= 0) alive = false;
			if (bullet_pos.x >= 1700) alive = false;
			if (bullet_pos.y <= 0) alive = false;
			if (bullet_pos.y >= 1000) alive = false;
		}
		else if (boss22)
		{
			bullet_pos.x += BULLET_SPEED_BOSS * dir_x;
			bullet_pos.y += BULLET_SPEED_BOSS * dir_y;

			//printf("%f,    %f\n", BULLET_SPEED_BOSS * dir_x, BULLET_SPEED_BOSS * dir_y);

			if (bullet_pos.x <= 0) alive = false;
			if (bullet_pos.x >= 1700) alive = false;
			if (bullet_pos.y <= 0) alive = false;
			if (bullet_pos.y >= 1000) alive = false;
		}
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT position = { bullet_pos.x + BULLET_WIDTH / 2, bullet_pos.y + BULLET_HEIGHT / 2 };
		bool is_overlap1_x = position.x >= player.GetPosition1().x && position.x <= player.GetPosition1().x + player.PLAYER1_WIDTH;
		bool is_overlap1_y = position.y >= player.GetPosition1().y && position.y <= player.GetPosition1().y + player.PLAYER1_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{
		POINT position = { bullet_pos.x + BULLET_WIDTH / 2, bullet_pos.y + BULLET_HEIGHT / 2 };
		bool is_overlap2_x = position.x >= player.GetPosition2().x && position.x <= player.GetPosition2().x + player.PLAYER2_WIDTH;
		bool is_overlap2_y = position.y >= player.GetPosition2().y && position.y <= player.GetPosition2().y + player.PLAYER2_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
	}

	void Draw(int delta)
	{
		anim_enemybullet->Play(bullet_pos.x, bullet_pos.y, delta);
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	Animation* anim_enemybullet;
	const double BULLET_SPEED_BOSS = 6;
	const double BULLET_SPEED = 2;
};

//敌人
class Enemy1
{
public:
	POINT enemy_pos = { 2000, 590 };

public:
	Enemy1()
	{
		anim_left_enemy = new Animation(atlas_enemy2_left, 45);
		anim_right_enemy = new Animation(atlas_enemy2_right, 45);
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		POINT bulletposition = { bullet.bullet_pos.x + (bullet.BULLET_WIDTH / 2), bullet.bullet_pos.y + (bullet.BULLET_HEIGHT / 2) };
		bool is_overlap_x = bulletposition.x >= enemy_pos.x && bulletposition.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap_y = bulletposition.y >= enemy_pos.y && bulletposition.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player) 
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= enemy_pos.x && player1position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap1_y = player1position.y >= enemy_pos.y && player1position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{
		
		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= enemy_pos.x && player2position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap2_y = player2position.y >= enemy_pos.y && player2position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
	}

	void Move(const Player& player)
	{
		const POINT& player1_position = player.GetPosition1();
		const POINT& player2_position = player.GetPosition2();
		int dir_x1 = player1_position.x - enemy_pos.x;
		int dir_x2 = player2_position.x - enemy_pos.x;
		int dir_y1 = player1_position.y - enemy_pos.y;
		int dir_y2 = player2_position.y - enemy_pos.y;
		int dir_x;
		if (abs(dir_y1) > abs(dir_y2)) dir_x = dir_x2;
		else if (abs(dir_y1) == abs(dir_y2)) if (abs(dir_x1) > abs(dir_x2)) dir_x = dir_x2; else dir_x = dir_x1;
		else dir_x = dir_x1;

		if (dir_x != 0)
		{
			dir_x = dir_x / abs(dir_x);
			enemy_pos.x += (int)(ENEMY_SPEED * dir_x);
		}
		
		if (dir_x < 0)
			enemy_facing_left = true;
		else if (dir_x > 0)
			enemy_facing_left = false;
	}

	void Draw(int delta)
	{
		if (enemy_facing_left)
			anim_left_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
		else 
			anim_right_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
	}

	~Enemy1()
	{
		delete anim_left_enemy;
		delete anim_right_enemy;
	}

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	const int ENEMY_SPEED = 2;//敌人数据
	const int FRAME_WIDTH = 60;
	const int FRAME_HEIGHT = 60;

private:
	Animation* anim_left_enemy;
	Animation* anim_right_enemy;
	bool enemy_facing_left = false;
	bool alive = true;
};

class Enemy2
{
public:
	POINT enemy_pos = { 2000, 590 };

public:
	Enemy2()
	{
		anim_left_enemy = new Animation(atlas_enemy1_left, 45);
		anim_right_enemy = new Animation(atlas_enemy1_right, 45);
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= enemy_pos.x && bullet.bullet_pos.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= enemy_pos.y && bullet.bullet_pos.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player) 
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= enemy_pos.x && player1position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap1_y = player1position.y >= enemy_pos.y && player1position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{
		
		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= enemy_pos.x && player2position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap2_y = player2position.y >= enemy_pos.y && player2position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
		
	}

	void Move(const Player& player)
	{
		const POINT& player1_position = player.GetPosition1();
		const POINT& player2_position = player.GetPosition2();
		int dir_x1 = player1_position.x - enemy_pos.x;
		int dir_x2 = player2_position.x - enemy_pos.x;
		int dir_y1 = player1_position.y - enemy_pos.y;
		int dir_y2 = player2_position.y - enemy_pos.y;
		int dir_x;
		if (abs(dir_y1) > abs(dir_y2)) dir_x = dir_x2;
		else if (abs(dir_y1) == abs(dir_y2)) if (abs(dir_x1) > abs(dir_x2)) dir_x = dir_x2; else dir_x = dir_x1;
		else dir_x = dir_x1;

		if (dir_x != 0)
		{
			dir_x = dir_x / abs(dir_x);
			enemy_pos.x += ENEMY_SPEED * dir_x;
		}

		if (dir_x < 0)
			enemy_facing_left = true;
		else if (dir_x > 0)
			enemy_facing_left = false;
	}

	void Draw(int delta)
	{
		if (enemy_facing_left)
			anim_left_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
		else
			anim_right_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
	}

	~Enemy2()
	{
		delete anim_left_enemy;
		delete anim_right_enemy;
	}

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	const int ENEMY_SPEED = 5;//敌人数据
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 60;

private:
	Animation* anim_left_enemy;
	Animation* anim_right_enemy;
	bool enemy_facing_left = false;
	bool alive = true;
};

class Enemy3
{
public:
	POINT enemy_pos = { 1000 , 0 };

public:
	Enemy3()
	{
		anim_left_enemy = new Animation(atlas_enemy3_left, 45);
		anim_right_enemy = new Animation(atlas_enemy3_right, 45);
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= enemy_pos.x && bullet.bullet_pos.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= enemy_pos.y && bullet.bullet_pos.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= enemy_pos.x && player1position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap1_y = player1position.y >= enemy_pos.y && player1position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{

		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= enemy_pos.x && player2position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap2_y = player2position.y >= enemy_pos.y && player2position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap2_x && is_overlap2_y;

	}

	void Move(const Player& player)
	{
		const POINT& player1_position = player.GetPosition1();
		const POINT& player2_position = player.GetPosition2();
		int dir_x1 = player1_position.x - enemy_pos.x;
		int dir_x2 = player2_position.x - enemy_pos.x;
		int dir_y1 = player1_position.y - enemy_pos.y;
		int dir_y2 = player2_position.y - enemy_pos.y;
		int dir_x;
		int dir_y;
		if (abs(dir_x1) > abs(dir_x2)) dir_x = dir_x2;
		else dir_x = dir_x1;
		if (abs(dir_y1) > abs(dir_y2)) dir_y = dir_y2;
		else dir_y = dir_y1;

		if (dir_x != 0)
		{
			dir_x = dir_x / abs(dir_x);
			enemy_pos.x += ENEMY_SPEED * dir_x;
		}
		if (dir_y != 0)
		{
			dir_y = dir_y / abs(dir_y);
			enemy_pos.y += ENEMY_SPEED * dir_y;
		}

		if (dir_x < 0)
			enemy_facing_left = true;
		else if (dir_x > 0)
			enemy_facing_left = false;
	}

	void Draw(int delta)
	{
		if (enemy_facing_left)
			anim_left_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
		else
			anim_right_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
	}

	~Enemy3()
	{
		delete anim_left_enemy;
		delete anim_right_enemy;
	}

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	const int ENEMY_SPEED = 2;//敌人数据
	const int FRAME_WIDTH = 60;
	const int FRAME_HEIGHT = 65;

private:
	Animation* anim_left_enemy;
	Animation* anim_right_enemy;
	bool enemy_facing_left = false;
	bool alive = true;
};

class Enemy4
{
public:
	POINT enemy_pos = { 900 , 200 };
	std::vector<EnemyBullet*> enemybullet_list;

public:
	Enemy4()
	{
		anim_left_enemy = new Animation(atlas_enemy3_left, 45);
		anim_right_enemy = new Animation(atlas_enemy3_right, 45);
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= enemy_pos.x && bullet.bullet_pos.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= enemy_pos.y && bullet.bullet_pos.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= enemy_pos.x && player1position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap1_y = player1position.y >= enemy_pos.y && player1position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{

		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= enemy_pos.x && player2position.x <= enemy_pos.x + FRAME_WIDTH;
		bool is_overlap2_y = player2position.y >= enemy_pos.y && player2position.y <= enemy_pos.y + FRAME_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
	}

	void Move(const Player& player)
	{
		const int INTERVAL = 217;
		static int counter = 0;
		if ((++counter) % INTERVAL == 0)
		{
			EnemyBullet* bullet = new EnemyBullet();
			bullet->bullet_pos = enemy_pos;
			bullet->bullet_begin = enemy_pos;
			double dir_x1 = player.GetPosition1().x - enemy_pos.x;
			double dir_x2 = player.GetPosition2().x - enemy_pos.x;
			double dir_y1 = player.GetPosition1().y - enemy_pos.y;
			double dir_y2 = player.GetPosition2().y - enemy_pos.y;
			if (fabs(dir_x1) > fabs(dir_x2)) bullet->dir_x = dir_x2;
			else bullet->dir_x = dir_x1;
			if (fabs(dir_y1) > fabs(dir_y2)) bullet->dir_y = dir_y2;
			else bullet->dir_y = dir_y1;
			enemybullet_list.push_back(bullet);
		}

		const POINT& player1_position = player.GetPosition1();
		const POINT& player2_position = player.GetPosition2();
		int dir_x1 = player1_position.x - enemy_pos.x;
		int dir_x2 = player2_position.x - enemy_pos.x;
		int dir_x;
		if (abs(dir_x1) > abs(dir_x2)) dir_x = dir_x2;
		else dir_x = dir_x1;
		if (dir_x < 0)
			enemy_facing_left = true;
		else if (dir_x > 0)
			enemy_facing_left = false;
	}

	void Draw(int delta)
	{
		if (enemy_facing_left)
			anim_left_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
		else
			anim_right_enemy->Play(enemy_pos.x, enemy_pos.y, delta);
	}

	~Enemy4()
	{
		delete anim_left_enemy;
		delete anim_right_enemy;
	}

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	const int ENEMYBULLET_SPEED = 2;//敌人数据
	const int FRAME_WIDTH = 60;
	const int FRAME_HEIGHT = 65;

private:
	Animation* anim_left_enemy;
	Animation* anim_right_enemy;
	bool enemy_facing_left = false;
	bool alive = true; 
};

class Boss1
{
public:
	POINT boss_pos = { -200 , -200 };
	int red = 1600;

public:
	Boss1()
	{
		anim_left_boss = new Animation(atlas_boss1_left, 45);
		anim_right_boss = new Animation(atlas_boss1_right, 45);
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= boss_pos.x && bullet.bullet_pos.x <= boss_pos.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= boss_pos.y && bullet.bullet_pos.y <= boss_pos.y + FRAME_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= boss_pos.x && player1position.x <= boss_pos.x + FRAME_WIDTH;
		bool is_overlap1_y = player1position.y >= boss_pos.y && player1position.y <= boss_pos.y + FRAME_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{

		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= boss_pos.x && player2position.x <= boss_pos.x + FRAME_WIDTH;
		bool is_overlap2_y = player2position.y >= boss_pos.y && player2position.y <= boss_pos.y + FRAME_HEIGHT;
		return is_overlap2_x && is_overlap2_y;

	}

	void Move(const Player& player)
	{
		boss_pos.x += BOSS_SPEED_X;
		boss_pos.y += BOSS_SPEED_Y;
		if (boss_pos.x + FRAME_WIDTH >= 1700 || boss_pos.x + FRAME_WIDTH <= 0) BOSS_SPEED_X *= -1;
		if (boss_pos.y + FRAME_HEIGHT >= 1000 || boss_pos.y + FRAME_HEIGHT <= 0) BOSS_SPEED_Y *= -1;

		const POINT& player1_position = player.GetPosition1();
		const POINT& player2_position = player.GetPosition2();
		int dir_x1 = player1_position.x - boss_pos.x;
		int dir_x2 = player2_position.x - boss_pos.x;
		int dir_x;
		if (abs(dir_x1) > abs(dir_x2)) dir_x = dir_x2;
		else dir_x = dir_x1;

		if (dir_x < 0)
			boss_facing_left = true;
		else if (dir_x > 0)
			boss_facing_left = false;
	}

	void Draw(int delta)
	{
		if (boss_facing_left)
			anim_left_boss->Play(boss_pos.x, boss_pos.y, delta);
		else
			anim_right_boss->Play(boss_pos.x, boss_pos.y, delta);
	}

	~Boss1()
	{
		delete anim_left_boss;
		delete anim_right_boss;
	}

	void Hurt()
	{
		if (red > 500) red -= 50;
		else
		{
			boss1xxx = 0;
			alive = false;
		}
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	int BOSS_SPEED_X = 8;//敌人数据
	int BOSS_SPEED_Y = 8;
	const int FRAME_WIDTH = 200;
	const int FRAME_HEIGHT = 200;

private:
	Animation* anim_left_boss;
	Animation* anim_right_boss;
	bool boss_facing_left = false;
	bool alive = true;
};

class Boss2
{
public:
	POINT boss_pos = { -200 , -200 };
	int red = 1600;
	int yellow = 1600;
	std::vector<EnemyBullet*> boss2_bullet;

public:
	Boss2()
	{
		anim_boss = new Animation(atlas_boss2, 120);
		loadimage(&img_boss2_picture, _T("img/boss2_picture.png"));
	}

	bool CheckBulletCollision1(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= 720 && bullet.bullet_pos.x <=720 + BOSS1_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= 250 && bullet.bullet_pos.y <= 250 + BOSS1_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckBulletCollision2(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.bullet_pos.x >= boss_pos.x && bullet.bullet_pos.x <= boss_pos.x + BOSS2_WIDTH;
		bool is_overlap_y = bullet.bullet_pos.y >= boss_pos.y && bullet.bullet_pos.y <= boss_pos.y + BOSS2_HEIGHT;
		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT player1 = player.GetPosition1();
		POINT player1position = { player1.x + player.PLAYER1_WIDTH / 2, player1.y + player.PLAYER1_HEIGHT / 2 };
		bool is_overlap1_x = player1position.x >= boss_pos.x && player1position.x <= boss_pos.x + BOSS2_WIDTH;
		bool is_overlap1_y = player1position.y >= boss_pos.y + 30 && player1position.y <= boss_pos.y + BOSS2_HEIGHT + 50;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{

		POINT player2 = player.GetPosition2();
		POINT player2position = { player2.x + player.PLAYER2_WIDTH / 2, player2.y + player.PLAYER2_HEIGHT / 2 };
		bool is_overlap2_x = player2position.x >= boss_pos.x && player2position.x <= boss_pos.x + BOSS2_WIDTH;
		bool is_overlap2_y = player2position.y >= boss_pos.y && player2position.y <= boss_pos.y + BOSS2_HEIGHT;
		return is_overlap2_x && is_overlap2_y;

	}

	void Move()
	{
		static int cnt = 0;
		if ((++cnt) % 10 == 0)
		{
			boss2_bullet.push_back(new EnemyBullet());
			boss2_bullet.back()->boss2 = true;
			boss2_bullet.back()->bullet_pos = { 850 , 500 };
			boss2_bullet.back()->bullet_begin = { 850 , 500 };
			if (cnt == 10) boss2_bullet.back()->end_pos = { 210, 1000 };
			else if (cnt == 20) boss2_bullet.back()->end_pos = { 0, 875 };
			else if (cnt == 30) boss2_bullet.back()->end_pos = { 0, 750 };
			else if (cnt == 40) boss2_bullet.back()->end_pos = { 0, 625 };
			else if (cnt == 50) boss2_bullet.back()->end_pos = { 0, 501 };
			else if (cnt == 60) boss2_bullet.back()->end_pos = { 0, 375 };
			else if (cnt == 70) boss2_bullet.back()->end_pos = { 0, 250 };
			else if (cnt == 80) boss2_bullet.back()->end_pos = { 0, 125 };
			else if (cnt == 90) boss2_bullet.back()->end_pos = { 210 , 0 };
			else if (cnt == 100) boss2_bullet.back()->end_pos = { 420 , 0 };
			else if (cnt == 110) boss2_bullet.back()->end_pos = { 630 , 0 };
			else if (cnt == 120) boss2_bullet.back()->end_pos = { 851 , 0 };
			else if (cnt == 130) boss2_bullet.back()->end_pos = { 1060 , 0 };
			else if (cnt == 140) boss2_bullet.back()->end_pos = { 1270 , 0 };
			else if (cnt == 150) boss2_bullet.back()->end_pos = { 1480 , 0 };
			else if (cnt == 160) boss2_bullet.back()->end_pos = { 1700, 125 };
			else if (cnt == 170) boss2_bullet.back()->end_pos = { 1700, 250 };
			else if (cnt == 180) boss2_bullet.back()->end_pos = { 1700, 375 };
			else if (cnt == 190) boss2_bullet.back()->end_pos = { 1700, 501 };
			else if (cnt == 200) boss2_bullet.back()->end_pos = { 1700, 625 };
			else if (cnt == 210) boss2_bullet.back()->end_pos = { 1700, 750 };
			else if (cnt == 220) boss2_bullet.back()->end_pos = { 1700, 875 };
			else if (cnt == 230) boss2_bullet.back()->end_pos = { 1480 , 1000 };
			else if (cnt == 240) boss2_bullet.back()->end_pos = { 1270 , 1000 };
			else if (cnt == 250) boss2_bullet.back()->end_pos = { 1060 , 1000 };
			else if (cnt == 260) boss2_bullet.back()->end_pos = { 851 , 1000 };
			else if (cnt == 270) boss2_bullet.back()->end_pos = { 630 , 1000 };
			else if (cnt == 280)
			{
				cnt -= 280;
				boss2_bullet.back()->end_pos = { 420 , 1000 };
			}
		}
	}

	void Move2()
	{
		if (boss_pos.x < 330 || boss_pos.x > 1230)
			BOSS_SPEED *= -1;
		boss_pos.x += BOSS_SPEED;

		static int cnt = 0;
		if ((++cnt) == 50)
		{
			//printf("%d    ,    %d\n", cnt, boss2_bullet.size());
			boss2_bullet.push_back(new EnemyBullet());
			boss2_bullet.back()->end_pos = { 0 , boss_pos.y + 100 + 1 };
			boss2_bullet.back()->boss2 = true;
			boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
			boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
			boss2_bullet.push_back(new EnemyBullet());
			boss2_bullet.back()->end_pos = { 1700 , boss_pos.y + 100 + 1 };
			boss2_bullet.back()->boss2 = true;
			boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
			boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
			cnt -= 50;
		}
	}

	void Move3(POINT now_pos)
	{
		boss_pos = now_pos;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = 0;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = 0;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0.3333334;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -0.3333334;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0.3333334;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -0.3333334;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = 0.3333334;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = -0.3333334;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = 0.3333334;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = -0.3333334;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0.6666667;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -0.6666667;
		boss2_bullet.back()->dir_y = -1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 0.6666667;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -0.6666667;
		boss2_bullet.back()->dir_y = 1;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = 0.6666667;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = 1;
		boss2_bullet.back()->dir_y = -0.6666667;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = 0.6666667;
		boss2_bullet.push_back(new EnemyBullet());
		boss2_bullet.back()->bullet_pos = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->bullet_begin = { boss_pos.x + 50 , boss_pos.y + 100 };
		boss2_bullet.back()->boss22 = true;
		boss2_bullet.back()->dir_x = -1;
		boss2_bullet.back()->dir_y = -0.6666667;
	}

	void Draw2(int delta)
	{
		anim_boss->Play(boss_pos.x, boss_pos.y, delta);
	}

	void Draw1()
	{
		putimage_alpha(720, 250, &img_boss2_picture);
	}

	~Boss2()
	{
		delete anim_boss;
	}

	void Hurt()
	{
		if (yellow > 500) yellow -= 20;
		else if (red > 500) red -= 50;
		if (yellow <= 500 && red <= 500)
		{
			boss2xxx = 0;
			alive = false;
		}
	}

	bool CheckAlive()
	{
		return alive;
	}

private:
	int BOSS_SPEED = 2;//敌人数据
	const int BOSS1_WIDTH = 273;
	const int BOSS1_HEIGHT = 500;
	const int BOSS2_WIDTH = 120;
	const int BOSS2_HEIGHT = 120;

private:
	IMAGE img_boss2_picture;
	Animation* anim_boss;
	bool alive = true;
};

//掉落物
class Aword1
{
public:
	POINT aword_pos = { 800 , 50 };
	POINT end_pos;
	bool alive = true;

public:
	Aword1() 
	{
		loadimage(&img_aword, _T("img/red.png"));
	}
	~Aword1() = default;

	void Move()
	{
			aword_pos.y += AWORD_SPEED;
			if (aword_pos.y + AWORD_HEIGHT >= end_pos.y) aword_pos.y = end_pos.y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT position = { aword_pos.x + AWORD_WIDTH / 2, aword_pos.y + AWORD_HEIGHT / 2 };
		bool is_overlap1_x = position.x >= player.GetPosition1().x && position.x <= player.GetPosition1().x + player.PLAYER1_WIDTH;
		bool is_overlap1_y = position.y >= player.GetPosition1().y && position.y <= player.GetPosition1().y + player.PLAYER1_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{
		POINT position = { aword_pos.x + AWORD_WIDTH / 2, aword_pos.y + AWORD_HEIGHT / 2 };
		bool is_overlap2_x = position.x >= player.GetPosition2().x && position.x <= player.GetPosition2().x + player.PLAYER2_WIDTH;
		bool is_overlap2_y = position.y >= player.GetPosition2().y && position.y <= player.GetPosition2().y + player.PLAYER2_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
	}

	void Draw()
	{
		if (alive)
			putimage_alpha(aword_pos.x, aword_pos.y, &img_aword);
	}

private:
	IMAGE img_aword;
	const int AWORD_SPEED = 10;
	const int AWORD_WIDTH = 40;
	const int AWORD_HEIGHT = 40;
};

class Aword2
{
public:
	POINT aword_pos = { 800 , 50 };
	POINT end_pos;
	bool alive = true;

public:
	Aword2()
	{
		loadimage(&img_aword, _T("img/blue.png"));
	}
	~Aword2() = default;

	void Move()
	{
		aword_pos.y += AWORD_SPEED;
		if (aword_pos.y + AWORD_HEIGHT >= end_pos.y) aword_pos.y = end_pos.y;
	}

	bool CheckPlayerCollision1(const Player& player)
	{
		POINT position = { aword_pos.x + AWORD_WIDTH / 2, aword_pos.y + AWORD_HEIGHT / 2 };
		bool is_overlap1_x = position.x >= player.GetPosition1().x && position.x <= player.GetPosition1().x + player.PLAYER1_WIDTH;
		bool is_overlap1_y = position.y >= player.GetPosition1().y && position.y <= player.GetPosition1().y + player.PLAYER1_HEIGHT;
		return is_overlap1_x && is_overlap1_y;
	}

	bool CheckPlayerCollision2(const Player& player)
	{
		POINT position = { aword_pos.x + AWORD_WIDTH / 2, aword_pos.y + AWORD_HEIGHT / 2 };
		bool is_overlap2_x = position.x >= player.GetPosition2().x && position.x <= player.GetPosition2().x + player.PLAYER2_WIDTH;
		bool is_overlap2_y = position.y >= player.GetPosition2().y && position.y <= player.GetPosition2().y + player.PLAYER2_HEIGHT;
		return is_overlap2_x && is_overlap2_y;
	}

	void Draw()
	{
		if (alive)
			putimage_alpha(aword_pos.x, aword_pos.y, &img_aword);
	}

private:
	IMAGE img_aword;
	const int AWORD_SPEED = 10;
	const int AWORD_WIDTH = 40;
	const int AWORD_HEIGHT = 40;
	
};

//按钮
class Button
{
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
	{
		region = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:
			break;
		}
	}

	~Button() = default;

	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage_alpha(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage_alpha(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage_alpha(region.left, region.top, &img_pushed);
			break;
		}
	}

	void back()
	{
		status = Status::Idle;
	}

protected:
	virtual void OnClick() = 0;

private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;

private:
	//检测鼠标点击
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class Button2
{
public:
	Button2(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
	{
		region = rect;

		loadimage(&img_idle, path_img_idle);
		atlas_botton = new Atlas(path_img_hovered, 15);
		anim_botton = new Animation(atlas_botton, 45);
		loadimage(&img_pushed, path_img_pushed);
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:
			break;
		}
	}

	~Button2()
	{
		delete anim_botton;
	}

	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage_alpha(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			anim_botton->Play(region.left, region.top, 1000 / 144);
			break;
		case Status::Pushed:
			putimage_alpha(region.left, region.top, &img_pushed);
			break;
		}
	}

protected:
	virtual void OnClick() = 0;

private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;
	Animation* anim_botton;

private:
	//检测鼠标点击
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

//开始游戏按钮
class StartGameButton : public Button
{
public:
	StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~StartGameButton() = default;

protected:
	void OnClick()
	{
		is_choose_start = true;
		back();
	}
};

//退出游戏按钮
class QuitGameButton : public Button
{
public:
	QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~QuitGameButton() = default;

protected:
	void OnClick()
	{
		running = false;
		game = false;
	}
};

//返回按钮
class BackButton : public Button
{
public:
	BackButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~BackButton() = default;

protected:
	void OnClick()
	{
		if (is_choose_start)
			is_choose_start = false;
		if (is_achievement)
			is_achievement = false;
		back();
	}
};

//双人开始按钮
class DoubleStartGame : public Button
{
public:
	DoubleStartGame(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~DoubleStartGame() = default;

protected:
	void OnClick()
	{
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};

//单人边框1
class BoxButton1 : public Button2
{
public:
	BoxButton1(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button2(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~BoxButton1() = default;

protected:
	void OnClick()
	{
		is_1_alive = false;
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};

//单人边框2
class BoxButton2 : public Button2
{
public:
	BoxButton2(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button2(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~BoxButton2() = default;

protected:
	void OnClick()
	{
		is_2_alive = false;
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};

//成就按钮
class Achievement : public Button
{
public:
	Achievement(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Achievement() = default;

protected:
	void OnClick()
	{
		is_achievement = true;
		back();
	}
};

void TryGenerateEnemy1(std::vector<Enemy1*>& enemy_list, mapnode* now)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0 && now->enemy1 != 0)
	{
		enemy_list.push_back(new Enemy1());
		enemy_list.back()->enemy_pos = now->enemy1_pos;
		now->enemy1--;
	}
}

void TryGenerateEnemy2(std::vector<Enemy2*>& enemy_list, mapnode* now)
{
	const int INTERVAL = 300;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0 && now->enemy2 != 0)
	{
		enemy_list.push_back(new Enemy2());
		enemy_list.back()->enemy_pos = now->enemy2_pos;
		now->enemy2--;
	}
}

void TryGenerateEnemy3(std::vector<Enemy3*>& enemy_list, mapnode* now)
{
	const int INTERVAL = 300;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0 && now->enemy3 != 0)
	{
		enemy_list.push_back(new Enemy3());
		enemy_list.back()->enemy_pos = now->enemy3_pos;
		now->enemy3--;
	}
}

//创建地图
mapnode* createmaplist()
{
	mapnode* head = (mapnode*)malloc(sizeof(mapnode));
	if (!head) return NULL;
	head->up = NULL;
	head->down = NULL;
	head->left = NULL;
	head->right = NULL;
	head->next = NULL; 
	
	mapnode* map1 = (mapnode*)malloc(sizeof(mapnode));
	if (!map1)
	{
		free(head);
		return NULL;
	}
	map1->number = 1;
	map1->up = NULL; 
	map1->down = NULL;
	map1->left = NULL;
	map1->right = NULL;
	map1->next = NULL;
	map1->background = _T("img/beijing1.png");
	map1->enemy1 = 10;
	map1->enemy2 = 0;
	map1->enemy3 = 0;
	map1->enemy4 = 0;
	map1->enemy1a = 10;
	map1->enemy2a = 0;
	map1->enemy3a = 0;
	map1->enemy4a = 0;
	map1->aword = false;
	map1->enemy1_pos = { 1100 , 590 };
	
	mapnode* map2 = (mapnode*)malloc(sizeof(mapnode));
	if (!map2)
	{
		free(head);
		free(map1);
		return NULL;
	}
	map2->number = 2;
	map2->up = map1;
	map2->down = NULL;   
	map2->left = NULL;
	map2->right = NULL;
	map2->next = NULL;
	map2->background = _T("img/beijing2.png");
	map2->enemy1 = 15;
	map2->enemy2 = 5;
	map2->enemy3 = 0;
	map2->enemy4 = 1;
	map2->enemy1a = 15;
	map2->enemy2a = 5;
	map2->enemy3a = 0;
	map2->enemy4a = 1;
	map2->aword = false;
	map2->enemy1_pos = { 400 , 570 };
	map2->enemy2_pos = { 1600 , 570 };

	mapnode* map3 = (mapnode*)malloc(sizeof(mapnode));
	if (!map3)
	{
		free(head);
		free(map1);
		free(map2);
		return NULL;  
	}
	map3->number = 3;
	map3->up = map2;
	map3->down = NULL;
	map3->left = NULL;
	map3->right = NULL;
	map3->next = NULL;
	map3->background = _T("img/beijing3.png");
	map3->enemy1 = 0;
	map3->enemy2 = 0;
	map3->enemy3 = 0;
	map3->enemy4 = 0;
	map3->enemy1a = 0;
	map3->enemy2a = 0;
	map3->enemy3a = 0;
	map3->enemy4a = 0;
	map3->boss1a = 1;
	map3->aword = false;

	mapnode* map4 = (mapnode*)malloc(sizeof(mapnode));
	if (!map4)
	{
		free(head);
		free(map1);
		free(map2);
		free(map3);
		return NULL;
	}
	map4->number = 4;
	map4->up = NULL;
	map4->down = NULL;
	map4->left = map2;
	map4->right = NULL;
	map4->next = NULL;  
	map4->background = _T("img/beijing4.png");
	map4->enemy1 = 10;
	map4->enemy2 = 10;
	map4->enemy3 = 5;
	map4->enemy4 = 2;
	map4->enemy1a = 10;
	map4->enemy2a = 10;
	map4->enemy3a = 5;
	map4->enemy4a = 2;
	map4->aword = false;
	map4->enemy1_pos = { 800 , 550 };
	map4->enemy2_pos = { 1600 , 550 };
	map4->enemy3_pos = { 1000 , 10 };

	mapnode* map5 = (mapnode*)malloc(sizeof(mapnode));
	if (!map5)
	{
		free(head);
		free(map1);
		free(map2);
		free(map3);
		free(map4);
		return NULL;
	}
	map5->number = 5;
	map5->up = map4;
	map5->down = NULL;
	map5->left = NULL;
	map5->right = NULL;
	map5->next = NULL;
	map5->background = _T("img/beijing5.png");
	map5->enemy1 = 5;
	map5->enemy2 = 10;
	map5->enemy3 = 10;
	map5->enemy4 = 0;
	map5->enemy1a = 5;
	map5->enemy2a = 10;
	map5->enemy3a = 10;
	map5->enemy4a = 0;
	map5->aword = false;
	map5->enemy1_pos = { 10 , 590 };
	map5->enemy2_pos = { 1680 , 590 };
	map5->enemy3_pos = { 1000 , 0 };

	mapnode* map6 = (mapnode*)malloc(sizeof(mapnode));
	if (!map6)
	{
		free(head);
		free(map1); 
		free(map2);
		free(map3);
		free(map4);
		free(map5);
		return NULL;
	}
	map6->number = 6;
	map6->up = NULL;
	map6->down = NULL;
	map6->left = NULL;
	map6->right = map5;
	map6->next = NULL;
	map6->background = _T("img/beijing6.png");
	map6->enemy1 = 8;
	map6->enemy2 = 15;
	map6->enemy3 = 10;
	map6->enemy4 = 1;
	map6->enemy1a = 8;
	map6->enemy2a = 15;
	map6->enemy3a = 10;
	map6->enemy4a = 1;
	map6->aword = false;
	map6->enemy1_pos = { 10 , 590 };
	map6->enemy2_pos = { 10 , 590 };
	map6->enemy3_pos = { 1000 , 0 };

	mapnode* map7 = (mapnode*)malloc(sizeof(mapnode));
	if (!map7)
	{
		free(head);
		free(map1);
		free(map2);
		free(map3);
		free(map4);
		free(map5);
		free(map6);
		return NULL;
	}
	map7->number = 7;
	map7->up = NULL;
	map7->down = NULL;
	map7->left = map5;
	map7->right = NULL;
	map7->next = NULL;
	map7->background = _T("img/beijing7.png");
	map7->enemy1 = 5;
	map7->enemy2 = 5;
	map7->enemy3 = 15;
	map7->enemy4 = 4;
	map7->enemy1a = 5;
	map7->enemy2a = 5;
	map7->enemy3a = 15;
	map7->enemy4a = 4;
	map7->aword = false;
	map7->enemy1_pos = { 1680 , 930 };
	map7->enemy2_pos = { 1680 , 920 };
	map7->enemy3_pos = { 1000 , 0 };

	mapnode* map8 = (mapnode*)malloc(sizeof(mapnode));
	if (!map8)
	{
		free(head);
		free(map1);
		free(map2);
		free(map3);
		free(map4);
		free(map5);
		free(map6);
		free(map7);
		return NULL;
	}
	map8->number = 8;
	map8->up = NULL;
	map8->down = NULL;
	map8->left = map7;
	map8->right = NULL;
	map8->next = NULL;
	map8->background = _T("img/beijing8.png");
	map8->enemy1 = 0;
	map8->enemy2 = 0;
	map8->enemy3 = 0;
	map8->enemy4 = 0;
	map8->enemy1a = 0;
	map8->enemy2a = 0;
	map8->enemy3a = 0;
	map8->enemy4a = 0; 
	map8->boss2a = 2;
	map8->aword = false;

	mapnode* map9 = (mapnode*)malloc(sizeof(mapnode));
	if (!map9)
	{
		free(head);
		free(map1);
		free(map2);
		free(map3);
		free(map4);
		free(map5);
		free(map6);
		free(map7);
		free(map8);
		return NULL;
	}
	map9->number = 9;
	map9->up = NULL;
	map9->down = NULL;
	map9->left = map8;
	map9->right = NULL;
	map9->next = NULL;
	map9->background = _T("img/beijing9.png");
	map9->enemy1 = 0;
	map9->enemy2 = 0;
	map9->enemy3 = 0;
	map9->enemy4 = 0;
	map9->enemy1a = 0;
	map9->enemy2a = 0;
	map9->enemy3a = 0;
	map9->enemy4a = 0;
	map9->boss2a = 1;
	map9->aword = false;

	head->next = map1;
	map1->down = map2;
	map2->down = map3;
	map2->right = map4;
	map4->down = map5;
	map5->left = map6;
	map5->right = map7;
	map7->right = map8;
	map8->right = map9;
	
	return head;
}

int cnt1 = 0;
int cnt2 = 0;
int cnt_boss2 = 0;

bool is_box_open = false;
bool is_open_draw = false;
bool is_close_draw = false;

int main()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);//创建窗口

	//初始化地图链表
	mapnode* head = createmaplist();
	mapnode* now = head->next;

	atlas_player1_left = new Atlas(_T("img/player1_left_%d.png"), 4);
	atlas_player2_left = new Atlas(_T("img/player2_left_%d.png"), 4);
	atlas_player1_right = new Atlas(_T("img/player1_right_%d.png"), 4);
	atlas_player2_right = new Atlas(_T("img/player2_right_%d.png"), 4);
	atlas_player1_left_st = new Atlas(_T("img/player1_left_%d.png"), 1);
	atlas_player2_left_st = new Atlas(_T("img/player2_left_%d.png"), 1);
	atlas_player1_right_st = new Atlas(_T("img/player1_right_%d.png"), 1);
	atlas_player2_right_st = new Atlas(_T("img/player2_right_%d.png"), 1);
	atlas_enemy1_left = new Atlas(_T("img/enemy1_left_%d.png"), 8);
	atlas_enemy2_left = new Atlas(_T("img/enemy2_left_%d.png"), 8);
	atlas_enemy1_right = new Atlas(_T("img/enemy1_right_%d.png"), 8);
	atlas_enemy2_right = new Atlas(_T("img/enemy2_right_%d.png"), 8);
	atlas_enemy3_left = new Atlas(_T("img/enemy3_left_%d.png"), 6);
	atlas_enemy3_right = new Atlas(_T("img/enemy3_right_%d.png"), 6);
	atlas_boss1_left = new Atlas(_T("img/boss_left_%d.png"), 8);
	atlas_boss1_right = new Atlas(_T("img/boss_right_%d.png"), 8);
	atlas_boss2 = new Atlas(_T("img/boss2_%d.png"), 6);
	atlas_bullet_left = new Atlas(_T("img/bullet_left_%d.png"), 8);
	atlas_bullet_right = new Atlas(_T("img/bullet_right_%d.png"), 8);
	atlas_enemybullet = new Atlas(_T("img/bullet_enemy_%d.png"), 1);


	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/shoot.mp3 alias shoot"), NULL, 0, NULL);

	ExMessage msg;
	
	IMAGE img_background;
	IMAGE img_menu;
	IMAGE img_choose;
	IMAGE img_player1_ui;
	IMAGE img_player2_ui;
	IMAGE img_boss_ui;
	IMAGE img_box_open;
	IMAGE img_box_close;
	IMAGE img_win;
	IMAGE img_achievement;
	IMAGE img_star;

	Player player1;
	Boss1 boss1;
	Boss2 boss2;
	std::vector<Enemy1*> enemy1_list;
	std::vector<Enemy2*> enemy2_list;
	std::vector<Enemy3*> enemy3_list;
	std::vector<Enemy4*> enemy4_list;
	std::vector<Bullet*> bullet_list;
	std::vector<Aword1*> aword1_list;
	std::vector<Aword2*> aword2_list;

	//按钮
	RECT region_btn_start_game, region_btn_quit_game, region_btn_back_choose, region_btn_double_start;
	RECT region_btn_box1, region_btn_box2, region_btn_achievement;

	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH_START) / 2;
	region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH_START;
	region_btn_start_game.top = 600;
	region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT_START;

	region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH_QUIT) / 2 - 10;
	region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH_QUIT;
	region_btn_quit_game.top = 780;
	region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT_QUIT;

	region_btn_back_choose.left = 0;
	region_btn_back_choose.right = 300;
	region_btn_back_choose.top = 0;
	region_btn_back_choose.bottom = 90;

	region_btn_double_start.left = 660;
	region_btn_double_start.right = 960;
	region_btn_double_start.top = 840;
	region_btn_double_start.bottom = 951;

	region_btn_box1.left = 265;
	region_btn_box1.right = 615;
	region_btn_box1.top = 335;
	region_btn_box1.bottom = 785;

	region_btn_box2.left = 1025;
	region_btn_box2.right = 1375;
	region_btn_box2.top = 335;
	region_btn_box2.bottom = 785;

	region_btn_achievement.left = (WINDOW_WIDTH - 64) / 2 - 10;
	region_btn_achievement.right = region_btn_achievement.left + 64;
	region_btn_achievement.top = 680;
	region_btn_achievement.bottom = region_btn_achievement.top + 64;

	StartGameButton btn_start_game = StartGameButton(region_btn_start_game,
		_T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGameButton btn_quit_game = QuitGameButton(region_btn_quit_game,
		_T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));
	BackButton btn_back_choose = BackButton(region_btn_back_choose,
		_T("img/back_buttom.png"), _T("img/back_buttom2.png"), _T("img/back_buttom2.png"));
	DoubleStartGame btn_double_start = DoubleStartGame(region_btn_double_start,
		_T("img/double_buttom.png"), _T("img/double_buttom2.png"), _T("img/double_buttom2.png"));
	BoxButton1 btn_box1 = BoxButton1(region_btn_box1,
		_T("img/choose_0.png"), _T("img/choose_%d.png"), _T("img/choose_0.png"));
	BoxButton2 btn_box2 = BoxButton2(region_btn_box2,
		_T("img/choose_0.png"), _T("img/choose_%d.png"), _T("img/choose_0.png"));
	Achievement btn_achievement = Achievement(region_btn_achievement,
		_T("img/achievement_button.jpg"), _T("img/achievement_button2.jpg"), _T("img/achievement_button2.jpg"));

	loadimage(&img_background, now->background);
	loadimage(&img_menu, _T("img/roefian.png"));
	loadimage(&img_choose, _T("img/menu2.png"));
	loadimage(&img_player1_ui, _T("img/player1_ui.png"));
	loadimage(&img_player2_ui, _T("img/player2_ui.png"));
	loadimage(&img_boss_ui, _T("img/boss_ui.png"));
	loadimage(&img_box_open, _T("img/box_open.png"));
	loadimage(&img_box_close, _T("img/box_close.png"));
	loadimage(&img_win, _T("img/win.png"));
	loadimage(&img_achievement, _T("img/achievement.png"));
	loadimage(&img_star, _T("img/star.png"));

	BeginBatchDraw();

	while(game)
	{ 
	while (running && !pause)//主循环
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))//输入循环
		{
			if (is_game_started)
			{
				player1.ProcessEvent(msg);

				switch (msg.message)
				{
				case WM_KEYDOWN:
					switch (msg.vkcode)
					{
					case 0x4A:
						if (player1_blue > 140 && player1.player1_alive)
						{
							mciSendString(_T("play shoot from 0"), NULL, 0, NULL);
							bullet_list.push_back(new Bullet());
							player1_blue -= 5;
							bullet_list.back()->bullet_facing_left = player1.facing_left1;
							if (bullet_list.back()->bullet_facing_left)
							{
								bullet_list.back()->bullet_pos.x = player1.GetPosition1().x + 100;
								bullet_list.back()->bullet_pos.y = player1.GetPosition1().y;
								bullet_list.back()->bullet_begin.x = player1.GetPosition1().x + 100;
								bullet_list.back()->bullet_begin.y = player1.GetPosition1().y;
							}
							else
							{
								bullet_list.back()->bullet_pos.x = player1.GetPosition1().x + 30;
								bullet_list.back()->bullet_pos.y = player1.GetPosition1().y;
								bullet_list.back()->bullet_begin.x = player1.GetPosition1().x + 30;
								bullet_list.back()->bullet_begin.y = player1.GetPosition1().y;
							}
						}
						break;

					case VK_NUMPAD1:
						if (player2_blue > 140 && player1.player2_alive)
						{
							mciSendString(_T("play shoot from 0"), NULL, 0, NULL);
							bullet_list.push_back(new Bullet());
							player2_blue -= 5;
							bullet_list.back()->bullet_facing_left = player1.facing_left2;
							if (bullet_list.back()->bullet_facing_left)
							{
								bullet_list.back()->bullet_pos.x = player1.GetPosition2().x + 100;
								bullet_list.back()->bullet_pos.y = player1.GetPosition2().y;
								bullet_list.back()->bullet_begin.x = player1.GetPosition2().x + 100;
								bullet_list.back()->bullet_begin.y = player1.GetPosition2().y;
							}
							else
							{
								bullet_list.back()->bullet_pos.x = player1.GetPosition2().x + 30;
								bullet_list.back()->bullet_pos.y = player1.GetPosition2().y;
								bullet_list.back()->bullet_begin.x = player1.GetPosition2().x + 30;
								bullet_list.back()->bullet_begin.y = player1.GetPosition2().y;
							}
						}
						break;

					case VK_ESCAPE:
						pause = true;
						break;
					}
				}
			}
			else
			{
				if (!is_choose_start && !is_achievement)
				{
					btn_start_game.ProcessEvent(msg);
					btn_quit_game.ProcessEvent(msg);
					btn_achievement.ProcessEvent(msg);
				}
				else if (is_choose_start && !is_achievement)
				{
					btn_back_choose.ProcessEvent(msg);
					btn_double_start.ProcessEvent(msg);
					btn_box1.ProcessEvent(msg);
					btn_box2.ProcessEvent(msg);
					if (!is_1_alive) player1.player1_alive = false;
					if (!is_2_alive) player1.player2_alive = false;
				}
				else if (!is_choose_start && is_achievement)
				{
					btn_back_choose.ProcessEvent(msg);
				}
			}
		}

		if (is_game_started)
		{
			//子弹
			for (Bullet* bullet : bullet_list)
				bullet->Move(player1);
			for (Enemy4* enemy : enemy4_list)
				for (EnemyBullet* enemybullet : enemy->enemybullet_list)
					enemybullet->Move();
			if (now->number == 8 && boss2.yellow > 500) boss2.Move();
			if (now->number == 8 && boss2.yellow == 500 && boss2.red > 500)
			{
				cnt_boss2++;
				if (cnt_boss2 < 1500)boss2.Move2();
				else if (cnt_boss2 >= 1500 && cnt_boss2 < 2500)
				{
					boss2.boss_pos = { 785 , 375 };
					boss2.Move();
				}
				else if (cnt_boss2 == 2500) boss2.Move3({ 185, 560 });
				else if (cnt_boss2 == 2700) boss2.Move3({ 118, 385 });
				else if (cnt_boss2 == 2900) boss2.Move3({ 1425, 288 });
				else if (cnt_boss2 == 3100) boss2.Move3({ 1505, 465 });
				else if (cnt_boss2 == 3300) boss2.Move3({ 1360, 590 });
				else if (cnt_boss2 == 3500)
				{
					cnt_boss2 -= 3500;
					boss2.boss_pos = { 850 , 800 };
				}
				
			}
			if (now->number == 8)
				for (EnemyBullet* enemybullet : boss2.boss2_bullet)
					enemybullet->Move();

			for (Aword1* aword : aword1_list)
				aword->Move();
			for (Aword2* aword : aword2_list)
				aword->Move();

			player1.Move();
			if (player1.player1_alive == false && player1.player2_alive == true) player1.player1_pos = player1.player2_pos;
			if (player1.player1_alive == true && player1.player2_alive == false) player1.player2_pos = player1.player1_pos;

			//地图碰撞
			//1
			if (now->number == 1)
			{
				if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1400 || player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1600)
				{
					if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 350 > WINDOW_HEIGHT)
					{
						player1.is_move_s = false;
					}
					else
					{
						if (player1.is_move_w == false)
							player1.is_move_s = true;
					}
				}
				else
				{
					if (player1.is_move_w == false) player1.is_move_s = true;
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1410 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 350 > WINDOW_HEIGHT) player1.player1_pos.x = 1410 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1590 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 350 > WINDOW_HEIGHT) player1.player1_pos.x = 1590 - (player1.PLAYER1_WIDTH / 2);
				}
				if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1400 || player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1600)
				{
					if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 350 > WINDOW_HEIGHT)
					{
						player1.is_move_down = false;
					}
					else
					{
						if (player1.is_move_up == false)
							player1.is_move_down = true;
					}
				}
				else
				{
					if (player1.is_move_up == false) player1.is_move_down = true;
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1410 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 350 > WINDOW_HEIGHT) player1.player2_pos.x = 1410 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1590 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 350 > WINDOW_HEIGHT) player1.player2_pos.x = 1590 - (player1.PLAYER2_WIDTH / 2);
				}
			}
			//2
			if (now->number == 2)
			{
				if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 360 || (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 570 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1150) || player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1370)
				{
					if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 370 > WINDOW_HEIGHT)
					{
						player1.is_move_s = false;
					}
					else
					{
						if (player1.is_move_w == false)
							player1.is_move_s = true;
					}
				}
				else
				{
					if (player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 450 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 465 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 380 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 570)
					{
						player1.player1_pos.y  = 450 - player1.PLAYER1_HEIGHT;
						if (player1.is_move_w == false)
						player1.is_move_s = false;
					}
					else if (player1.is_move_w == false) player1.is_move_s = true;
					if (player1.player1_pos.y == WINDOW_HEIGHT - player1.PLAYER1_HEIGHT && player1.player1_alive )
					{
						if (player1_red <= 340 && player1.player2_alive)
						{
							player1_red = 140;
							player1.player1_alive = false;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
						}
						else if (player1_red <= 340 && !player1.player2_alive)
						{
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
						}
						else
						{
							player1_red -= 200;
							player1.player1_pos = { 430 , 300 };
						}
					}
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 370 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 370 > WINDOW_HEIGHT) player1.player1_pos.x = 370 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 580 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 560 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 370 > WINDOW_HEIGHT) player1.player1_pos.x = 560 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1140 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1160 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 370 > WINDOW_HEIGHT) player1.player1_pos.x = 1160 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1360 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 370 > WINDOW_HEIGHT) player1.player1_pos.x = 1360 - (player1.PLAYER1_WIDTH / 2);
				}
				if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 360 || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 570 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1150) || player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1370)
				{
					if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 370 > WINDOW_HEIGHT)
					{
						player1.is_move_down = false;
					}
					else
					{
						if (player1.is_move_up == false)
							player1.is_move_down = true;
					}
				}
				else
				{
					if (player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 450 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 465 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 380 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 570)
					{
						player1.player2_pos.y  = 450 - player1.PLAYER2_HEIGHT;
						if (player1.is_move_up == false)
							player1.is_move_down = false;
					}
					else if (player1.is_move_up == false) player1.is_move_down = true;
					if (player1.player2_pos.y == WINDOW_HEIGHT - player1.PLAYER2_HEIGHT && player1.player2_alive )
					{
						if (player2_red <= 340 && player1.player1_alive)
						{
							player2_red = 140;
							player1.player2_alive = false;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
						}
						else if (player2_red <= 340 && !player1.player1_alive)
						{
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
						}
						else 
						{
							player2_red -= 200;
							player1.player2_pos = { 430 , 300 };
						}
					}
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 370 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 370 > WINDOW_HEIGHT) player1.player2_pos.x = 370 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 580 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 560 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 370 > WINDOW_HEIGHT) player1.player2_pos.x = 560 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1140 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1160 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 370 > WINDOW_HEIGHT) player1.player2_pos.x = 1160 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1360 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 370 > WINDOW_HEIGHT) player1.player2_pos.x = 1360 - (player1.PLAYER2_WIDTH / 2);
				}
			}
			//3
			if (now->number == 3)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 420 > WINDOW_HEIGHT)
				{
					player1.is_move_s = false;
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 420 > WINDOW_HEIGHT)
				{
					player1.is_move_down = false;
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
			}
			//4
			if (now->number == 4)
			{
				if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 440 || (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 620 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 820) || (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1080 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1280))
				{
					if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT)
					{
						player1.is_move_s = false;
					}
					else
					{
						if (player1.is_move_w == false)
							player1.is_move_s = true;
					}
				}
				else
				{
					if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 450 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 465) && ((player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 870 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1010) || (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1410 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1560)))
					{
						player1.player1_pos.y  = 450 - player1.PLAYER1_HEIGHT;
						if (player1.is_move_w == false)
							player1.is_move_s = false;
					}
					else if (player1.is_move_w == false) player1.is_move_s = true;
					if (player1.player1_pos.y == WINDOW_HEIGHT - player1.PLAYER1_HEIGHT && player1.player1_alive )
					{
						if (player1_red <= 340 && player1.player2_alive)
						{
							player1_red = 140;
							player1.player1_alive = false;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
						}
						else if ( player1_red <= 340 && !player1.player2_alive)
						{
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
						}
						else 
						{
							player1_red -= 200;
							player1.player1_pos = { 930 , 300 };
						}
					}
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 450 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 450 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 620 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 610 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 610 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) > 820 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 830 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 830 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1080 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1070 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 1070 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1280 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1290 && player1.player1_pos.y + player1.PLAYER1_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 1290 - (player1.PLAYER1_WIDTH / 2);
				}
				if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 440 || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 620 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 820) || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1080 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1280))
				{
					if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT)
					{
						player1.is_move_down = false;
					}
					else
					{
						if (player1.is_move_up == false)
							player1.is_move_down = true;
					}
				}
				else
				{
					if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 450 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 465) && ((player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 870 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1010) || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1410 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1560)))
					{
						player1.player2_pos.y  = 450 - player1.PLAYER2_HEIGHT;
						if (player1.is_move_up == false)
							player1.is_move_down = false;
					}
					else if (player1.is_move_up == false) player1.is_move_down = true;
					if (player1.player2_pos.y == WINDOW_HEIGHT - player1.PLAYER2_HEIGHT && player1.player2_alive )
					{
						if (player2_red <= 340 && player1.player1_alive)
						{
							player2_red = 140;
							player1.player2_alive = false;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
						}
						else if (player2_red <= 340 && !player1.player1_alive)
						{
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
						}
						else
						{
							player2_red -= 200;
							player1.player2_pos = { 930 , 300 };
						}
					}
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 450 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT) player1.player1_pos.x = 450 - (player1.PLAYER1_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 620 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 610 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT) player1.player2_pos.x = 610 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) > 820 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 830 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT) player1.player2_pos.x = 830 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1080 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1070 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT) player1.player2_pos.x = 1070 - (player1.PLAYER2_WIDTH / 2);
					if (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1280 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1290 && player1.player2_pos.y + player1.PLAYER2_HEIGHT + 380 > WINDOW_HEIGHT) player1.player2_pos.x = 1290 - (player1.PLAYER2_WIDTH / 2);
				}
			}
			//5
			if (now->number == 5)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 330 > WINDOW_HEIGHT)
				{
					player1.is_move_s = false;
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 330 > WINDOW_HEIGHT)
				{
					player1.is_move_down = false;
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
			}
			//6
			if (now->number == 6)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 330 > WINDOW_HEIGHT)
				{
					player1.is_move_s = false;
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 330 > WINDOW_HEIGHT)
				{
					player1.is_move_down = false;
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
			}
			//7
			if (now->number == 7)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 20 > WINDOW_HEIGHT)
				{
					player1.is_move_s = false;
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 750 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 765) && ((player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 510 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 710) || (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1170 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1370)))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 750 - player1.PLAYER1_HEIGHT;
					}
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 550 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 565) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 850 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1050))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 550 - player1.PLAYER1_HEIGHT;
					}
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 20 > WINDOW_HEIGHT)
				{
					player1.is_move_down = false;
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 750 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 765) && ((player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 510 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 710) || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1170 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1370)))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 750 - player1.PLAYER2_HEIGHT;
					}
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 550 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 565) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 850 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1050))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 550 - player1.PLAYER2_HEIGHT;
					}
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
			}
			//8
			if (now->number == 8)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 20 > WINDOW_HEIGHT)
				{
					player1.player1_pos.y = 920;
					player1.is_move_s = false;
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 750 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 765) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 150 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 350))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 750 - player1.PLAYER1_HEIGHT;
					}
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 770 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 785) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1330 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1530))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 770 - player1.PLAYER1_HEIGHT;
					}
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 570 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 585) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 70 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 270))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 570 - player1.PLAYER1_HEIGHT;
					}
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 650 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 675) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1470 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1670))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 650 - player1.PLAYER1_HEIGHT;
					}
				}
				else if ((player1.player1_pos.y + player1.PLAYER1_HEIGHT >= 460 && player1.player1_pos.y + player1.PLAYER1_HEIGHT <= 465) && (player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1390 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1590))
				{
					if (player1.is_move_w == false)
					{
						player1.is_move_s = false;
						player1.player1_pos.y = 460 - player1.PLAYER1_HEIGHT;
					}
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 20 > WINDOW_HEIGHT)
				{
					player1.player2_pos.y = 920;
					player1.is_move_down = false;
				}
				 else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 750 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 765) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 150 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 350))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 750 - player1.PLAYER2_HEIGHT;
					}
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 770 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 785) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1330 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1530))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 770 - player1.PLAYER2_HEIGHT;
					}
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 570 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 585) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 70 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 270))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 570 - player1.PLAYER2_HEIGHT;
					}
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 650 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 675) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1470 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1670))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 650 - player1.PLAYER2_HEIGHT;
					}
				}
				else if ((player1.player2_pos.y + player1.PLAYER2_HEIGHT >= 460 && player1.player2_pos.y + player1.PLAYER2_HEIGHT <= 465) && (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1390 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1590))
				{
					if (player1.is_move_up == false)
					{
						player1.is_move_down = false;
						player1.player2_pos.y = 460 - player1.PLAYER2_HEIGHT;
					}
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
			}
			//9
			if (now->number == 9)
			{
				if (player1.player1_pos.y + player1.PLAYER1_HEIGHT + 50 > WINDOW_HEIGHT)
				{
					player1.is_move_s = false;
				}
				else
				{
					if (player1.is_move_w == false)
						player1.is_move_s = true;
				}
				if (player1.player2_pos.y + player1.PLAYER2_HEIGHT + 50 > WINDOW_HEIGHT)
				{
					player1.is_move_down = false;
				}
				else
				{
					if (player1.is_move_up == false)
						player1.is_move_down = true;
				}
				if (player1.player1_pos.x >= 610 && player1.player1_pos.x <= 1180 || player1.player2_pos.x >= 610 && player1.player2_pos.x <= 1180)
				{
					if (player1.is_move_w || player1.is_move_up)
					{
						MessageBox(GetHWnd(), _T("win!"), _T("end"), MB_OK);
						game = false;
						running = false;
					}
				}
			}
			
			TryGenerateEnemy1(enemy1_list, now);
			
			for (Enemy1* enemy : enemy1_list)
				enemy->Move(player1);

			TryGenerateEnemy2(enemy2_list, now);

			for (Enemy2* enemy : enemy2_list)
				enemy->Move(player1);

			TryGenerateEnemy3(enemy3_list, now);

			for (Enemy3* enemy : enemy3_list)
				enemy->Move(player1);

			if (now->number == 2 && now->enemy4)
			{
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 200 , 200 };
				now->enemy4 = 0;
			}
			if (now->number == 4 && now->enemy4)
			{
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 350 , 150 };
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 1000 , 220 };
				now->enemy4 = 0;
			}
			if (now->number == 6 && now->enemy4)
			{
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 1400 , 350 };
				now->enemy4 = 0;
			}
			if (now->number == 7 && now->enemy4)
			{
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 300 , 550 };
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 600 , 300 };
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 1550 , 550 };
				enemy4_list.push_back(new Enemy4());
				enemy4_list.back()->enemy_pos = { 1260 , 300 };
				now->enemy4 = 0;
			}
			for (Enemy4* enemy : enemy4_list)
				enemy->Move(player1);

			if (now->number == 3 && boss1.CheckAlive())
			{
				if (boss1.boss_pos.x == -200 && boss1.boss_pos.y == -200) boss1.boss_pos = { 100 , 100 };
				boss1.Move(player1);
			}

			if (now->number == 8 && boss2.CheckAlive() && boss2.yellow <= 500 && boss2.red > 500)
			{
				if (boss2.boss_pos.x == -200 && boss2.boss_pos.y == -200) boss2.boss_pos = { 850 , 800 };
			}

			//检测敌人和玩家的碰撞
			
			const int x = 100;
			if ((++cnt1) >= x)
			{
				for (Enemy1* enemy : enemy1_list)
					if (enemy->CheckPlayerCollision1(player1) && player1.player1_alive)
					{
						now->enemy1a--;
						//printf("%d ", now->enemy1a);
						if (player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
							player1.player1_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player1_red > 140)
						{
							player1_red -= 100;
							cnt1 = 0;
							enemy->Hurt();
						}
						break;
					}
				
				for (Enemy2* enemy : enemy2_list)
					if (enemy->CheckPlayerCollision1(player1) && player1.player1_alive)
					{
						now->enemy2a--;
						//printf("%d ", now->enemy2a);
						if (player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
							player1.player1_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player1_red > 140)
						{
							player1_red -= 100;
							cnt1 = 0;
							enemy->Hurt();
						}
						break;
					}
				
				for (Enemy3* enemy : enemy3_list)
					if (enemy->CheckPlayerCollision1(player1) && player1.player1_alive)
					{
						now->enemy3a--;
						//printf("%d ", now->enemy3a);
						if (player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
							player1.player1_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player1_red > 140)
						{
							player1_red -= 100;
							cnt1 = 0;
							enemy->Hurt();
						}
						break;
					}
				
				for (Enemy4* enemy : enemy4_list)
					if (enemy->CheckPlayerCollision1(player1) && player1.player1_alive)
					{
						now->enemy4a--;
						if (player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
							player1.player1_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							cnt1 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player1_red > 140)
						{
							player1_red -= 100;
							cnt1 = 0;
							enemy->Hurt();
						}
						break;
					}
				
				if (boss1.CheckPlayerCollision1(player1) && player1.player1_alive)
				{
					if (player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						cnt1 = 0;
						MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
						player1.player1_alive = false;
						boss1.Hurt();
					}
					else if (!player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						cnt1 = 0;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						boss1.Hurt();
					}
					if (player1_red > 140)
					{
						player1_red -= 100;
						cnt1 = 0;
						boss1.Hurt();
					}
				}

				if (boss2.yellow <= 500 && boss2.red > 500 && boss2.CheckPlayerCollision1(player1) && player1.player1_alive)
				{
					if (player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						cnt1 = 0;
						MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
						player1.player1_alive = false;
						boss2.Hurt();
					}
					else if (!player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						cnt1 = 0;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						boss2.Hurt();
					}
					if (player1_red > 140)
					{
						player1_red -= 100;
						cnt1 = 0;
						boss2.Hurt();
					}
				}
			}
			if ((++cnt2) >= x)
			{
				for (Enemy1* enemy : enemy1_list)
					if (enemy->CheckPlayerCollision2(player1) && player1.player2_alive)
					{
						now->enemy1a--;
						if (player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
							player1.player2_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player2_red > 140)
						{
							player2_red -= 100;
							cnt2 = 0;
							enemy->Hurt();
						}
						break;
					}

				for (Enemy2* enemy : enemy2_list)
					if (enemy->CheckPlayerCollision2(player1) && player1.player2_alive)
					{
						now->enemy2a--;
						if (player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
							player1.player2_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player2_red > 140)
						{
							player2_red -= 100;
							cnt2 = 0;
							enemy->Hurt();
						}
						break;
					}

				for (Enemy3* enemy : enemy3_list)
					if (enemy->CheckPlayerCollision2(player1) && player1.player2_alive)
					{
						now->enemy3a--;
						if (player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
							player1.player2_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player2_red > 140)
						{
							player2_red -= 100;
							cnt2 = 0;
							enemy->Hurt();
						}
						break;
					}

				for (Enemy4* enemy : enemy4_list)
					if (enemy->CheckPlayerCollision2(player1) && player1.player2_alive)
					{
						now->enemy4a--;
						if (player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
							player1.player2_alive = false;
							enemy->Hurt();
						}
						else if (!player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							cnt2 = 0;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemy->Hurt();
						}
						else if (player2_red > 140)
						{
							player2_red -= 100;
							cnt2 = 0;
							enemy->Hurt();
						}
						break;
					}

				if (boss1.CheckPlayerCollision2(player1) && player1.player2_alive)
				{
					if (player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						cnt2 = 0;
						MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
						player1.player2_alive = false;
						boss1.Hurt();
					}
					else if (!player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						cnt2 = 0;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						boss1.Hurt();
					}
					else if (player2_red > 140)
					{
						player2_red -= 100;
						cnt2 = 0;
						boss1.Hurt();
					}
				}

				if (boss2.yellow <= 500 && boss2.red > 500 && boss2.CheckPlayerCollision2(player1) && player1.player2_alive)
				{
					if (player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						cnt2 = 0;
						MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
						player1.player2_alive = false;
						boss2.Hurt();
					}
					else if (!player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						cnt2 = 0;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						boss2.Hurt();
					}
					else if (player2_red > 140)
					{
						player2_red -= 100;
						cnt2 = 0;
						boss2.Hurt();
					}
				}
			}
			//检测子弹和玩家的碰撞
			for (Enemy4* enemy : enemy4_list)
				for (EnemyBullet* enemybullet : enemy->enemybullet_list)
				{
					if (enemybullet->CheckPlayerCollision1(player1) && player1.player1_alive)
					{
						if (player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
							player1.player1_alive = false;
							enemybullet->alive = false;
						}
						else if (!player1.player2_alive && player1_red == 240)
						{
							player1_red -= 100;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemybullet->alive = false;
						}
						else if (player1_red > 140)
						{
							player1_red -= 100;
							enemybullet->alive = false;
						}
						break;
					}

					if (enemybullet->CheckPlayerCollision2(player1) && player1.player2_alive)
					{
						if (player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
							player1.player2_alive = false;
							enemybullet->alive = false;
						}
						else if (!player1.player1_alive && player2_red == 240)
						{
							player2_red -= 100;
							MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
							game = false;
							running = false;
							enemybullet->alive = false;
						}
						else if (player2_red > 140)
						{
							player2_red -= 100;
							enemybullet->alive = false;
						}
						break;
					}
				}
			for (EnemyBullet* enemybullet : boss2.boss2_bullet)
			{
				if (enemybullet->CheckPlayerCollision1(player1) && player1.player1_alive)
				{
					if (player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						MessageBox(GetHWnd(), _T("player1 die!"), _T("be careful!"), MB_OK);
						player1.player1_alive = false;
						enemybullet->alive = false;
					}
					else if (!player1.player2_alive && player1_red == 240)
					{
						player1_red -= 100;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						enemybullet->alive = false;
					}
					else if (player1_red > 140)
					{
						player1_red -= 100;
						enemybullet->alive = false;
					}
					break;
				}
				if (enemybullet->CheckPlayerCollision2(player1) && player1.player2_alive)
				{
					if (player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						MessageBox(GetHWnd(), _T("player2 die!"), _T("be careful!"), MB_OK);
						player1.player2_alive = false;
						enemybullet->alive = false;
					}
					else if (!player1.player1_alive && player2_red == 240)
					{
						player2_red -= 100;
						MessageBox(GetHWnd(), _T("loser!"), _T("end"), MB_OK);
						game = false;
						running = false;
						enemybullet->alive = false;
					}
					else if (player2_red > 140)
					{
						player2_red -= 100;
						enemybullet->alive = false;
					}
					break;
				}
			}
			//检测子弹和敌人的碰撞
			for (Enemy1* enemy : enemy1_list)
			{
				for (Bullet* bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(*bullet))
					{
						now->enemy1a--;
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						bullet->alive = false;
					}
				}
			}
			for (Enemy2* enemy : enemy2_list)
			{
				for (Bullet* bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(*bullet))
					{
						now->enemy2a--;
						//printf("%d ", now->enemy2a);
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						bullet->alive = false;
					}
				}
			}
			for (Enemy3* enemy : enemy3_list)
			{
				for (Bullet* bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(*bullet))
					{
						now->enemy3a--;
						//printf("%d ", now->enemy3a);
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						bullet->alive = false;
					}
				}
			}
			for (Enemy4* enemy : enemy4_list)
			{
				for (Bullet* bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(*bullet))
					{
						printf("111 ");
						now->enemy4a--;
						//printf("%d ", now->enemy4a);
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						bullet->alive = false;
					}
				}
			}
			for (Bullet* bullet : bullet_list)
			{
				if (boss1.CheckBulletCollision(*bullet))
				{
					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					boss1.Hurt();
					bullet->alive = false;
				}
			}
			for (Bullet* bullet : bullet_list)
			{
				if (now->number == 8 && boss2.yellow > 500 && boss2.red > 500 && boss2.CheckBulletCollision1(*bullet))
				{
					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					boss2.Hurt();
					bullet->alive = false;
				}
				if (boss2.yellow <= 500 && boss2.red > 500 && boss2.CheckBulletCollision2(*bullet))
				{
					mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					boss2.Hurt();
					bullet->alive = false;
				}
			}
			//移除生命值归零的敌人
			for (size_t i = 0; i < enemy1_list.size(); ++i)
			{
				Enemy1* enemy = enemy1_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy1_list[i], enemy1_list.back());
					enemy1_list.pop_back();
					delete enemy;
				}
			}
			for (size_t i = 0; i < enemy2_list.size(); ++i)
			{
				Enemy2* enemy = enemy2_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy2_list[i], enemy2_list.back());
					enemy2_list.pop_back();
					delete enemy;
				}
			}
			for (size_t i = 0; i < enemy3_list.size(); ++i)
			{
				Enemy3* enemy = enemy3_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy3_list[i], enemy3_list.back());
					enemy3_list.pop_back();
					delete enemy;
				}
			}
			for (size_t i = 0; i < enemy4_list.size(); ++i)
			{
				Enemy4* enemy = enemy4_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy4_list[i], enemy4_list.back());
					enemy4_list.pop_back();
					delete enemy;
				}
			}
			if (now->number == 3)
				if (!boss1.CheckAlive())
				{
					now->boss1a = 0;
					boss1.boss_pos = { -200 , -200 };
				}
			if (now->number == 8)
				if (!boss2.CheckAlive())
				{
					now->boss2a = 0;
					boss2.boss_pos = { -200 , -200 };
				}
			//移除子弹
			for (size_t i = 0; i < bullet_list.size(); ++i)
			{
				Bullet* bullet = bullet_list[i];
				if (!bullet->CheckAlive())
				{
					std::swap(bullet_list[i], bullet_list.back());
					bullet_list.pop_back();
					delete bullet;
				}
			}
			for (Enemy4* enemy : enemy4_list)
				for (size_t i = 0; i < enemy->enemybullet_list.size(); ++i)
				{
					EnemyBullet* enemybullet = enemy->enemybullet_list[i];
					if (!enemybullet->CheckAlive())
					{
						std::swap(enemy->enemybullet_list[i], enemy->enemybullet_list.back());
						enemy->enemybullet_list.pop_back();
						delete enemybullet;
					}
			}
			for (size_t i = 0; i < boss2.boss2_bullet.size(); ++i)
			{
				EnemyBullet* enemybullet = boss2.boss2_bullet[i];
				if (!enemybullet->CheckAlive())
				{
					//printf("dgwhegfjkw\n");
					std::swap(boss2.boss2_bullet[i], boss2.boss2_bullet.back());
					boss2.boss2_bullet.pop_back();
					delete enemybullet;
				}
			}
			//检测掉落物与玩家的碰撞
			for (Aword1* aword : aword1_list)
			{
				if (aword->CheckPlayerCollision1(player1) && player1.player1_alive)
				{
					aword->alive = false;
					if (player1_red < 440) player1_red += 100;
					else player1_blue += 150;
				}
				if (aword->CheckPlayerCollision2(player1) && player1.player2_alive)
				{
					aword->alive = false;
					if (player2_red < 440) player2_red += 100;
					else player2_blue += 150;
				}
			}
			for (Aword2* aword : aword2_list)
			{
				if (aword->CheckPlayerCollision1(player1) && player1.player1_alive)
				{
					aword->alive = false;
					player1_blue += 125;
				}
				if (aword->CheckPlayerCollision2(player1) && player1.player2_alive)
				{
					aword->alive = false;
					player2_blue += 125;
				}
			}
			//检测玩家与宝箱的碰撞
			if (now->number == 3 && !now->boss1a && !is_box_open)
			{
				if (player1.player1_pos.x >= 200 && player1.player1_pos.x <= 300 && player1.player1_pos.y >= 500 && player1.player1_pos.y <= 600)
					is_box_open = true;
				if (player1.player2_pos.x >= 200 && player1.player2_pos.x <= 300 && player1.player2_pos.y >= 500 && player1.player2_pos.y <= 600)
					is_box_open = true;
			}
			//清除掉落物
			for (size_t i = 0; i < aword1_list.size(); ++i)
			{
				Aword1* aword = aword1_list[i];
				if (!aword->alive)
				{
					std::swap(aword1_list[i], aword1_list.back());
					aword1_list.pop_back();
					delete aword;
				}
			}
			for (size_t i = 0; i < aword2_list.size(); ++i)
			{
				Aword2* aword = aword2_list[i];
				if (!aword->alive)
				{
					std::swap(aword2_list[i], aword2_list.back());
					aword2_list.pop_back();
					delete aword;
				}
			}

			//掉落物
			if (now->number == 1 && now->enemy1a == 0 && !now->aword)
			{
				now->aword = true;
				aword2_list.push_back(new Aword2());
				aword2_list.back()->end_pos = { 1000 , 600 };
				aword2_list.back()->aword_pos.x = 850;
				aword1_list.push_back(new Aword1());
				aword1_list.back()->end_pos = { 1000 , 600 };
			}
			if (now->number == 2 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy4a == 0 && !now->aword)
			{
				now->aword = true;
				aword2_list.push_back(new Aword2());
				aword2_list.back()->end_pos = { 1000 , 570 };
			}
			if (now->number == 3 && !now->boss1a && !now->aword && !is_box_open)
			{
				is_close_draw = true;
			}
			if (now->number == 3 && !now->boss1a && is_box_open)
			{
				is_open_draw = true;
				is_close_draw = false;
				if (!now->aword)
				{
					now->aword = true;
					aword2_list.push_back(new Aword2());
					aword2_list.back()->end_pos = { 1000 , 550 };
					aword2_list.back()->aword_pos = { 100 , 450 };
					aword2_list.push_back(new Aword2());
					aword2_list.back()->end_pos = { 1000 , 550 };
					aword2_list.back()->aword_pos = { 400 , 450 };
					aword1_list.push_back(new Aword1());
					aword1_list.back()->end_pos = { 1000 , 550 };
					aword1_list.back()->aword_pos = { 50 , 450 };
					aword1_list.push_back(new Aword1());
					aword1_list.back()->end_pos = { 1000 , 550 };
					aword1_list.back()->aword_pos = { 350 , 450 };
				}
			}
			if (now->number == 4 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0 && !now->aword)
			{
				now->aword = true;
				aword2_list.push_back(new Aword2());
				aword2_list.back()->end_pos = { 1000 , 590 };
				aword2_list.back()->aword_pos.x = 750;
				aword1_list.push_back(new Aword1());
				aword1_list.back()->end_pos = { 1000 , 590 };
			}
			if (now->number == 5 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0 && !now->aword)
			{
				now->aword = true;
				aword1_list.push_back(new Aword1());
				aword1_list.back()->end_pos = { 1000, 640 };
			}
			if (now->number == 6 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0 && !now->aword)
			{
				now->aword = true;
				aword1_list.push_back(new Aword1());
				aword1_list.back()->end_pos = { 1000, 640 };
			}
			if (now->number == 7 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0 && !now->aword)
			{
				now->aword = true;
				aword2_list.push_back(new Aword2());
				aword2_list.back()->end_pos = { 1000, 950 };
			}
			if (now->number == 8 && !now->boss2a && !now->aword)
			{
				now->aword = true;
				putimage_alpha(0, 0, &img_win);
			}
		}

		//地图切换
		if (player1.player1_pos.x == 0 && player1.player2_pos.x == 0)
		{
			printf("向左 ");
			if (now->number == 4 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->left;
				printf("%d ", now->number);
				player1.player1_pos.x = 1600;
				player1.player2_pos.x = 1600;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 5 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->left;
				printf("%d ", now->number);
				player1.player1_pos.x = 1600;
				player1.player2_pos.x = 1600;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 7 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->left;
				printf("%d ", now->number);
				player1.player1_pos.x = 1600;
				player1.player2_pos.x = 1600;
				if (!player1.is_move_w)
					player1.player1_pos.y = 670;
				if (!player1.is_move_up)
					player1.player2_pos.y = 670;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 8 && now->boss2a == 0)
			{
				now = now->left;
				printf("%d ", now->number);
				player1.player1_pos.x = 1600;
				player1.player2_pos.x = 1600;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 9)
			{
				now = now->left;
				printf("%d ", now->number);
				player1.player1_pos.x = 1600;
				player1.player2_pos.x = 1600;
				loadimage(&img_background, now->background);
			}
		}
		if (player1.player1_pos.x == WINDOW_WIDTH - player1.PLAYER1_WIDTH && player1.player2_pos.x == WINDOW_WIDTH - player1.PLAYER2_WIDTH)
		{
			printf("向右 ");
			if (now->number == 2 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->right;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 3 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->up;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 5 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->right;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				if (!player1.is_move_w)
					player1.player1_pos.y = 980;
				if (!player1.is_move_up)
					player1.player2_pos.y = 980;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 6 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->right;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 7 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->right;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				loadimage(&img_background, now->background);
			}
			else if (now->number == 8 && now->boss2a == 0)
			{
				now = now->right;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				if (!player1.is_move_w)
					player1.player1_pos.y = 900;
				if (!player1.is_move_up)
					player1.player2_pos.y = 900;
				loadimage(&img_background, now->background);
			}
		}
		if ((player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1300 && player1.player1_pos.y >= 750) || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1300 && player1.player2_pos.y >= 750))
		{
			printf("向下 ");
			if (now->number == 4 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->down;
				printf("%d ", now->number);
				player1.player1_pos.x = 1000;
				player1.player2_pos.x = 1000;
				player1.player1_pos.y = 100;
				player1.player2_pos.y = 100;
				loadimage(&img_background, now->background);
			}
		}
		if ((player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) >= 1150 && player1.player1_pos.x + (player1.PLAYER1_WIDTH / 2) <= 1370 && player1.player1_pos.y >= 750) || (player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) >= 1150 && player1.player2_pos.x + (player1.PLAYER2_WIDTH / 2) <= 1370 && player1.player2_pos.y >= 750))
		{
			printf("向下 ");
			if (now->number == 2 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->down;
				printf("%d ", now->number);
				player1.player1_pos.x = 1000;
				player1.player2_pos.x = 1000;
				player1.player1_pos.y = 100;
				player1.player2_pos.y = 100;
				loadimage(&img_background, now->background, 1700, 1000);
			}
		}
		if ((player1.player1_pos.y + (player1.PLAYER1_HEIGHT / 2) >= 950) || (player1.player2_pos.y + (player1.PLAYER2_HEIGHT / 2) >= 950))
		{
			if (now->number == 1 && now->enemy1a == 0 && now->enemy2a == 0 && now->enemy3a == 0 && now->enemy4a == 0)
			{
				now = now->down;
				printf("%d ", now->number);
				player1.player1_pos.x = 100;
				player1.player2_pos.x = 100;
				player1.player1_pos.y = 100;
				player1.player2_pos.y = 100;
				loadimage(&img_background, now->background);
			}
		}

		cleardevice();

		if (is_game_started)
		{
			//输出背景和玩家动画
			putimage_alpha(0, 0, &img_background);
			
			if (player1.player1_alive)
			{
				putimage_alpha(20, 20, &img_player1_ui);
				rectangle(140, 30, 440, 50);
				setfillcolor(RED);
				fillrectangle(140, 30, player1_red, 50);
				setfillcolor(BLUE);
				fillrectangle(140, 60, player1_blue, 80);
			}
			if (player1.player2_alive)
			{
				putimage_alpha(20, 110, &img_player2_ui);
				rectangle(140, 120, 440, 140);
				setfillcolor(RED);
				fillrectangle(140, 120, player2_red, 140);
				setfillcolor(BLUE);
				fillrectangle(140, 150, player2_blue, 170);
			}

			if (now->number == 3 && now->boss1a)
			{
				rectangle(500, 30, 1600, 50);
				setfillcolor(RED);
				fillrectangle(500, 30, boss1.red, 50);
				putimage_alpha(900, 60, &img_boss_ui);
			}

			if (now->number == 8 && now->boss2a)
			{
				rectangle(500, 30, 1600, 50);
				setfillcolor(RED);
				fillrectangle(500, 30, boss2.red, 50);
				setfillcolor(YELLOW);
				fillrectangle(500, 30, boss2.yellow, 50);
				putimage_alpha(900, 60, &img_boss_ui);
			}

			if (now->number == 9)
			{
				if (boss1xxx == 1 && boss2xxx == 0 && ((player1.player1_alive && player1_red != 440) || (player1.player2_alive && player2_red != 440)))
				{
					putimage_alpha(920, 455, &img_star);
					putimage_alpha(765, 455, &img_star);
				}
				else if (is_box_open && boss1xxx == 0 && boss2xxx == 0 && ((player1.player1_alive && player1_red != 440) || (player1.player2_alive && player2_red != 440)))
				{
					putimage_alpha(842, 455, &img_star);
				}
				else if (boss1xxx == 1 && boss2xxx == 0 && ((player1.player1_alive && player1_red == 440) || (player1.player2_alive && player2_red == 440)))
				{
					putimage_alpha(842, 455, &img_star);
					putimage_alpha(985, 455, &img_star);
					putimage_alpha(699, 455, &img_star);
				}
				else if (is_box_open && boss1xxx == 0 && boss2xxx == 0 && ((player1.player1_alive && player1_red == 440) || (player1.player2_alive && player2_red == 440)))
				{
					putimage_alpha(920, 455, &img_star);
					putimage_alpha(765, 455, &img_star);
					putimage_alpha(1060, 455, &img_star);
					putimage_alpha(630, 455, &img_star);
				}
				else if (!is_box_open && boss1xxx == 0 && boss2xxx == 0)
				{
					putimage_alpha(842, 455, &img_star);
					putimage_alpha(960, 455, &img_star);
					putimage_alpha(724, 455, &img_star);
					putimage_alpha(606, 455, &img_star);
					putimage_alpha(1078, 455, &img_star);
				}
			}

			if (now->number == 8 && boss2.yellow > 500) boss2.Draw1();
			if (now->number == 8 && boss2.yellow <= 500 && boss2.red > 500) boss2.Draw2(1000 / 144);

			if (is_close_draw && now->number == 3) putimage_alpha(200, 500, &img_box_close);;
			if (is_open_draw && now->number == 3) putimage_alpha(200, 500, &img_box_open);

			if (player1.player1_alive)
				player1.Draw1(1000 / 144);
			if (player1.player2_alive)
				player1.Draw2(1000 / 144);

			for (Enemy1* enemy : enemy1_list)
				enemy->Draw(1000 / 144);
			for (Enemy2* enemy : enemy2_list)
				enemy->Draw(1000 / 144);
			for (Enemy3* enemy : enemy3_list)
				enemy->Draw(1000 / 144);
			for (Enemy4* enemy : enemy4_list)
				enemy->Draw(1000 / 144);

			if (now->number == 3 && boss1.CheckAlive())
				boss1.Draw(1000 / 144);

			//子弹
			for (Bullet* bullet : bullet_list)
				bullet->Draw(1000 / 144);
			for (Enemy4* enemy : enemy4_list)
				for (EnemyBullet* enemybullet : enemy->enemybullet_list)
					enemybullet->Draw(1000 / 144);
			if (now->number == 8 && boss2.red > 500) 
				for (EnemyBullet* enemybullet : boss2.boss2_bullet)
					enemybullet->Draw(1000 / 144);

			for (Aword1* aword : aword1_list)
				aword->Draw();
			for (Aword2* aword : aword2_list)
				aword->Draw();
		}
		else
		{
			if (!is_choose_start && !is_achievement)
			{
				putimage_alpha(0, 0, &img_menu);
				btn_start_game.Draw();
				btn_quit_game.Draw();
				btn_achievement.Draw();
			}
			else if (is_choose_start && !is_achievement)
			{
				putimage_alpha(0, 0, &img_choose);
				btn_back_choose.Draw();
				btn_double_start.Draw();
				btn_box1.Draw();
				btn_box2.Draw();
			}
			else if (!is_choose_start && is_achievement)
			{
				putimage_alpha(0, 0, &img_achievement);
				btn_back_choose.Draw();
			}
		}

		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}

		if (game)
		{
			MessageBox(GetHWnd(), _T("pause"), _T("pause"), MB_OK);
			pause = false;
		}
	}

	delete atlas_player1_left;
	delete atlas_player2_left;
	delete atlas_player1_right;
	delete atlas_player2_right;
	delete atlas_player1_left_st;
	delete atlas_player2_left_st;
	delete atlas_player1_right_st;
	delete atlas_player2_right_st;
	delete atlas_enemy1_left;
	delete atlas_enemy2_left;
	delete atlas_enemy1_right;
	delete atlas_enemy2_right;
	delete atlas_enemy3_left;
	delete atlas_enemy3_right;
	delete atlas_boss1_left;
	delete atlas_boss1_right;
	delete atlas_bullet_left;
	delete atlas_bullet_right;

	delete atlas_botton;

	EndBatchDraw();

	return 0;
}
