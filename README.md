# Call_Fun_In_SPROM
SPROM Code build Guide

User can call sprom 

![image](https://github.com/keikkang/Call_Fun_In_SPROM/assets/108905975/427af01e-f3af-4815-bad4-911211fe8cf3)

```bash
MAIN_APROM  0x0  ;FLASH의 시작주소. 또한 MAIN_APROM은 라벨링으로
{                ;다른 이름으로 변경하여도 상관 X
	APROM.bin  0x0 ;원하는 컴파일 출력물의 이름
	{
		startup_M480.o (RESET, +FIRST) ;FIRST지시어는 startup_M480.s → startup_M40. 파일RESET 라벨을 메모리 가장 앞부분에 배치하게 됨
		*.o (+RO) ;컴파일된 모든 object파일의 +RO 데이터를 APROM.bin에 배치하라는 의미.
	}
	
	SRAM  0x20000000 0x20000 ;실제 M487 SRAM 영역
	{
		* (+RW, +ZI) ;전역 변수 및 정적변수의(RW) 경우 APROM에 초기에는 저장 돼 있다가 SRAM으로 복사 됨
	}              ;ZI의 경우 0으로 초기화 되기 때문에 APROM으로부터 복사가 일어나지 않음
}

PROTECT_SPROM  0x200000 ;SPROM의 시작 주소
{                    ;
	SPROM.bin  0x200000 
	{
		sprom.o (+RO)
	}
}
```

ref..
```C
#inlcude <stdio.h>
			/* Noticed taht Keil(GCC)*/
int a = 20; 		//RW(.data)
int b =0; 		//ZI(.bss)
int const c = 10; 	//RO(.constdata)

void main(void)
{
  static int d;		//ZI(.bss)
  static int e = 10; 	//RW(.data)

  int f; 		//Stack
  char g;		//Stack

  f = (char*)malloc(sizeof(char)*200); //Heap
} 
```


