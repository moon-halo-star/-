#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<windows.h>
#include <math.h>
#include <time.h>
int i=0; 
typedef struct PolyNode {
    double coef;           
    int exp;               
	struct PolyNode* next; 
}PolyNode,*Polylist;

typedef struct allPoly{
	Polylist *polys;   
    int count;         
    int capacity; 
}PolyManager;

// 所有存储的数组 
// 以下是功能函数 
int analysis(char*input,PolyNode terms[]);//分析函数
Polylist createPoly();//创建空多项式 
Polylist createPolyfrominput();//由用户的输入来创建多项式 
void printfpoly(Polylist poly);//输出多项式 
Polylist Ployadd(Polylist poly_a,Polylist poly_b);//多项式加法 
Polylist Ployminus(Polylist poly_a,Polylist poly_b);//多项式减法 
Polylist Polymulti(Polylist poly_a,Polylist poly_b);//多项式乘法 
int compareExp(int a,int b);//指数比较 
void Deletepoly(Polylist poly);//多项式删除 
void display_menu();//菜单显示 
void insertNode(Polylist poly, double coef, int exp);//降序插入

// 以下是菜单的函数
// 初始化多项式管理器
void initPolyManager();
// 扩容多项式数组 
void expandPolyManager();
// 获得多项式名字 
char* getPolyName(int index);
// 多项式显示
void showAllPolys();
// 选择多项式
int selectPoly();
// 添加多项式 
int addNewPoly(Polylist poly);

//                                     轻量级身份认证功能 
// 服务器存储的秘密多项式
Polylist secretPoly = NULL;
// 生成随机挑战点
double generateChallenge();
// 计算多项式在x点的值
double evaluatePoly(Polylist poly, double x);
// 设置服务器的秘密多项式
void setSecretPoly();
// 显示当前秘密多项式
void showSecretPoly();
// 模拟设备认证过程
void deviceAuthentication();
// 批量测试认证成功率
void batchAuthenticationTest();
// 认证子菜单
void authSubMenu();
// 生成随机稀疏多项式
Polylist generateRandomSparsePoly();

PolyManager polyManager = {NULL, 0, 0};

int main()
{
    srand((unsigned int)time(NULL)); // 初始化随机数种子
	initPolyManager();
	display_menu();
	return 0;
}

int addNewPoly(Polylist poly){
    if(polyManager.count>=polyManager.capacity){
        expandPolyManager();
    }
    
    polyManager.polys[polyManager.count]=poly;
    polyManager.count++;
    return polyManager.count-1; // 返回新多项式的索引
}

// 初始化多项式管理器
void initPolyManager(){
    polyManager.capacity=10;
    polyManager.polys=(Polylist*)malloc(polyManager.capacity*sizeof(Polylist));
    polyManager.count=0;
    
    // 初始化五个空多项式
    polyManager.polys[0]=createPoly();
    polyManager.polys[1]=createPoly();
    polyManager.polys[2]=createPoly();
    polyManager.polys[3]=createPoly();
    polyManager.polys[4]=createPoly();
    polyManager.count=5;
}

//多项式的容量扩容 
void expandPolyManager()
{
    int newCapacity=polyManager.capacity*2;
    Polylist*newPolys=(Polylist*)malloc(newCapacity*sizeof(Polylist));
    int i=0;
    for(i=0;i<polyManager.count; i++){
        newPolys[i]=polyManager.polys[i];
    }
    
    free(polyManager.polys);
    polyManager.polys=newPolys;
    polyManager.capacity=newCapacity;
}

//获得多项式名字 
char* getPolyName(int index){
    static char name[20];
    sprintf(name,"多项式%d",index+1);
    return name;
}

//多项式显示
void showAllPolys(){
    printf("\n当前所有多项式:\n");
    printf("====================\n");
    int i; 
    for(i=0;i<polyManager.count;i++)
		{
        	if(polyManager.polys[i]!=NULL&&polyManager.polys[i]->next != NULL)
			{
            	printf("%d.%s:",i+1,getPolyName(i));
            	printfpoly(polyManager.polys[i]);
        	}
			else{
        	    printf("%d. %s:空\n",i+1,getPolyName(i));
        	}
    }
    printf("====================\n");
}

