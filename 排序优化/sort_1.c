#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define N 10000


/*计时*/
double now(){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC,&t);
	return t.tv_sec*1000.0+t.tv_nsec/1e6;
} 

/*冒泡排序*/ 
void Bubble_sort(float arr[],int n){
	int i,j;
	float a=0;
	for(i=0;i<n-1;i++){
		for(j=0;j<n-i-1;j++){
			if(arr[j]<arr[j+1]){
				a=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=a;	
			}
		} 
	}	
}

/*归并排序*/ 
void merge(float arr[],int left,int mid,int right){
	int nl=mid-left+1;
	int nr=right-mid;
	int i=0,j=0,k=left;
	float leftarr[nl];
	float rightarr[nr];
	for(i=0;i<nl;i++) leftarr[i]=arr[left+i];
	for(j=0;j<nr;j++) rightarr[j]=arr[mid+1+j];
	i=0;
	j=0;
	while(i<nl&&j<nr){
		if(leftarr[i]>=rightarr[j]){
			arr[k]=leftarr[i];
			i++;
		}
		else{
			arr[k]=rightarr[j];
			j++;
		}
		k++;
	}
	while(i<nl){
		arr[k]=leftarr[i];
		i++;
		k++;
	}
	while(j<nr){
		arr[k]=rightarr[j];
		j++;
		k++;	
	}
} 

void Merge_sort(float arr[],int left,int right){
	if(left>=right) return; 
	int mid=(right+left)/2;
	Merge_sort(arr,left,mid);
	Merge_sort(arr,mid+1,right);
	merge(arr,left,mid,right);
}
/*我第一次学归并,用deepseek写了个示意图放在这
mergeSort(arr, 0, 7)
├── mergeSort(arr, 0, 3)    // 左半部分 [5,2,4,7]
│   ├── mergeSort(arr, 0, 1)  // [5,2]
│   │   ├── mergeSort(arr, 0, 0) → 返回 (单元素)
│   │   ├── mergeSort(arr, 1, 1) → 返回 (单元素)
│   │   └── merge([5],[2]) → [2,5]
│   ├── mergeSort(arr, 2, 3)  // [4,7]
│   │   ├── mergeSort(arr, 2, 2) → 返回
│   │   ├── mergeSort(arr, 3, 3) → 返回
│   │   └── merge([4],[7]) → [4,7]
│   └── merge([2,5], [4,7]) → [2,4,5,7]
├── mergeSort(arr, 4, 7)    // 右半部分 [1,3,2,6]
│   ├── mergeSort(arr, 4, 5)  // [1,3]
│   │   ├── mergeSort(arr, 4, 4) → 返回
│   │   ├── mergeSort(arr, 5, 5) → 返回
│   │   └── merge([1],[3]) → [1,3]
│   ├── mergeSort(arr, 6, 7)  // [2,6]
│   │   ├── mergeSort(arr, 6, 6) → 返回
│   │   ├── mergeSort(arr, 7, 7) → 返回
│   │   └── merge([2],[6]) → [2,6]
│   └── merge([1,3], [2,6]) → [1,2,3,6]
└── merge([2,4,5,7], [1,2,3,6]) → [1,2,2,3,4,5,6,7]
这样一眼就可以看出来具体过程*/

/*选择排序*/
void Select_sort(float arr[],int n){
	int i,j,k;
	for(i=n-1;i>=0;i--) {
		k=0;
		for(j=0;j<=i;j++){
			if(arr[j]>arr[k]) k=j;
		}
		float a=arr[i];
		arr[i]=arr[k];
		arr[k]=a;
	}
}

/*初始化数组*/
float *getnum(){
	float *arr=(float*)malloc(N*sizeof(float));
	int i;
	for(i=0;i<N;i++)
		arr[i]=rand();
	return arr;
}
/*拷贝数据*/ 
float *copy(float arr[],int n){
	float*arr1=(float*)malloc(N*sizeof(float));
	int i;
	for(i=0;i<n;i++) 
		arr1[i]=arr[i];
	return arr1;
} 

int main(){
	srand(time(NULL)); 
	float*arr=getnum();
	float *arr_1=copy(arr,N);
	double start1=now();
	Bubble_sort(arr_1,N);
	double end1=now();
	printf("time of bubble:%.2f",end1-start1);
	float *arr_2=copy(arr,N); /*对数组初始化*/ 
	double start2=now();
	Merge_sort(arr_2,0,N-1);
	double end2=now();
	printf("time of merge:%.2f",end2-start2);
	float *arr_3=copy(arr,N);
	double start3=now();
	Select_sort(arr_3,N);
	double end3=now();
	printf("time of select:%.2f",end3-start3); 
	return 0;
}

