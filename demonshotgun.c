
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

typedef long long ll;

//枚举道具类型TOOLS
enum TOOLS {
    SMOKE,
    HAND_C,
    MAGNIFYING_GLASS,
    KNIFE,
    DRINKS
};

//用结构体保存玩家信息
struct PLAYER_INFO {
    char name[100]; // 存储玩家的名字
    ll life; // 玩家生命值
    ll tls[5]; // 玩家的五种道具
} player, dealer;

struct BULLETS_INFO{
	ll all_bullets;
	ll tru_bullets;
	ll emp_bullets;
	ll nw;
	ll nw_damage;
}bullet;

//全局变量声明
ll max_life; // 最大生命值
ll R(ll left_boundary, ll right_boundary);
ll player_tools=0,dealer_tools=0,max_tools = 8;
bool now_gun = true, nxt_gun = false; // now_gun表示当前玩家的枪权，nxt_gun表示玩家下一回合是否拿枪
bool AI_usedHandcuffs = false,Player_usedHandcuffs=false; // 记录是否已经使用了手铐
bool continueTurn = false,hasAttacked = false;//庄家状态
int *Bullets; // 动态分配内存

// 函数声明
void Introduction();
void GiveOutTools();
void printOutTools();
void reloadOutTools();
void Init_Bullets();
bool Round();
void Do_Dealer_AI();
void SomeOneDead();

int main()
{
	srand(time(NULL));
	bullet.nw_damage=1;
    max_life = R(2, 6); // 生成随机的最大生命值
    player.life = max_life; // 设置玩家的生命值
    dealer.life = max_life; // 设置庄家的生命值
	Init_Bullets();//定义子弹
    Introduction();//启示画面
    GiveOutTools();//刷新道具
    while (Round())
    {
    }
    free(Bullets); // 释放动态分配的内存
    return 0;
}

void Introduction() {
    // 设置控制台颜色为红色
    system("color 4");

    // 打印游戏标题
    printf("===========================================\n");
    printf("\n");
    printf("              恶 魔 轮 盘 \n");
    printf("\n");
    printf("===========================================\n");
    Sleep(500); // 暂停0.5秒

    // 清屏
    system("cls");
    Sleep(1000);

    // 获取玩家名字
    printf("===========================================\n");
    printf("\n");
    printf("           输入您的名字：\n");
    printf("\n");
    printf("===========================================\n");
    printf("Enter:");
    fgets(player.name, sizeof(player.name), stdin);
    player.name[strcspn(player.name, "\n")] = 0; // 去除换行符
    Sleep(1000);
    system("cls");
    Sleep(1000);

    // 打印欢迎信息
    printf("===========================================\n");
    printf("\n");
    printf("            欢迎赌命:%s\n", player.name);
    printf("\n");
    printf("===========================================\n");
    Sleep(500);
    system("cls");
    Sleep(500);

    // 初始化随机数生成器
    srand((unsigned int)time(NULL));

    // 显示子弹数和生命信息
    printf("===========================================\n");
    printf("\n");
    printf("本回合子弹数量:%lld 发\n", bullet.all_bullets);
    printf("每轮真弹数量：%lld 发\n", bullet.tru_bullets);
    printf("每轮虚弹数量：%lld 发\n", bullet.emp_bullets);
    printf("你的生命值：%lld / %lld 滴\n", player.life,max_life);
    printf("对手的生命值：%lld / %lld 滴\n", dealer.life,max_life);
    printf("\n");
    printf("===========================================\n");
    Sleep(3500);
    system("cls");
    Sleep(1000);
}

void Debug_CheakBullet(){
	int i;
	for(i=0;i<bullet.all_bullets;i++){
		printf("%lld ",Bullets[i]);
	}
	printf("\n");
}


// 生成[left_boundary, right_boundary]之间的随机数
ll R(ll left_boundary, ll right_boundary) {
    return left_boundary + rand() % (right_boundary - left_boundary + 1);
}