// 选择多项式
int selectPoly(){
    showAllPolys();
    printf("请选择多项式(1-%d): ",polyManager.count);
    int choice;
    scanf("%d",&choice);
    getchar();
    if(choice < 0||choice>=polyManager.count){
        printf("无效选择！\n");
        return -1;
    }
    return choice-1;
} 

//该函数用于对用户输入的多项式进行分析,来得出项数count,系数coef,指数exp的数据 
int analysis(char*input, PolyNode terms[]){
    int count=0;
    char*token=strtok(input," ");
    
   while(token!=NULL)
   {
        float coef=1.0;
        int exp=0;
        char *ptr=token;
        int sign=1;

        if(*ptr=='+')
			ptr++;
        else if(*ptr=='-')
		{
            sign=-1;
            ptr++;
        }

        if(isdigit(*ptr)||*ptr=='.')
		{
            coef=atof(ptr);
        }
        coef*=sign;
        
        while(isdigit(*ptr)||*ptr=='.')
		{
            ptr++;
        }
        ////////////////////////
        if(*ptr=='x')
		{
            ptr++;
            if(*ptr=='^')
			{
                ptr++;
                exp=atoi(ptr);
            } 
			else 
			{
                exp=1;
            }
        } 
		else exp=0; 
		/////////////////////
		terms[count].coef=coef;
        terms[count].exp=exp;
        count++;
        //QAQ---------ciallo!
        token=strtok(NULL," ");
    }
    
    return count;
}

//该函数用于创建链表的头节点 
Polylist createPoly() 
{
    Polylist poly = (Polylist)malloc(sizeof(PolyNode));
    if (!poly) {
        printf("内存分配失败!\n");
        return NULL;
    }
    poly->next = NULL;
	return poly;
}

//由用户的输入来创建多项式
Polylist createPolyfrominput()
{
	Polylist h=createPoly();
	Polylist p;
	printf("请输入多项式(各项用空格分隔，如:6x^2 -3x +1\n");
	char input[100];
	PolyNode terms[100];
	fgets(input,100,stdin);
	int count=analysis(input,terms);
	int i=0;
	for(i=0;i<count;i++)
	{
		insertNode(h,terms[i].coef,terms[i].exp);
	}
	printfpoly(h);
	return h;
}

//将输入的多项式打印出来 
void printfpoly(Polylist poly)
{
	Polylist p= poly->next;
	int i=0;
	
	while(p!=NULL)
	{
		if(p->exp>=0)
		{
			if(i==0)
				printf("%.2fx^%d",p->coef,p->exp);
			else if(p->coef>0)
				printf("+%.2fx^%d",p->coef,p->exp);
			else if(p->coef<0) 
				printf("%.2fx^%d",p->coef,p->exp);
		}
		p=p->next;
		i++;
	}
	
	printf("\n");
}

//指数比较 
int compareExp(int a,int b)
{
	if(a==b) return 0;
	if(a>b) return 1;
	if(a<b) return -1;
}

//多项式加法
Polylist Ployadd(Polylist poly_a,Polylist poly_b)
{
    Polylist result=createPoly();
    Polylist pa,pb,pr;  
    pa=poly_a->next;
    pb=poly_b->next;
    while(pa!=NULL&&pb!=NULL)
    {
        switch(compareExp(pa->exp,pb->exp))
        {
            case 0:
                if(pa->coef+pb->coef!=0){
                	insertNode(result,pa->coef+pb->coef,pa->exp);
                    pa=pa->next;
                    pb=pb->next;
                    break;
                }
                else{
                    pa=pa->next;
                    pb=pb->next;
                    break;
                }
            case 1:
                {
                	insertNode(result,pa->coef,pa->exp);
                    pa=pa->next;
                    break;
                }
            case -1:
                {
                	insertNode(result,pb->coef,pb->exp);
                    pb=pb->next;
                    break;
                }
        }
    }
    
    while(pa != NULL) {
    	insertNode(result,pa->coef,pa->exp);
        pa=pa->next;
    }
    
    while(pb != NULL) {
		insertNode(result,pb->coef,pb->exp);
        pb=pb->next;
    }
    
    return result;
}

