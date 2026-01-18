#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
//控制台输入输出库
#include<windows.h>//系统

//棋盘map[15][15]
// 落棋x y，0空，1黑棋，2白棋

int n=1;//输入错误计数器
int t=1;//控制游戏解释后状态

//禁手判断：黑棋不能三三，四四，长连
//return 0=正常, 1=长连, 2=三三, 3=四四
int IsForbidden(int map[15][15], int x, int y) {
    if (map[y][x] != 1) return 0;
    //只有黑棋才判断禁手

    int k, i;
    int three_count = 0;//活三
    int four_count = 0;//四连
    int five_found = 0;//是否成5
    
    //方向数组
    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};

    //遍历四个方向
    for(k = 0; k < 4; k++) {
        int count = 1;//连子数
        int left_open = 0;//反方向是否为空
        int right_open = 0;//正方向是否为空
        
        //正方向
        for(i = 1; i <= 5; i++) {
            int nx = x + i * dx[k];
            int ny = y + i * dy[k];

            //判断是否出界
            if(nx < 0 || nx > 14 || ny < 0 || ny > 14) break;
            
            if(map[ny][nx] == 1) count++;
            //黑棋 ＋1
            else {
                if(map[ny][nx] == 0) right_open = 1;
                //空位标记
                break;
            }
        }
        //同上查找反方向
        for(i = 1; i <= 5; i++) {
            int nx = x - i * dx[k];
            int ny = y - i * dy[k];
            if(nx < 0 || nx > 14 || ny < 0 || ny > 14) break;
            
            if(map[ny][nx] == 1) count++;
            else {
                if(map[ny][nx] == 0) left_open = 1;
                break;
            }
        }

        if(count == 5) five_found = 1;//成五
        if(count > 5) return 1;//长连
        if(count == 4) four_count++;//四连
        if(count == 3 && left_open && right_open) three_count++;
    }

    if(five_found) return 0;
    //只要五连即为获胜，哪怕禁手
    if(four_count >= 2) return 3;//四四
    if(three_count >= 2) return 2;//三三

    return 0;//无禁手
}

//判断在（x,y）落子后是否获胜
int Win_or_not(int map[15][15],int x,int y){
    int i,j;
    int temp=map[y][x];
    //temp用来暂存当前落子的颜色
    //return 1=黑棋获胜, 2=白棋获胜，0=无获胜
    if(temp == 0) return 0;
    //水平
    for(j=x-4,i=y; j<=x; j++){ 
        if(j>=0 && j<=10 && temp==map[i][j] && temp==map[i][j+1] && temp==map[i][j+2] && temp==map[i][j+3] && temp==map[i][j+4]) return temp;
    }
    //垂直
    for(i=y-4,j=x; i<=y; i++){ 
        if(i>=0 && i<=10 && temp==map[i][j] && temp==map[i+1][j] && temp==map[i+2][j] && temp==map[i+3][j] && temp==map[i+4][j]) return temp;
    }
    //左下到右上
    for(j=x-4,i=y+4; j<=x && i>=1; j++,i--){ 
        if(j>=0 && j<=10 && i>=4 && i<=14 && temp==map[i][j] && temp==map[i-1][j+1] && temp==map[i-2][j+2] && temp==map[i-3][j+3] && temp==map[i-4][j+4]) return temp;
    }
    //左上到右下
    for(j=x-4,i=y-4; j<=x && i<=y; j++,i++){ 
        if(j>=0 && j<=10 && i>=0 && i<=10 && temp==map[i][j] && temp==map[i+1][j+1] && temp==map[i+2][j+2] && temp==map[i+3][j+3] && temp==map[i+4][j+4]) return temp;
    }
    return 0;//无
}