//生成子弹
void Init_Bullets() {
    if (Bullets) {
        free(Bullets); // 释放之前分配的内存空间
    }

    // 确定子弹总数
    bullet.all_bullets = R(2,8); // 假设子弹总数在2到8之间

    // 确定实弹的数量，确保实弹和虚弹的数量差不超过1到2颗
    // 实弹数量设置为子弹总数的一半，向上取整
    bullet.tru_bullets = (bullet.all_bullets + 1) / 2;
    bullet.emp_bullets = bullet.all_bullets - bullet.tru_bullets; // 计算虚弹数量

    // 分配足够容纳所有子弹的内存空间
    Bullets = (int *)malloc(bullet.all_bullets * sizeof(int));
    if (Bullets == NULL) {
        printf("内存分配失败！程序退出...\n");
        free(Bullets);
        exit(EXIT_FAILURE);
    }

    // 初始化子弹数组，随机填充实弹和虚弹
    for (ll i = 0; i < bullet.tru_bullets; i++) {
        Bullets[i] = 1; // 第i发是实弹
    }
    for (ll i = bullet.tru_bullets; i < bullet.all_bullets; i++) {
        Bullets[i] = 0; // 剩余的是虚弹
    }

    // 随机打乱子弹顺序
    for (ll i = 0; i < bullet.all_bullets; i++) {
        ll j = rand() % bullet.all_bullets;
        int temp = Bullets[i];
        Bullets[i] = Bullets[j];
        Bullets[j] = temp;
    }
}


void reloadOutTools(){   // 子弹和道具刷新
    if (bullet.nw>=bullet.all_bullets || bullet.tru_bullets + bullet.emp_bullets <= 0) {

    	printf("本轮回合已结束\n");
		system("cls");
		Init_Bullets(); // 重新初始化子弹
        GiveOutTools(); // 重新分配道具
        printf("===========================================\n");
        printf("事件:子弹和道具已经刷新\n");
        printf("===========================================\n");
        printf("剩余 %lld 发实弹和 %lld 发空弹\n", bullet.tru_bullets, bullet.emp_bullets);
        printf("===========================================\n");
        bullet.nw=0;
        now_gun = true;
        nxt_gun = false;
        AI_usedHandcuffs = false;
		Player_usedHandcuffs=false;
		continueTurn = false;
        Sleep(5000);
        system("cls");
        Round();
    }
}

//道具的刷新
void GiveOutTools() {
    ll r = R(2, 4); // 刷r个道具 2 - 4

    for (ll i = 0; i < r; i++) {
        if (player_tools >= max_tools) {
            printf("太可惜了，你的道具满了,你没有更多的道具\n");
        } else {
            ll tr = R(0, 4);
            player.tls[tr]++;
            player_tools++;
        }
    }

    for (ll i = 0; i < r; i++) {
        if (dealer_tools >= max_tools) {
            printf("庄家的道具满了，没有更多的道具\n");
        } else {
            ll tr = R(0, 4);
            dealer.tls[tr]++;
            dealer_tools++;
        }
    }
}

//输出双方道具量
void printOutTools()
{
	if (strcmp(player.name, "HACK") == 0 || strcmp(player.name, "hack") == 0) {
    Debug_CheakBullet();
}
	printf("===========================================\n");
    printf("%s 生命值：%lld / %lld 滴\n", player.name,player.life,max_life);
    printf("对手 生命值：%lld / %lld 滴\n", dealer.life,max_life);
    printf("===========================================\n");
    printf("您的道具：\n");
    printf("1.华子 %lld 个\n", player.tls[0]);
    printf("2.手铐 %lld 个\n", player.tls[1]);
    printf("3.放大镜 %lld 个\n", player.tls[2]);
    printf("4.小刀 %lld 个\n", player.tls[3]);
    printf("5.饮料 %lld 个\n", player.tls[4]);
    printf("===========================================\n");
    printf("庄家道具：\n");
    printf("1.华子 %lld 个\n", dealer.tls[0]);
    printf("2.手铐 %lld 个\n", dealer.tls[1]);
    printf("3.放大镜 %lld 个\n", dealer.tls[2]);
    printf("4.小刀 %lld 个\n", dealer.tls[3]);
    printf("5.饮料 %lld 个\n", dealer.tls[4]);
    printf("===========================================\n");
}

void SomeOneDead(){
	if (dealer.life <= 0)
	{
	system("cls");
	printf("====================牛=====================\n");
	Sleep(1000);
	printf("对方剩余 %lld 滴血，没血力\n", dealer.life);
	Sleep(1000);
	printf("                 对手挂了\n");
	Sleep(1000);
	printf("====================牛=====================\n");
	free(Bullets);
	system("pause");
	exit(0);
	}
	if (player.life <= 0) {
    system("cls");
    printf("====================寄=====================\n");
    Sleep(1000);
    printf("                  你挂了\n");
    Sleep(1000);
    printf("====================寄=====================\n");
    free(Bullets);
    system("pause");
    exit(0);
    }

}