//多项式减法
Polylist Ployminus(Polylist poly_a,Polylist poly_b)
{
    Polylist result=createPoly();
    Polylist pa,pb,pr;  
    pa=poly_a->next;
    pb=poly_b->next;
    while(pa!=NULL&&pb!=NULL)
    {
        switch(compareExp(pa->exp,pb->exp))
        {
            case 0:
                if(pa->coef+pb->coef!=0){
                	insertNode(result,pa->coef-pb->coef,pa->exp);
                    pa=pa->next;
                    pb=pb->next;
                    break;
                }
                else{
                    pa=pa->next;
                    pb=pb->next;
                    break;
                }
            case 1:
                {
                	insertNode(result,pa->coef,pa->exp);
                    pa=pa->next;
                    break;
                }
            case -1:
                {
                	insertNode(result,pb->coef,pb->exp);
                    pb=pb->next;
                    break;
                }
        }
    }
    
    while(pa != NULL) {
    	insertNode(result,pa->coef,pa->exp);
        pa=pa->next;
    }
    
    while(pb != NULL) {
		insertNode(result,pb->coef,pb->exp);
        pb=pb->next;
    }
    
    return result;
}

//多项式乘法
Polylist Polymulti(Polylist poly_a,Polylist poly_b)
{
	Polylist pa=poly_a->next;
    Polylist pb;
    Polylist result=createPoly(); 
     while(pa != NULL)
     {
     	pb=poly_b->next;
     	while(pb != NULL)
     	{
     		double coef=pa->coef*pb->coef;
            int exp=pa->exp+pb->exp;
     		insertNode(result,coef,exp);
			pb=pb->next;	
		 }
		pa=pa->next;
	 }
	 return result;
} 

//多项式删除 
void Deletepoly(Polylist poly)
{
	Polylist p=poly;
	while(p)
	{
		Polylist m=p;
		p=p->next;
		free(m);	
	} 
}

//降序插入 
void insertNode(Polylist poly,double coef,int exp) {
    Polylist newNode=(Polylist)malloc(sizeof(PolyNode));
    newNode->coef=coef;
    newNode->exp=exp;
    newNode->next=NULL;
    
    if(poly->next==NULL||exp>poly->next->exp)
	{
        newNode->next=poly->next;
        poly->next=newNode;
        return;
    }
    
    Polylist current=poly->next;
    Polylist pre=poly;
    while(current!=NULL&&current->exp>exp)
	{
        pre=current;
        current=current->next;
    }
    if (current!=NULL&&current->exp==exp){
        current->coef+=coef;
        free(newNode);
        if (current->coef == 0) {
            pre->next = current->next;
            free(current);
        }
    }else{
        newNode->next=current;
        pre->next=newNode;
    }
}

//轻量级身份认证功能实现 

// 生成随机挑战点
double generateChallenge() {
    return (double)(rand() % 100 + 1); // 生成1-100的随机整数作为挑战
}

// 计算多项式在x点的值
double evaluatePoly(Polylist poly, double x) {
    double result = 0.0;
    Polylist p = poly->next;
    
    while (p != NULL) {
        result += p->coef * pow(x, p->exp);//pow函数 
        p = p->next;
    }
    
    return result;
}

// 生成随机稀疏多项式（项数小于8，最大指数小于10）
Polylist generateRandomSparsePoly() {
    Polylist poly = createPoly();
    
    // 随机项数（1到7项）
    int termCount = rand() % 7 + 1;
    int usedExps[10] = {0}; // 记录已使用的指数（0-9）
    
    printf("正在生成随机稀疏多项式...\n");
    printf("项数: %d, 最大指数: 9\n", termCount);
    
    for (i = 0; i < termCount; i++) {
        int exp;
        do {
            exp = rand() % 10; // 指数0-9
        } while (usedExps[exp] && i > 0); // 确保指数不重复
        
        usedExps[exp] = 1;
        
        double coef = (double)(rand() % 10 + 1);
        
        insertNode(poly, coef, exp);
    }
    
    printf("生成的稀疏多项式: ");
    printfpoly(poly);
    return poly;
}

