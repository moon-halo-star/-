#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>

#define map_height 32
#define map_width 22
#define count_enimies 20      // 敌机的最大数量
#define count_bullets 100    // 子弹数量 
#define count_boss_bullets 100    // boss子弹上限 

// 子弹结构体
typedef struct {
    int x, y;
    int active;
} Bullet;

// 敌机结构体
typedef struct {
    int x, y;
    int active;
} Enemy;

//boss结构体
typedef struct {
    int x, y;
    int active;
    int health;
    int move_direction; // 1向右移动，-1向左移动 
} Boss;

//boss子弹结构体
typedef struct {
    int x, y;
    int active;
    int move_direction; // 子弹移动的方向
} Boss_bullet;

char input;
int difficulty_hard = 0;//仅在困难模式下等于1，代表小怪和boss同时存在 
int player_x = 19, player_y = 15;
int score = 0, health = 3;
int game_over = 0;
int frame_count = 0;
int boss_active = 0; // 判断Boss是否激活
int boss_spawn_score = 5; // 生成Boss需要的击杀数
int judge = 0; 
int last_shot_time = 0; // 上一次发射子弹的时间
int shot_cooldown = 5; // 发射冷却时间（帧数）
int choice=0;

Bullet bullets[count_bullets];
Enemy enemies[count_enimies];
Boss_bullet boss_bullets[count_boss_bullets]; 
Boss boss;

// 定位光标
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 隐藏光标
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void handle_input();
void update_bullets();
void update_enemies();
void update_boss_bullets();
void update_boss();
void check_collisions();
void draw_game();
void initialize_game();
void generate_enemy();
void shoot_bullet(int x, int y);
void clear_old_positions();
void draw_boss(); 
void spawn_boss();
void boss_shoot();
void display_menu();
void game_explain();
void Difficulty_adjustment();

int main()
{
    // 初始化数据 
    initialize_game();
    //进入游戏菜单 
    display_menu();
    system("cls");
    
    while(game_over == 0)
    {    
        frame_count++;
        
        // 输入
        handle_input();
        
        // 更新画面 
        update_bullets();
        update_enemies();
        
        // 更新Boss相关数据 
        if (boss_active) {
            update_boss();
            update_boss_bullets();
        }
        
        check_collisions();
        
        // 检查是否需要生成Boss
        if (boss_active == 0 && score >= boss_spawn_score && judge == 0) {
            spawn_boss();
            judge = 1;
        }
        
        // 清除上一帧的元素，画出当前帧的元素 
        draw_game();
        
        // 控制游戏速度
        Sleep(100);
    }
    
    system("cls");
    if (boss.health <= 0) {
        printf("恭喜！击败Boss，游戏胜利！\n") ; 
    } else {
        printf("game over!\n") ; 
    }
    printf("最终得分: %d\n", score);
    printf("按任意键退出...");
    _getch();
    
    return 0;
}

// 初始化游戏
void initialize_game()
{
    for(int i = 0; i < count_bullets; i++) {
        bullets[i].active = 0;
    }
    
    for(int i = 0; i < count_enimies; i++) {
        enemies[i].active = 0;
    }
    
    for(int i = 0; i < count_boss_bullets; i++) {
        boss_bullets[i].active = 0;
    }
    
    boss.active = 0;
    boss.health = 0;
    last_shot_time = 0;
    
    srand((unsigned int)time(NULL));//初始化随机数种子 
    hideCursor();
}

// 输入处理
void handle_input()
{
    if (_kbhit()) 
    {           
        input = _getch();
        
        // 玩家移动
        switch(input)
        {
            case 'w': case 'W':
                if(player_x > 1) player_x--;
                break;
            case 's': case 'S':
                if(player_x < map_width-2) player_x++;
                break;
            case 'a': case 'A':
                if(player_y > 1) player_y--;
                break;
            case 'd': case 'D':
                if(player_y < map_height-2) player_y++;
                break;
            case ' ':  // 发射子弹
                // 检查冷却时间
                if (frame_count - last_shot_time >= shot_cooldown) {
                    shoot_bullet(player_x, player_y);
                    last_shot_time = frame_count;
                }
                break;
        }
    }
}