//棋盘显示，禁手显示
void ShowMap(int map[15][15],int* isblack,int win_or_not,int num){
    int i,j;
    int arr[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    //打印横坐标
    printf("   ");
    for(i=0;i<15;i++){
        if(i<9) printf("%d ",arr[i]);
        else    printf("%d",arr[i]);
    }
    printf("\n");
    
    for(i=0;i<15;i++){
        if(i<9) printf("%d  ",arr[i]);
        else    printf("%d ",arr[i]);
        //纵坐标

        for(j=0;j<15;j++)
        {            
            if(map[i][j]==0){
                int is_bad_move = 0;
                //禁手显示
                if(*isblack == 1 && win_or_not == 0) { 
                    map[i][j] = 1;
                    //虚拟落子
                    if(IsForbidden(map, j, i)) is_bad_move = 1;
                    //检查是否禁手
                    map[i][j] = 0;
                    //还原
                }

                if(is_bad_move) {
                    printf("× ");//显示禁手
                } else {
                    if(i==0&&j==0){printf("┌ ");}
                    else if(i==0&&j==14){printf("┐");}
                    else if(i==14&&j==0){printf("└ ");}
                    else if(i==14&&j==14){printf("┘");}
                    else if(i==0){printf("┬ ");}
                    else if(i==14){printf("┴ ");}
                    else if(j==0){printf("├ ");}
                    else if(j==14){printf("┤");}
                    else {printf("┼ ");}
                }
                //打印棋盘
            }
            else if(map[i][j]==1){printf("● ");//黑棋
            } else{printf("○ ");//白棋
            }
        }
        printf("\n");
    }

    if(t==1||win_or_not==1||win_or_not==2||num==225){return;}
    else if(*isblack==1){printf("黑棋(×为禁手)请输入:");}
    else {printf("白棋(无禁手)请输入:");}
    //底部提示
}

void Press_Error_Y_N_O(char* press){
     if(n==1){printf("\n请重新输入:");n++;}
    *press=getch();
    if(*press=='y'||*press=='Y'||*press=='n'||*press=='N'||*press=='o'||*press=='O'){n=1;return;}
    else{Press_Error_Y_N_O(press);}
}

void Press_Error_Y_N(char* press){
    if(n==1){printf("\n请重新输入:");n++;}
    *press=getch();
    if(*press=='y'||*press=='Y'||*press=='n'||*press=='N'){n=1;return;}
    else{Press_Error_Y_N(press);}
}

//落子
//输入坐标 是否合法 是否禁手 切换回合
void SetPoint(int map[15][15],int* isblack,int* win_or_not){
    int f,s;
    
    //验证输入的格式
    if(scanf("%d %d",&f,&s) != 2){
        while(getchar()!='\n');
        printf("格式错误，请重输: ");
        SetPoint(map,isblack,win_or_not);
        return;
    }
    
    //转化为数组内数字
    int x=f-1;
    int y=s-1;
    
    //验证坐标是否越界，是否已落子
    if(x<0||y<0||x>14||y>14){
        printf("越界，请重输: ");
        SetPoint(map,isblack,win_or_not);
        return;
    }
    else if(map[y][x]!=0){
        printf("已有子，请重输: ");
        SetPoint(map,isblack,win_or_not);
        return;
    }

    //黑棋
    if(*isblack==1){
        map[y][x] = 1;
        //检查禁手
        int forbiddenType = IsForbidden(map, x, y);
        if(forbiddenType != 0) {
            //提示禁手
            if(forbiddenType == 1) printf("【长连禁手】不可落子: ");
            else if(forbiddenType == 2) printf("【三三禁手】不可落子: ");
            else if(forbiddenType == 3) printf("【四四禁手】不可落子: ");
            
            map[y][x] = 0;//还原
            SetPoint(map, isblack, win_or_not);
            return;
        }
        *isblack = 2;//切换回合
    }
    //白棋
    else{
        map[y][x] = 2;
        *isblack = 1;
    }
    //检查是否胜利
    *win_or_not=Win_or_not(map,x,y);
}

//游戏主循环
void PlayGame(){
    int map[15][15]={0};//初始化
    int a=1;
    int* isblack=&a;
    int num=0;
    
    ShowMap(map,isblack,0,num);
    if(t==1){
        t++;
        printf("黑棋先手，请输入坐标:");
    };
    
    while(1)
    {
        int b=0;//获胜判断标识0无获胜1黑棋获胜2白棋获胜
        int* win_or_not=&b;
        
        SetPoint(map,isblack,win_or_not);
        system("cls");//清屏
        num++;
        //重新显示棋盘
        ShowMap(map,isblack,*win_or_not,num);
        
        if(*win_or_not==1){
            printf("黑棋WIN!\n"); 
            break;
        }
        if(*win_or_not==2){
            printf("白棋WIN!\n");
            break;
        }
        if(num==225){
            printf("平局");
            break;
        }    
    }
    printf("再来一局?(y/n) 或输入o查看说明: ");
    t=1;
}

void Introduction(){
    printf("======= 五子棋 =======\n");
    printf("规则：黑棋有三三、四四、长连禁手。\n");
    printf("操作：输入横纵坐标 (如: 8 8) 回车。\n");
    printf("----------------------\n");
    printf("开始游戏？(y/n):");
    
    char* press;
    char temp=getch();
    press=&temp;
    
    if(*press=='n'||*press=='N'){exit(0);}
    else if(*press=='y'||*press=='Y'){
        system("cls");
        PlayGame();
    }
    else{
        Press_Error_Y_N(press);
        if(*press=='n'||*press=='N'){exit(0);}
        if(*press=='y'||*press=='Y'){
            system("cls");
            PlayGame();
        }
    }
}   
//重新开始游戏
void RePlayGame(){
    while(1)
    {       
        char* press;
        char temp=getch();
        press=&temp;
        if(*press=='n'||*press=='N'){exit(0);}
        else if(*press=='o'||*press=='O'){
            system("cls");
            printf("【说明】图中 '×' 为黑棋禁手点。\n再来一局？(y/n):");
        }
        else if(*press=='y'||*press=='Y'){
            system("cls");
            PlayGame();
        }
        else{
            Press_Error_Y_N_O(press);
            if(*press=='n'||*press=='N'){exit(0);}
            else if(*press=='o'||*press=='O'){
                system("cls");
                printf("说明...\n再来一局？(y/n):");
            }
            else if(*press=='y'||*press=='Y'){
                system("cls");
                PlayGame();
            }
        }
    }
}

int main()
{
    system("color 70");//设置控制台颜色为白字黑底
    Introduction();//游戏介绍
    RePlayGame();//重新开始游戏
    return 0;//程序正常结束
}