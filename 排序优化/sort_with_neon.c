#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<arm_neon.h> 

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
/*冒泡排序neon优化*/ 
void Bubble_sort_neon(float arr[],int n){
	int i,j,k;
	float a=0;
	for(i=0;i<n-1;i++){
		for(j=0;j<n-i-5;j+=4){
			float32x4_t vector1=vld1q_f32(&arr[j]);
			float32x4_t vector2=vld1q_f32(&arr[j+1]);
			uint32x4_t vectorcompare=vcgtq_f32(vector1,vector2);
			if(vgetq_lane_u32(vectorcompare,0)||vgetq_lane_u32(vectorcompare,1)||vgetq_lane_u32(vectorcompare,2)||vgetq_lane_u32(vectorcompare,3)){
				for (k=0;k<4;k++){
                    if(arr[j+k]>arr[j+k+1]){
                        a=arr[j+k];
                        arr[j+k]=arr[j+k+1];
                        arr[j+k+1]=a;
						}/*这里就能看出单纯用neon优化冒泡其实并不好,这仍然是传统思路*/
                    }
			}
	
			for(;j<n-i-1;j++){
				if(arr[j]>arr[j+1]){
                a=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=a;
				}
			} 
		} 
	}
}
/*第1,2,3,4个与第2,3,4,5个对齐比较,把大的交换到后面*/

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
		if(leftarr[i]<=rightarr[j]){
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

/*归并排序带neon*/ 
void merge_neon(float arr[],int left,int mid,int right){
	int nl=mid-left+1;
	int nr=right-mid;
	int i=0,j=0,k=left;
	float leftarr[nl];
	float rightarr[nr];
	for(i=0;i<nl;i++) leftarr[i]=arr[left+i];
	for(j=0;j<nr;j++) rightarr[j]=arr[mid+1+j];
	i=0;
	j=0;
	if (nl >= 4 && nr >= 4 && leftarr[nl-1] <= rightarr[0]) {
        int idx = 0;
        for (; idx <= nl - 4; idx += 4) {
            float32x4_t chunk = vld1q_f32(&leftarr[idx]);
            vst1q_f32(&arr[k], chunk);
            k += 4;
        }
        for (; idx < nl; idx++) {
            arr[k++] = leftarr[idx];
        }
        
        idx = 0;
        for (; idx <= nr - 4; idx += 4) {
            float32x4_t chunk = vld1q_f32(&rightarr[idx]);
            vst1q_f32(&arr[k], chunk);
            k += 4;
        }

        for (; idx < nr; idx++) {
            arr[k++] = rightarr[idx];
        }
        
        return;
        
    } 
	else if (nl >= 4 && nr >= 4 && rightarr[nr-1] <= leftarr[0]) {
        int idx = 0;
        for (; idx <= nr - 4; idx += 4) {
            float32x4_t chunk = vld1q_f32(&rightarr[idx]);
            vst1q_f32(&arr[k], chunk);
            k += 4;
        }
        for (; idx < nr; idx++) {
            arr[k++] = rightarr[idx];
        }
        idx = 0;
        for (; idx <= nl - 4; idx += 4) {
            float32x4_t chunk = vld1q_f32(&leftarr[idx]);
            vst1q_f32(&arr[k], chunk);
            k += 4;
        }
        for (; idx < nl; idx++) {
            arr[k++] = leftarr[idx];
        }
        return; 
    }
    while(i<nl&&j<nr){
		if(leftarr[i]<=rightarr[j]){
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
/*算法核心在于一次加载四个数据,看是否需要排序,不排序的时候直接四个四个移动*/

void Merge_sort_neon(float arr[],int left,int right){
	if(left>=right) return; 
	int mid=(right+left)/2;
	Merge_sort_neon(arr,left,mid);
	Merge_sort_neon(arr,mid+1,right);
	merge_neon(arr,left,mid,right);
}

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

/*选择排序带neon*/
void  Select_sort_neon(float arr[],int n){
	int i,j,k;
	for(i=0;i<n;i++){
		float min=arr[i];
		int minlocation=i;
		for(j=i+1;j<=n-4;j+=4){
			float32x4_t vector=vld1q_f32(&arr[j]);
			float v_min=vminvq_f32(vector);
			if(v_min<min){
				min=v_min;
				for(k=0;k<4;k++){
					if(arr[j+k]==min){
						minlocation=j+k;
						break;
					}
				}
			} 
		}
		
		for(;j<n;j++){
			if(arr[j]<min){
				min=arr[j];
				minlocation=j;
			}
		}
		
		float a=arr[i];
		arr[i]=arr[minlocation];
		arr[minlocation]=a; 
	} 
}
/*该neon优化的思路为将原本的一一处理转化为4个4个处理*/

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
	printf("time of bubble:%.2f\n",end1-start1);
	float *arr_2=copy(arr,N); /*对数组初始化*/ 
	double start2=now();
	Merge_sort(arr_2,0,N-1);
	double end2=now();
	printf("time of merge:%.2f\n",end2-start2);
	float *arr_3=copy(arr,N);
	double start3=now();
	Select_sort(arr_3,N);
	double end3=now();
	printf("time of select:%.2f\n",end3-start3); 
	float *arr_4=copy(arr,N);
	double start4=now();
	Select_sort_neon(arr_4,N);
	double end4=now();
	printf("time of select with neon:%.2f\n",end4-start4); 
	float *arr_5=copy(arr,N);
	double start5=now();
	Bubble_sort_neon(arr_5,N);
	double end5=now();
	printf("time of bubble with neon:%.2f\n",end5-start5);
	float *arr_6=copy(arr,N);
	double start6=now();
	Merge_sort_neon(arr_6,0,N);
	double end6=now();
	printf("time of merge with neon:%.2f\n",end6-start6);
	return 0;
}