// 设置服务器的秘密多项式
void setSecretPoly() {
    printf("设置服务器的秘密多项式:\n");
    printf("1. 手动输入多项式\n");
    printf("2. 自动生成随机稀疏多项式\n");
    printf("请选择: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();
    
    if (secretPoly != NULL) {
        Deletepoly(secretPoly);
    }
    
    if (choice == 1) {
        secretPoly = createPolyfrominput();
    } else {
        secretPoly = generateRandomSparsePoly();
    }
    printf("秘密多项式设置成功！\n");
}

// 显示当前秘密多项式
void showSecretPoly() {
    if (secretPoly == NULL || secretPoly->next == NULL) {
        printf("当前未设置秘密多项式！\n");
        return;
    }
    printf("当前秘密多项式: ");
    printfpoly(secretPoly);
    
}

// 模拟设备认证过程
void deviceAuthentication() {
    if (secretPoly == NULL || secretPoly->next == NULL) {
        printf("错误：请先设置服务器的秘密多项式！\n");
        return;
    }
    
    printf("=== 身份认证过程 ===\n");
    
    double challenge = generateChallenge();
    printf("1. 服务器生成挑战数 r = %.2f\n", challenge);
    
    double deviceResponse = evaluatePoly(secretPoly, challenge);//服务器随机生成一个挑战数,代入计算后输出结果
    
    printf("\n选择模拟的设备类型:\n");
    printf("1. 合法设备（知道秘密多项式）\n");//合法者的结果即为此结果,不合法者的结果为此结果加上一个随机误差来模拟 
    printf("2. 攻击者（不知道秘密多项式）\n");
    printf("请选择: ");
    
    int deviceChoice;
    scanf("%d", &deviceChoice);
    getchar();
    
    double actualResponse;
    if (deviceChoice == 1) {
        actualResponse = deviceResponse;
        printf("合法设备返回响应: %.6f\n", actualResponse);
    } else {
        // 攻击者猜测（添加随机误差）
        actualResponse = deviceResponse + (double)(rand() % 20 - 10);
        printf("攻击者猜测响应: %.6f\n", actualResponse);
    }
    
    // 服务器验证
    printf("3. 服务器验证响应...\n");
    
    double tolerance = 1e-6;
    if (fabs(actualResponse - deviceResponse) < tolerance) {
        printf("认证成功!\n");
    } else {
        printf("认证失败\n");
    }
}

// 批量测试认证成功率
void batchAuthenticationTest() {
    if (secretPoly == NULL || secretPoly->next == NULL) {
        printf("错误：请先设置服务器的秘密多项式！\n");
        return;
    }
    
    printf("开始批量认证测试...\n");
    printf("输入测试次数: ");
    int testCount;
    scanf("%d", &testCount);
    getchar();
    
    int successCount = 0;
    
    printf("\n进行 %d 次认证测试...\n", testCount);
    for (i = 0; i < testCount; i++) {
        double challenge = generateChallenge();
        double correctResponse = evaluatePoly(secretPoly, challenge);
        
        // 模拟攻击者随机猜测
        double wrongResponse = correctResponse + (double)(rand() % 40 - 20);
        double deviceResponse = wrongResponse;
        
        double tolerance = 1e-6;
        if (fabs(deviceResponse - correctResponse) < tolerance) {
            successCount++;
        }
    }
    
    double successRate = (double)successCount / testCount * 100;
    printf("\n批量测试结果:\n");
    printf("测试次数: %d\n", testCount);
    printf("攻击者成功次数: %d\n", successCount);
    printf("攻击者成功率: %.6f%%\n", successRate);
    
    if (successRate < 0.001) {
        printf("认证系统安全性优秀！\n");
    } else if (successRate < 0.01) {
        printf("认证系统安全性良好！\n");
    } else {
        printf("认证系统存在安全风险！\n");
    }
}

// 认证子菜单
void authSubMenu() {
    int authChoice;
    do {
        system("cls");
        printf("***********轻量级身份认证系统 **********\n");
        printf("1. 设置服务器秘密多项式\n");
        printf("2. 显示当前秘密多项式\n");
        printf("3. 单次认证测试\n");
        printf("4. 批量认证测试\n");
        printf("5. 更换秘密多项式\n");
        printf("6. 返回主菜单\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &authChoice);
        getchar();
        
        switch(authChoice) {
            case 1:
                setSecretPoly();
                break;
            case 2:
                showSecretPoly();
                break;
            case 3:
                deviceAuthentication();
                break;
            case 4:
                batchAuthenticationTest();
                break;
            case 5:
                if (secretPoly != NULL) {
                    Deletepoly(secretPoly);
                }
                secretPoly = generateRandomSparsePoly();
                printf("? 秘密多项式已更换！\n");
                break;
            case 6:
                printf("返回主菜单...\n");
                break;
            default:
                printf("无效选择！\n");
        }
        
        if (authChoice != 6) {
            printf("按任意键继续...");
            getchar();
        }
    } while (authChoice != 6);
}

void display_menu() {
    int choice,i;
    
    while(1) {
        system("cls");  
        printf("---------- 动态多项式计算器 ----------\n");
        printf("1. 输入/编辑多项式\n");
        printf("2. 多项式加法\n");
        printf("3. 多项式减法\n");
        printf("4. 多项式乘法\n");
        printf("5. 查看所有多项式\n");
        printf("6. 删除多项式\n");
        printf("7. 轻量级身份认证\n");
        printf("0. 退出\n");
        printf("请选择: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: {
                int index = selectPoly();
                if(index >= 0) {
                    if(polyManager.polys[index] != NULL) {
                        Deletepoly(polyManager.polys[index]);
                    }
                    polyManager.polys[index] = createPolyfrominput();
                }
                break;
            }
                
            case 2: {
                printf("选择两个多项式进行加法运算:\n");
                int index1 = selectPoly();
                if(index1 < 0) break;
                int index2 = selectPoly();
                if(index2 < 0) break;
                
                Polylist result = Ployadd(polyManager.polys[index1], polyManager.polys[index2]);
                int resultIndex = addNewPoly(result);
                printf("加法结果已保存为 %s\n", getPolyName(resultIndex));
                printf("结果: ");
                printfpoly(result);
                break;
            }
                
            case 3: {
                printf("选择两个多项式进行减法运算:\n");
                int index1 = selectPoly();
                if(index1 < 0) break;
                int index2 = selectPoly();
                if(index2 < 0) break;
                
                Polylist result = Ployminus(polyManager.polys[index1], polyManager.polys[index2]);
                int resultIndex = addNewPoly(result);
                printf("减法结果已保存为 %s\n", getPolyName(resultIndex));
                printf("结果: ");
                printfpoly(result);
                break;
            }
                
            case 4: {
                printf("选择两个多项式进行乘法运算:\n");
                int index1 = selectPoly();
                if(index1 < 0) break;
                int index2 = selectPoly();
                if(index2 < 0) break;
                
                Polylist result = Polymulti(polyManager.polys[index1], polyManager.polys[index2]);
                int resultIndex = addNewPoly(result);
                printf("乘法结果已保存为 %s\n", getPolyName(resultIndex));
                printf("结果: ");
                printfpoly(result);
                break;
            }
                
            case 5:
                showAllPolys();
                break;
                
            case 6: {
                int index = selectPoly();
                if(index >= 0 && index < polyManager.count) {
                    if(polyManager.polys[index] != NULL) {
                        Deletepoly(polyManager.polys[index]);
                        polyManager.polys[index] = createPoly();
                        printf("%s 已被清空\n", getPolyName(index));
                    }
                }
                break;
            }
            
            case 7:
                authSubMenu();
                break;
                
            case 0: 
                // 清理秘密多项式
                if (secretPoly != NULL) {
                    Deletepoly(secretPoly);
                }
                for(i = 0; i < polyManager.count; i++) {
                    if(polyManager.polys[i] != NULL) {
                        Deletepoly(polyManager.polys[i]);
                    }
                }
                free(polyManager.polys);
                printf("再见！\n");
                return;
                
            default:
                printf("无效选择，请重新输入！\n");
        }
        
        printf("按任意键继续...");
        getchar(); 
        getchar();
    }
}