//回合信息
bool Round()
{
    // 在此定义 ReUse 标签
ReUse:
    if (now_gun)
    {
    	AI_usedHandcuffs = false;
    	printf("===========================================\n");
		printf("玩家回合：\n");
		printf("===========================================\n");
		printOutTools();
        printf("===========================================\n");
        printf("输入1  使用道具\n输入2  直接开枪\n");
        printf("===========================================\n");
        printf("Enter:");
        ll slt;//定义选择数字
        scanf("%lld", &slt);
        while (slt != 1 && slt != 2){
		scanf("%lld", &slt);
		}
        if (slt == 1)
        {
ReUseTools:
            system("cls");
            printf("玩家回合：\n");
            printOutTools();
            printf("请按顺序输入道具使用序列，道具之间空格隔开(输入中文)\n");
            printf("Enter:");
            char Use_tls[1000];
            scanf("%s", Use_tls);//保存玩家输入的道具名
            char *token = strtok(Use_tls, " ");//以 *token划分Use_tls的名称
            while (token != NULL)//直到token为NULL时停止
            {
            	//玩家华子逻辑
                if (strcmp(token, "华子") == 0||strcmp(token, "1") == 0) // 如果加一滴血后不超过生命上限且有华子道具
                {
                	system("cls");
                		if(player.tls[SMOKE]>0)
						{
		                	if(player.life >= max_life)//如果生命值已满
							{
							printf("玩家回合：\n");
							printOutTools();
		                    printf("你感到很轻松，但是你的血量已经满了\n");
		                    player.tls[0]--; // 使用华子道具
		                    player_tools--;
		                    printf("华子-1\n");
		                    printf("===========================================\n");
							}
							else if (player.life < max_life) // 如果生命未满
		                    {
		                        player.tls[0]--; // 使用华子道具
		                        player.life++; // 玩家加一滴血
		                        player_tools--;
		                        printf("玩家回合：\n");
		      					printf("===========================================\n");
						        printf("%s 剩余血量 %lld 滴\n", player.name, player.life);
						        printf("庄家 剩余血量 %lld 滴\n", dealer.life);
								printOutTools();
		                        printf("你使用了华子回复一滴血\n");
		                        printf("华子-1\n");
		                        printf("===========================================\n");
		                    }
		                }else
						{
							printf("玩家回合：\n");
							printOutTools();
							printf("你没有华子！\n");
							printf("===========================================\n");
						}
                    Sleep(1500);
					system("cls");
                    goto ReUse;
                }
                //玩家手铐逻辑
				if (strcmp(token, "手铐") == 0||strcmp(token, "2") == 0)
				{
					system("cls");
				    if (player.tls[HAND_C] > 0 && !nxt_gun && !Player_usedHandcuffs)
				    {
				        player.tls[HAND_C]--; // 使用手铐道具
				        player_tools--;
				        printf("玩家回合：\n");
					    printf("===========================================\n");
				        printf("你使用了手铐，对方下一回合无法操作\n");
				        printf("手铐-1\n");
				        printf("===========================================\n");
						nxt_gun = true;
						Player_usedHandcuffs = true;
				    }
				    else if(nxt_gun)
				    {
						printf("玩家回合：\n");
						printf("===========================================\n");
				        printf("你这一回合已经使用过手铐了！无法再次使用\n");
				        printf("===========================================\n");
				    }
				    else if(player.tls[HAND_C] == 0){
						printf("玩家回合：\n");
						printf("===========================================\n");
				        printf("你没有手铐可以使用！\n");
				        printf("===========================================\n");
					}else if(Player_usedHandcuffs){
						printf("玩家回合：\n");
						printf("===========================================\n");
				        printf("你这一回合已经使用过手铐了！无法再次使用\n");
				        printf("===========================================\n");
					}
					Sleep(1500);
					system("cls");
					goto ReUse;
				}
				if (strcmp(token, "放大镜") == 0||strcmp(token, "3") == 0)
                {
                	system("cls");
                    if (player.tls[2]>0)
                    {
						printf("玩家回合：\n");
						player.tls[2]--;
						player_tools--;
						printOutTools();
                        printf("当前子弹为: %s\n", (Bullets[bullet.nw] == 1 ? "实弹" : "空弹"));
                        printf("放大镜-1\n");
                        printf("===========================================\n");
                    }
                    else{
						printf("玩家回合：\n");
      					printOutTools();
						printf("你没有足够的放大镜！\n");
						printf("===========================================\n");
					}
					Sleep(1500);
					system("cls");
					goto ReUse;
                }
                if (strcmp(token, "小刀") == 0||strcmp(token, "4") == 0)
                {
                	system("cls");
                    if (player.tls[3]>0)
                    {
                        player.tls[3]--;
                        player_tools--;
						bullet.nw_damage = 2;
                        printf("===========================================\n");
                        printf("玩家回合：\n");
                        printf("===========================================\n");
                        printf("已使用小刀，枪的伤害加倍了\n");
                        printf("小刀-1\n");
                        printf("===========================================\n");
                    }
                    else{
                    	printf("===========================================\n");
				    	printf("玩家回合：\n");
				    	printf("===========================================\n");
						printf("你没有足够的小刀！\n");
				        printf("===========================================\n");
					}
     			    Sleep(1500);
					system("cls");
					goto ReUse;
                }
                if (strcmp(token, "饮料") == 0||strcmp(token, "5") == 0)
				{
					system("cls");
				    if (player.tls[DRINKS] > 0)
				    {
						player.tls[DRINKS]--; // 使用饮料道具
				        player_tools--;
				        if (Bullets[bullet.nw] == 1)
				        {
				            bullet.tru_bullets--; // 如果退的是实弹，实弹数量减一
				        }
				        else if(Bullets[bullet.nw] == 0)
				        {
				            bullet.emp_bullets--; // 如果退的是空弹，空弹数量减一
				        }
				        printf("===========================================\n");
				        printf("你使用了饮料退弹一颗，退掉的是%s\n", (Bullets[bullet.nw] == 1 ? "实弹" : "空弹"));
				        printf("饮料-1\n");
				        printf("===========================================\n");
						bullet.nw++; // 更新子弹索引
				        Sleep(1500);
				        system("cls");
				    }
				    else if (player.tls[DRINKS] <= 0){
				    	printf("玩家回合：\n");
				    	printOutTools();
						printf("你没有足够的饮料道具来退弹\n");
				        printf("===========================================\n");
				        Sleep(1500);
				        system("cls");
				    }
				    goto ReUse; // 如果玩家选择退弹，跳回ReUse标签，继续游戏
				}
                token = strtok(NULL, " ");//分割出玩家输入的空格
            }
        }
        printf("你要向谁开枪（1表示自己，2表示对手）：\n");
        printf("Enter:");
        scanf("%lld", &slt);

        while (slt != 1 && slt != 2)
		{
            scanf("%lld", &slt);
		}

        if (slt == 1)//玩家选择攻击自己
        {
        	system("cls");
            if (Bullets[bullet.nw] == 1)
            {
				player.life -= bullet.nw_damage;
            	printf("===========================================\n");
            	printf("玩家回合：\n");
                printf("很不辛，你被自己开出的实弹命中 ，- %lld 滴血，剩余 %lld 滴血\n", bullet.nw_damage, player.life);
                printf("===========================================\n");
		        now_gun = nxt_gun; //是否使用过手铐，根据此规则判断枪权
		        nxt_gun = !nxt_gun;
                bullet.tru_bullets--;
                bullet.nw_damage = 1;
                bullet.nw++;
				SomeOneDead();
                Sleep(1000);
                system("cls");
            }
            else if (Bullets[bullet.nw] == 0)
            {
            	printf("===========================================\n");
				printf("玩家回合：\n");
				printf("===========================================\n");
                printf("         是空弹，下一轮还是你\n");
                printf("===========================================\n");
                bullet.emp_bullets--;
                bullet.nw++;
                bullet.nw_damage = 1;
				Sleep(1500);
				system("cls");
            }
        }
        // 在 Round() 函数中处理玩家攻击庄家后的逻辑
			if (slt == 2)//玩家选择攻击庄家
			{
				system("cls");
			    if (Bullets[bullet.nw] == 1)
			    {
			        // 实弹：庄家扣血
			        printf("===========================================\n");
			        printf("是实弹，庄家扣血 %lld 滴\n", bullet.nw_damage);
			        printf("===========================================\n");
			        dealer.life -= bullet.nw_damage;
			        bullet.tru_bullets--;
					bullet.nw++;
				    bullet.nw_damage = 1;
				    now_gun = nxt_gun; //是否使用过手铐，根据此规则判断枪权
			        nxt_gun = !nxt_gun;
			        SomeOneDead();
			    }
			    else if(Bullets[bullet.nw] == 0)
			    {
					printf("===========================================\n");
			        printf("是空弹，攻击未造成伤害\n");
			        printf("===========================================\n");
			        bullet.emp_bullets--;
			        bullet.nw++;
			        bullet.nw_damage = 1;
			        now_gun = nxt_gun; //是否使用过手铐，根据此规则判断枪权
		        	nxt_gun = !nxt_gun;
     			  }
			    Sleep(1500);
				system("cls");
			}
        //检测是否刷新道具和子弹
	    reloadOutTools();
    }
    else if(!now_gun)
    {
        Do_Dealer_AI();
    }
    return true;
	}