// 发射子弹
void shoot_bullet(int x, int y)
{
    for(int i = 0; i < count_bullets; i++) {
        if(bullets[i].active==0) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].active = 1;
            break;
        }
    }
}

// 更新子弹
void update_bullets()
{
    for(int i = 0; i < count_bullets; i++) {
        if(bullets[i].active==1) {
            bullets[i].x--;
            
            // 子弹到达顶边，消失
            if(bullets[i].x <= 0) {
                bullets[i].active = 0;
            }
        }
    }
}

// 更新敌机
void update_enemies()
{
    // 随机生成新敌机
    if(rand()%2==0 && !boss_active||difficulty_hard  == 1) { // 非困难模式下，Boss出现时停止生成小怪 
        generate_enemy();
    }
    
    // 更新活跃敌机
    for(int i = 0; i < count_enimies; i++) {
        if(enemies[i].active) {
            enemies[i].x++;
            
            // 敌机到达底边时消失
            if(enemies[i].x >= map_width-1) {
                enemies[i].active = 0;
            }
        }
    }
}

// 生成新敌机
void generate_enemy()
{
    for(int i = 0; i < count_enimies; i++) {
        if(enemies[i].active==0) {
            enemies[i].x = 1;
            enemies[i].y = rand() % (map_height-2) + 1;
            enemies[i].active = 1;
            break;
        }
    }
}

// 生成Boss
void spawn_boss()
{
    boss.x = 2;
    // 调整初始Y位置，确保不会靠近右边界
    boss.y = (map_height / 2) - 1;
    boss.active = 1;
    boss.health = 5;
    boss.move_direction = 1; // 初始向右移动
    boss_active = 1;
}

// 更新Boss
void update_boss()
{
    if (!boss.active) return;
    
    // Boss水平移动
    boss.y += boss.move_direction;
    
    // 精确的边界检测，考虑Boss的完整宽度(3个字符)
    // Boss的绘制范围是 [boss.y-1, boss.y+1]，所以需要检测这个范围是否超出边界
    if (boss.y - 1 <= 1) {
        // 左边界检测：当Boss的左边缘( boss.y-1 )碰到左边界
        boss.move_direction = 1; // 强制向右移动
        boss.y = 2; // 确保不会卡在边界
    } else if (boss.y + 1 >= map_height - 2) {
        // 右边界检测：当Boss的右边缘( boss.y+1 )碰到右边界
        boss.move_direction = -1; // 强制向左移动
        boss.y = map_height - 3; // 确保不会卡在边界
    }
    
    // Boss射击
    if (frame_count % 10 == 0) { // 每10帧射击一次
        boss_shoot();
    }
}

// Boss射击 - 弹幕攻击
void boss_shoot()
{
    // 创建扇形弹幕
    int directions[] = {-2, -1, 0, 1, 2}; // 5个方向的弹幕
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < count_boss_bullets; j++) {
            if (!boss_bullets[j].active) {
                boss_bullets[j].x = boss.x + 1;
                boss_bullets[j].y = boss.y;
                boss_bullets[j].active = 1;
                boss_bullets[j].move_direction = directions[i];
                break;
            }
        }
    }
}

// 更新Boss子弹
void update_boss_bullets()
{
    for (int i = 0; i < count_boss_bullets; i++) {
        if (boss_bullets[i].active) {
            boss_bullets[i].x++; // 向下移动
            boss_bullets[i].y += boss_bullets[i].move_direction; // 水平移动
            
            // 超出边界消失
            if (boss_bullets[i].x >= map_width-1 || boss_bullets[i].y <= 0 || boss_bullets[i].y >= map_height-1) {
                boss_bullets[i].active = 0;
            }
        }
    }
}

// 检测碰撞
void check_collisions()
{
    // 检测子弹与敌机的碰撞
    for(int i = 0; i < count_bullets; i++) {
        if(bullets[i].active) {
            for(int j = 0; j < count_enimies; j++) {
                if(enemies[j].active ==1 && (bullets[i].x == enemies[j].x||bullets[i].x+1 == enemies[j].x)  &&
                 bullets[i].y == enemies[j].y) {
                    score++;
                    bullets[i].active = 0;
                    enemies[j].active = 0;
                    break;
                }
            }
            
            // 检测子弹与Boss的碰撞
            if (boss.active && bullets[i].active) {
                if ((bullets[i].x == boss.x || bullets[i].x == boss.x + 1) && 
                    bullets[i].y >= boss.y - 1 && bullets[i].y <= boss.y + 1) {
                    boss.health--;
                    bullets[i].active = 0;
                    
                    if (boss.health <= 0) {
                        boss.active = 0;
                        boss_active = 0;
                        score += 5; // 击败Boss额外加分
                        game_over = 1; // Boss死后触发游戏结束
                    }
                    break;
                }
            }
        }
    }
    
    // 检测玩家与敌机的碰撞
    for(int i = 0; i < count_enimies; i++) {
        if(enemies[i].active == 1 && player_x == enemies[i].x && player_y == enemies[i].y) {
            health--;
            enemies[i].active = 0;
            
            if(health <= 0) {
                game_over = 1;
            }
            break;
        }
    }
    
    // 检测玩家与Boss子弹的碰撞
    for (int i = 0; i < count_boss_bullets; i++) {
        if (boss_bullets[i].active && player_x == boss_bullets[i].x && player_y == boss_bullets[i].y) {
            health--;
            boss_bullets[i].active = 0;
            
            if (health <= 0) {
                game_over = 1;
            }
            break;
        }
    }
    
    // 检测玩家与Boss的碰撞
    if (boss.active && 
        ((player_x == boss.x || player_x == boss.x + 1) && 
         player_y >= boss.y - 1 && player_y <= boss.y + 1)) {
        health--;
        if (health <= 0) {
            game_over = 1;
        }
    }
}

// 清除上一帧的位置
void clear_old_positions()
{
    static int last_player_x = 0, last_player_y = 0;
    static Bullet last_bullets[count_bullets];
    static Enemy last_enemies[count_enimies];
    static Boss last_boss;
    static Boss_bullet last_boss_bullets[count_boss_bullets];
    
    if(last_player_x != 0 && last_player_y != 0) {
        setCursorPosition(last_player_y, last_player_x);
        printf(" ");
    }
    
    for(int i = 0; i < count_bullets; i++) {
        if(last_bullets[i].active == 1) {
            setCursorPosition(last_bullets[i].y, last_bullets[i].x);
            printf(" ");
        }
    }
    
    for(int i = 0; i < count_enimies; i++) {
        if(last_enemies[i].active == 1) {
            setCursorPosition(last_enemies[i].y, last_enemies[i].x);
            printf(" ");
        }
    }
    
    // 优化Boss清除：确保完全清除Boss的所有部分
    if (last_boss.active) {
        // 清除Boss可能占用的所有位置
        // 清除第一行 
    setCursorPosition(last_boss.y-1, last_boss.x-1);
    printf("   ");
    
    // 清除第二行 
    setCursorPosition(last_boss.y, last_boss.x);
    printf(" ");
    
    // 清除第三行 
    setCursorPosition(last_boss.y-1, last_boss.x+1);
    printf("   ");
    
    // 额外清除可能残留的字符
    setCursorPosition(last_boss.y, last_boss.x);
    printf(" ");
    setCursorPosition(last_boss.y, last_boss.x + 2);
    printf(" ");
    }
    
    // 清除Boss子弹
    for (int i = 0; i < count_boss_bullets; i++) {
        if (last_boss_bullets[i].active) {
            setCursorPosition(last_boss_bullets[i].y, last_boss_bullets[i].x);
            printf(" ");
        }
    }
    
    // 更新玩家位置
    last_player_x = player_x;
    last_player_y = player_y;
    
    for(int i = 0; i < count_bullets; i++) {
        last_bullets[i] = bullets[i];
    }
    
    for(int i = 0; i < count_enimies; i++) {
        last_enemies[i] = enemies[i];
    }
    
    last_boss = boss;
    
    for (int i = 0; i < count_boss_bullets; i++) {
        last_boss_bullets[i] = boss_bullets[i];
    }
}