void Do_Dealer_AI() {
    // 庄家的回合信息
    Player_usedHandcuffs = false;
    printf("庄家的回合：\n");
    printOutTools();
    Sleep(2000);

    // 检查庄家的健康状况并使用华子（Smoke）恢复血量
    while (dealer.tls[SMOKE] > 0 && dealer.life < max_life) {
        dealer.tls[SMOKE]--;
        dealer_tools--;
        dealer.life++;
        printf("庄家使用了华子，血量恢复到 %d\n", dealer.life);
        Sleep(2000);
    }

    continueTurn = true;
    hasAttacked = false;

    while (continueTurn) {
        continueTurn = false;

        // 检查是否只剩下实弹或虚弹
        if (bullet.tru_bullets == 0 || bullet.emp_bullets == 0) {
            if (bullet.emp_bullets == 0) {
                // 使用手铐（Handcuffs）
                if (!AI_usedHandcuffs && dealer.tls[HAND_C] > 0 && bullet.tru_bullets >= 2) {
                    dealer.tls[HAND_C]--;
                    dealer_tools--;
                    printf("庄家使用了手铐，你下一回合无法操作\n");
                    nxt_gun = false;
                    Sleep(2000);
                    AI_usedHandcuffs = true;
                    continueTurn = true; // 手铐后继续当前回合
                    continue;
                }

                // 使用小刀（Knife）
                if (bullet.tru_bullets >= 1 && dealer.tls[KNIFE] > 0 && bullet.emp_bullets == 0) {
                    dealer.tls[KNIFE]--;
                    dealer_tools--;
                    bullet.nw_damage = 2; // 伤害加倍
                    printf("庄家使用了小刀，下一次攻击伤害加倍\n");
                    Sleep(2000);
                }

                // 攻击玩家
                if (bullet.tru_bullets > 0) {
                    printf("庄家选择攻击玩家\n");
	                Sleep(1000);
	                printf("实弹，玩家血量 -%lld\n", bullet.nw_damage);
                    player.life -= bullet.nw_damage;
                    bullet.tru_bullets--;
                    bullet.nw++;
                    now_gun = nxt_gun;
                    nxt_gun = !nxt_gun;
                    hasAttacked = true;
                    bullet.nw_damage = 1;
					Sleep(2000);
					SomeOneDead();
                    reloadOutTools();
                }
            } else if (bullet.tru_bullets == 0) {
                printf("庄家对自己开枪\n");
                Sleep(1000);
                printf("空弹，未造成伤害\n");
                bullet.emp_bullets--;
                bullet.nw++;
                continueTurn = true;
                bullet.nw_damage = 1;
				Sleep(2000);
                reloadOutTools();
                continue;
            }
        }

        // 检查并使用饮料（Drinks）退弹
        while (dealer.tls[DRINKS] > 0 && (bullet.tru_bullets > 1 || bullet.emp_bullets > 1)) {
            dealer.tls[DRINKS]--;
            dealer_tools--;
            if (Bullets[bullet.nw] == 1) {
                bullet.tru_bullets--;
            } else if (Bullets[bullet.nw] == 0) {
                bullet.emp_bullets--;
            }
            printf("庄家使用了饮料退弹一颗\n");
            printf("退下的子弹是%s\n", (Bullets[bullet.nw] == 1 ? "实弹" : "空弹"));
            bullet.nw++; // 子弹索引增加，模拟退弹操作
            continueTurn = true;
            Sleep(2000);
			if (continueTurn) {
            continue;
        }//返回到循环体起始位置，检查是否已经满足bullet.tru_bullets == 0 || bullet.emp_bullets == 0的情况
    }

        // 使用手铐（Handcuffs）
        if (!AI_usedHandcuffs && dealer.tls[HAND_C] > 0 && bullet.tru_bullets + bullet.emp_bullets > 2) {
            dealer.tls[HAND_C]--;
            dealer_tools--;
            printf("庄家使用了手铐，你下一回合无法操作\n");
            nxt_gun = false;
            Sleep(2000);
            AI_usedHandcuffs = true;
            continueTurn = true; // 手铐后继续当前回合
            continue;
        }

        // 使用放大镜（Magnifying Glass）查看子弹
        if (!hasAttacked && dealer.tls[MAGNIFYING_GLASS] > 0 && bullet.nw < bullet.all_bullets) {
            dealer.tls[MAGNIFYING_GLASS]--;
            dealer_tools--;
            printf("庄家正在使用放大镜观察子弹\n");
            Sleep(2000);
            printf("庄家查看到当前子弹为: %s\n", (Bullets[bullet.nw] == 1 ? "实弹" : "空弹"));
            Sleep(2000);
            if (Bullets[bullet.nw] == 1) {
                // 使用小刀（Knife）
                if (dealer.tls[KNIFE] > 0) {
                    dealer.tls[KNIFE]--;
                    bullet.nw_damage = 2; // 伤害加倍
                    printf("庄家使用了小刀，下一次攻击伤害加倍\n");
                    Sleep(2000);
                }

                // 攻击玩家
                printf("庄家选择攻击玩家\n");
                Sleep(2000);
                printf("实弹，玩家血量 -%d\n", bullet.nw_damage);
                player.life -= bullet.nw_damage;
                bullet.tru_bullets--;
                bullet.nw++;
                bullet.nw_damage = 1;
                now_gun = nxt_gun;
                nxt_gun = !nxt_gun;
				SomeOneDead();
                hasAttacked = true;
            } else {
                // 虚弹状态下攻击自己
                printf("庄家对自己开枪\n");
                Sleep(2000);
                printf("空弹，未造成伤害\n");
                bullet.emp_bullets--;
                bullet.nw++;
                bullet.nw_damage = 1;
                continueTurn = true;
                Sleep(2000);
                continue;
            }
        }

        // 随机默认攻击行为
        if (!hasAttacked && bullet.nw < bullet.all_bullets) {
            int random_decision = rand() % 2;
            if (random_decision == 0) {
                printf("庄家选择攻击玩家\n");
                Sleep(2000);
                if (Bullets[bullet.nw] == 1) {
                    printf("实弹，玩家 -%d 血量\n", bullet.nw_damage);
                    player.life -= bullet.nw_damage;
                    bullet.tru_bullets--;
                    bullet.nw++;
                    bullet.nw_damage = 1;
                    now_gun = nxt_gun;
                    nxt_gun = !nxt_gun;
                    Sleep(2000);
                } else if (Bullets[bullet.nw] == 0) {
                    printf("空弹，未造成伤害\n");
                    bullet.emp_bullets--;
                    bullet.nw++;
                    bullet.nw_damage = 1;
                    now_gun = nxt_gun;
                    nxt_gun = !nxt_gun;
                    Sleep(2000);
                }
            } else {
                printf("庄家对自己开枪\n");
                Sleep(2000);
                if (Bullets[bullet.nw] == 1) {
                    dealer.life -= bullet.nw_damage;
                    bullet.tru_bullets--;
                    bullet.nw++;
                    printf("实弹，庄家 -%d 滴血\n", bullet.nw_damage);
                    bullet.nw_damage = 1;
                    now_gun = nxt_gun;
                    nxt_gun = !nxt_gun;
                    Sleep(2000);
					SomeOneDead();
                } else if (Bullets[bullet.nw] == 0) {
                    printf("空弹，未造成伤害\n");
                    bullet.emp_bullets--;
                    bullet.nw++;
                    bullet.nw_damage = 1;
                    continueTurn = true;
                    Sleep(2000);
                }
            }
        }
    }

    Sleep(2000);
    system("cls");
    reloadOutTools(); // 刷新子弹和道具、
    return;
}