// 绘制Boss
void draw_boss() {
    if (!boss.active) return;
    
    // 确保Boss绘制在有效范围内
    int draw_y = boss.y;
    if (draw_y < 2) draw_y = 2;
    if (draw_y > map_height - 3) draw_y = map_height - 3;
    
    // 绘制Boss主体 (3x3大小)
    setCursorPosition(draw_y-1, boss.x-1);
    printf("[ ]");
    setCursorPosition(draw_y, boss.x );
    printf("B");
    setCursorPosition(draw_y-1, boss.x+1);
    printf("{ }");
    
    // 显示Boss血量
    setCursorPosition(map_height + 2, 0);
    printf("Boss血量: ");
    for (int i = 0; i < 5; i++) {
        if (i < boss.health) {
            printf("[]");
        } else {
            printf(" ");
        }
    }
}

// 绘制游戏
void draw_game()
{
    // 清除旧位置
    clear_old_positions();
    
    // 绘制玩家
    setCursorPosition(player_y, player_x);
    printf("S");
    
    // 绘制活跃子弹
    for(int i = 0; i < count_bullets; i++) {
        if(bullets[i].active == 1) {
            setCursorPosition(bullets[i].y, bullets[i].x);
            printf("*");
        }
    }
    
    // 绘制活跃敌机
    for(int i = 0; i < count_enimies; i++) {
        if(enemies[i].active) {
            setCursorPosition(enemies[i].y, enemies[i].x);
            printf("#");
        }
    }   
    
    // 绘制Boss子弹
    for (int i = 0; i < count_boss_bullets; i++) {
        if (boss_bullets[i].active) {
            setCursorPosition(boss_bullets[i].y, boss_bullets[i].x);
            printf("o");
        }
    }
    
    // 绘制Boss
    draw_boss();
    
    // 绘制边框（只一次）
    if(frame_count == 1)
    {
        for(int i = 0; i < map_width; i++)
        {
            for(int t = 0; t < map_height; t++)
            {
                if(i == 0 || i == map_width-1 || t == 0 || t == map_height-1)
                {
                    setCursorPosition(t, i);
                    printf("*");
                } 
            }
        }
    }
    
    setCursorPosition(0, map_width);
    printf("血量: %d  得分: %d ", health, score);
    setCursorPosition(0, map_width + 1);
    printf("控制: WASD移动, 空格发射");
    
    // 显示冷却状态
    int cooldown_remaining = shot_cooldown - (frame_count - last_shot_time);
    if (cooldown_remaining > 0) {
        setCursorPosition(0, map_width + 2);
        printf("子弹冷却: %d ", cooldown_remaining);
    } else {
        setCursorPosition(0, map_width + 2);
        printf("子弹就绪!        ");
    }
    
    // 显示Boss提示
    if (!boss_active && score >= boss_spawn_score-1) {
        setCursorPosition(0, map_width + 3);
        printf("Boss即将出现！击败Boss即可获胜！");
    }
}
void display_menu() {
    system("cls");  
    printf("========== 主菜单 ==========\n");
    printf("1. 开始游戏\n");
    printf("2. 游戏说明\n");
    printf("3. 难度调节\n");
    printf("4. 退出游戏\n");
    printf("============================\n");
    printf("请选择 (1-4): ");
    scanf("%d", &choice);
    switch(choice) {
            case 1:
                return;
                break;
            case 2:
                game_explain();
                break;
            case 3:
                Difficulty_adjustment();
                break;
            case 4:
            	system("cls");
            	exit(0);
            	break;
            default:
            	system("cls");
                printf("无效选择，请重新输入！\n");
                Sleep(1000);
                display_menu();
        }
    
}
void game_explain()
{	
	system("cls");
	printf("控制: WASD移动, 空格发射");
	printf("按任意键返回");
	_getch(); 
	display_menu(); 
}
void Difficulty_adjustment(){
	system("cls");
	int difficulty; 
	printf("1为简单\n2为中等\n3为困难");
	scanf("%d",&difficulty);
	switch(difficulty) {
        case 1:   // 简单模式
            boss_spawn_score = 8;       
            shot_cooldown = 3;          
            health = 5;                 
            break;
            
        case 2: 
            boss_spawn_score = 5;
            shot_cooldown = 5;
            health = 3;
            break;
            
        case 3:   
            difficulty_hard = 1; 
            boss_spawn_score = 3;       // 很快出现Boss
            shot_cooldown = 8;          // 射击冷却长
            health = 2;                 // 玩家血量少
            break;
    }
}
