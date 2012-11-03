/* Program for simulation of Intel 8085 microprocessor */
/* Date: 30 Dec 2003 */
/* Author: Srinivas Nayak, Anil Kumar Mohapatra, Sujit Mohanty */
/* Compile using Turbo C++ */

/* This code is distributed under the GPL License. */
/* For more info check: */
/* http://www.gnu.org/copyleft/gpl.html */

//in this program
//(1) comments have been written for all general functions
//       and for 8085 instruction functions
//(2) all general functions are separated from 8085 instruction functions
//(3) all 8085 instruction functions have been arranged alphabatically
//(4) all general functions also have been arranged in order of their
//       appearance(NOT strictly)
//(5) all conditions inside exec() have been arranged in alphabatically order
//(6) display of key board has been changed
//(7) exreg() function and its assesories has been written with comments
//(8)flag changes for all 8085 functions have been written
#include<stdio.h>
#include<conio.h>
#include<graphics.h>
#include<stdlib.h>
#include<string.h>
#include<dos.h>
   union REGS i,o;
   int x1,y1,x2,y2;//for restricting mouse pointer from (x1,y1) to (x2,y2)
   char hex16[4],hex8[2],stack[32][2];
	//to store 16 & 8 bit hex numbers for temporary use
	// stack[][] acts as stack of 8085
   int d[8];//to store 8 bit data bitwise for temporary use
   int exmemno=0,exregno=0,gono=0,rsetno=0,endno=0;
	//calls specific functions when becomes 1
   int button,x,y;//for x,y position & button status of mouse
   int A=0,B=0,C=0,D=0,E=0,H=0,L=0,PC=0,SP=32,BC=0,HL=0,DE=0;
	//registers to contain integer values they have
   char Ah,Al,Fh,Fl,Bh,Bl,Ch,Cl,Dh,Dl,Eh,El,Hh,Hl,Lh,Ll,IRh,IRl;
	//two parts of 8 bit registers to store highorder & loworder hexvalues
   char PCHh,PCLh,SPHh,SPLh,PCHl,PCLl,SPHl,SPLl;
	//two parts of 16 bit registers to store highorder & loworder hexvalues
   int SFLAG=0,ZFLAG=0,ACFLAG=0,PFLAG=0,CYFLAG=0;//flags of 8085
   char mem[512][2];// memory (RAM) size 512 bytes
   char pos1='0',pos2='0',pos3='0',pos4='0',pos5='0',pos6='0';
	//to contain positional characters to be displayed at data area
	//& address area of microcomputer display
   char *pos1ptr=&pos1,*pos2ptr=&pos2,*pos3ptr=&pos3;
   char *pos4ptr=&pos4,*pos5ptr=&pos5,*pos6ptr=&pos6;
   int pos1val=0,pos2val=0,pos3val=0,pos4val=0,pos5val=0,pos6val=0;
	// to store integer values of positional characters
   int addr=0,data,firstnext=1;
	// addr to store integer address values for temporary use
	// after first call to memnext() it becomes 0
   int halt=0;// assembly language program halts when it becomes 1
   int regnum=1;//stores number of the register of 8085
   //***********************
   void restrictmouseptr(int,int,int,int),showmouseptr();
   void hidemouseptr(),getmousepos(int*,int*,int*);
   void check(),display();
   void exmem(),exreg(),go(),rset(),end();
   void shiftladdr(),shiftldata(), displayaddr(),memnext();
   void memprev(),displaydata(),regnext();
   void regprev(),firstmemnext();
   void displayreg(),getregval();
   void addrhex(),hexaddr(),writedata(),getmemval();
   void exec();
   char inttochar(int);
//******************************************************************
//this function displays the graphical interface to simulate the behaviour
//of single board 8085 microcomputer,then activates mouse and wait for
//user response
void main()
	{     int gd=DETECT,gm,i,j;
	      initgraph(&gd,&gm,"");
	      display();
	      initmouse();
	      showmouseptr();
			   // initialization memory
			      for(i=0;i<=511;i++)
			      for(j=0;j<=1;j++)
			      {mem[i][j]='0';}
	      //resets everything
	      rset();
	     //waits for user response
	     while(endno!=1)
	      {
	      if(exmemno==1) {exmem();}
	      if(exregno==1) {exreg();}
	      if(gono==1)    {go();   }
	      if(rsetno==1)  {rset(); }
	      if( exmemno==0&&exregno==0&&gono==0&&rsetno==0&&endno==0){check();}
	      }
	      closegraph();
	}
//*********************************************************************
int initmouse()
{
	i.x.ax=0;
	int86(0x33,&i,&o);
	return(o.x.ax);
}
//********************************************************************
void showmouseptr()
{
	i.x.ax=1;
	int86(0x33,&i,&o);
}
//******************************************************************
void restrictmouseptr(int x1,int y1,int x2,int y2)
{
	i.x.ax=7;
	i.x.cx=x1;
	i.x.dx=x2;
	int86(0x33,&i,&o);
	i.x.ax=8;
	i.x.cx=y1;
	i.x.dx=y2;
	int86(0x33,&i,&o);
}
//*****************************************************************
void getmousepos(int *button,int *x,int*y)
{
	i.x.ax=3;
	int86(0x33,&i,&o);
	*button=o.x.bx;
	*x=o.x.cx;
	*y=o.x.dx;
}
//****************************************************************
void hidemouseptr()
{
	i.x.ax=2;
	int86(0x33,&i,&o);
}
//*****************************************************************
//this function displays the 8085 microcomputer interface
void display()
	{     int i,j;
	      setbkcolor(1);
	      setcolor(10);
	      rectangle(100,100,400,400) ;
	      //vertical lines
	      for(i=100;i<=400;i+=50)
	      line(i,200,i,400) ;
	      //horizontal lines
	      for(j=200;j<=400;j+=50)
	      line(100,j,400,j) ;
	      //address space
		  //two horizontal lines
	      line(110,125,270,125);
	      line(110,175,270,175);
		  // 5 vertical lines
	      line(110,125,110,175);
		 gotoxy(17,10);printf("%c",1); //pos 1
	      line(150,125,150,175);
		 gotoxy(22,10);printf("U"); //pos 2
	      line(190,125,190,175);
		 gotoxy(27,10);printf("S"); //pos 3
	      line(230,125,230,175);
		 gotoxy(32,10);printf("E"); //pos 4
	      line(270,125,270,175);
	      //data space
		  //two horizontal lines
	      line(310,125,390,125);line(310,175,390,175);
		  // 3 vertical lines
	      line(310,125,310,175);
		 gotoxy(42,10);printf("M");  //pos 5
	      line(350,125,350,175);
		 gotoxy(47,10);printf("E");  //pos 6
	      line(390,125,390,175);
	      //    1 st row keys
	      gotoxy(16,15);printf("C");
	      gotoxy(23,15);printf("D");
	      gotoxy(28,15);printf("E");
	      gotoxy(35,15);printf("F");
	      gotoxy(39,15);printf("reset");
	      gotoxy(46,15);printf("end");
	      //    2 nd row keys
	      gotoxy(16,18);printf("8");
	      gotoxy(23,18);printf("9");
	      gotoxy(28,18);printf("A");
	      gotoxy(35,18);printf("B");
	      gotoxy(39,18);printf("next");
	      gotoxy(46,18);printf("prev");
	      //    3 rd row keys
	      gotoxy(16,21);printf("4");
	      gotoxy(23,21);printf("5");
	      gotoxy(28,21);printf("6");
	      gotoxy(35,21);printf("7");
	      gotoxy(39,20);printf("exam");gotoxy(39,21);printf("mem");
	      gotoxy(46,20);printf("exam");gotoxy(46,21);printf("reg");
	      //    4 th row keys
	      gotoxy(16,24);printf("0");
	      gotoxy(23,24);printf("1");
	      gotoxy(28,24);printf("2");
	      gotoxy(35,24);printf("3");
	      gotoxy(39,24);printf("go");
	      gotoxy(46,24);printf("exec");
	}
//********************************************************************
//this function checks the mouse position where user clicks
//and calls coresponding functions for appropriate action
void check()
	{   while(1)
	    {
	    getmousepos(&button,&x,&y);
	    if(button==1&&x>300&&x<350&&y>200&&y<250){rsetno=1;break;}
	    if(button==1&&x>350&&x<400&&y>200&&y<250){endno=1;break;}
	    if(button==1&&x>300&&x<350&&y>300&&y<350){exmemno=1;break;}
	    if(button==1&&x>350&&x<400&&y>300&&y<350){exregno=1;break;}
	    if(button==1&&x>300&&x<350&&y>350&&y<400){gono=1;break;}
	    }
	}
//********************************************************************
//this function is called when user presses 'exam mem' key,
//then it waits for address to be given by the user & when 'next'
//key is pressed shows the data present at that location.
//it also takes data to be inserted at any memory location
void exmem()
	{     while(1)
	      {
		displayaddr();
		if(firstnext==1)
		{
		getmousepos(&button,&x,&y);
		if(button==1&&x>100&&x<150&&y>200&&y<250){shiftladdr();pos4='C';pos4val=12;delay(500);}
		if(button==1&&x>150&&x<200&&y>200&&y<250){shiftladdr();pos4='D';pos4val=13;delay(500);}
		if(button==1&&x>200&&x<250&&y>200&&y<250){shiftladdr();pos4='E';pos4val=14;delay(500);}
		if(button==1&&x>250&&x<300&&y>200&&y<250){shiftladdr();pos4='F';pos4val=15;delay(500);}
		if(button==1&&x>100&&x<150&&y>250&&y<300){shiftladdr();pos4='8';pos4val=8;delay(500);}
		if(button==1&&x>150&&x<200&&y>250&&y<300){shiftladdr();pos4='9';pos4val=9;delay(500);}
		if(button==1&&x>200&&x<250&&y>250&&y<300){shiftladdr();pos4='A';pos4val=10;delay(500);}
		if(button==1&&x>250&&x<300&&y>250&&y<300){shiftladdr();pos4='B';pos4val=11;delay(500);}
		if(button==1&&x>100&&x<150&&y>300&&y<350){shiftladdr();pos4='4';pos4val=4;delay(500);}
		if(button==1&&x>150&&x<200&&y>300&&y<350){shiftladdr();pos4='5';pos4val=5;delay(500);}
		if(button==1&&x>200&&x<250&&y>300&&y<350){shiftladdr();pos4='6';pos4val=6;delay(500);}
		if(button==1&&x>250&&x<300&&y>300&&y<350){shiftladdr();pos4='7';pos4val=7;delay(500);}
		if(button==1&&x>100&&x<150&&y>350&&y<400){shiftladdr();pos4='0';pos4val=0;delay(500);}
		if(button==1&&x>150&&x<200&&y>350&&y<400){shiftladdr();pos4='1';pos4val=1;delay(500);}
		if(button==1&&x>200&&x<250&&y>350&&y<400){shiftladdr();pos4='2';pos4val=2;delay(500);}
		if(button==1&&x>250&&x<300&&y>350&&y<400){shiftladdr();pos4='3';pos4val=3;delay(500);}
		if(button==1&&x>300&&x<350&&y>250&&y<300){delay(500);firstmemnext();}//next
		if(button==1&&x>300&&x<350&&y>200&&y<250){delay(500);exmemno=0;rsetno=1;break;}//reset
		if(button==1&&x>350&&x<400&&y>200&&y<250){delay(500);exmemno=0;endno=1;break;}//end
		}  //if end
		else
		{
		displaydata();
		getmousepos(&button,&x,&y);
		if(button==1&&x>100&&x<150&&y>200&&y<250){shiftldata();pos6='C';delay(500);}
		if(button==1&&x>150&&x<200&&y>200&&y<250){shiftldata();pos6='D';delay(500);}
		if(button==1&&x>200&&x<250&&y>200&&y<250){shiftldata();pos6='E';delay(500);}
		if(button==1&&x>250&&x<300&&y>200&&y<250){shiftldata();pos6='F';delay(500);}
		if(button==1&&x>100&&x<150&&y>250&&y<300){shiftldata();pos6='8';delay(500);}
		if(button==1&&x>150&&x<200&&y>250&&y<300){shiftldata();pos6='9';delay(500);}
		if(button==1&&x>200&&x<250&&y>250&&y<300){shiftldata();pos6='A';delay(500);}
		if(button==1&&x>250&&x<300&&y>250&&y<300){shiftldata();pos6='B';delay(500);}
		if(button==1&&x>100&&x<150&&y>300&&y<350){shiftldata();pos6='4';delay(500);}
		if(button==1&&x>150&&x<200&&y>300&&y<350){shiftldata();pos6='5';delay(500);}
		if(button==1&&x>200&&x<250&&y>300&&y<350){shiftldata();pos6='6';delay(500);}
		if(button==1&&x>250&&x<300&&y>300&&y<350){shiftldata();pos6='7';delay(500);}
		if(button==1&&x>100&&x<150&&y>350&&y<400){shiftldata();pos6='0';delay(500);}
		if(button==1&&x>150&&x<200&&y>350&&y<400){shiftldata();pos6='1';delay(500);}
		if(button==1&&x>200&&x<250&&y>350&&y<400){shiftldata();pos6='2';delay(500);}
		if(button==1&&x>250&&x<300&&y>350&&y<400){shiftldata();pos6='3';delay(500);}
		if(button==1&&x>300&&x<350&&y>250&&y<300){memnext();delay(500);}//next
		if(button==1&&x>350&&x<400&&y>250&&y<300){memprev();delay(500);} //prev
		if(button==1&&x>300&&x<350&&y>200&&y<250){exmemno=0;rsetno=1;break;}//reset
		if(button==1&&x>350&&x<400&&y>200&&y<250){exmemno=0;endno=1;break;}//end
		}//else end
	      }//while end
	}
//************************************************************************
//it is called only once at first time pressing of 'next' key
//and shows the data at 0000h location
void firstmemnext()
	{
	     getmemval();
	      displaydata();
	      firstnext++;
	}
//**********************************************************************
//it is called after first time pressing of 'next' key
//and shows the address(next to the current address)
//and data present at that memory location
void memnext()
	{
	      writedata();
	      addr++;
	      addrhex();
	      displayaddr();
	      getmemval();
	      displaydata();
	}
//***********************************************************************
//this function is called when 'prev' key is pressed
//and shows the address(previous to the current address)
//and data present at that memory location .
//NO previous address (or data) is shown when current address is 0000h
void memprev()
	{     if(addr>0)//checks if current address is greater than 0000h
	      {
	      hexaddr();
	      addr--;
	      pos5=mem[addr][0];
	      pos6=mem[addr][1];
	      addrhex();
	      }
	}
//***********************************************************************
//this function is called when 'exam reg' key is pressed
//it shows data value present in the register (shown by address field)
// and also takes data for input to the register (shown at address field)
void exreg()
	{     displayreg();
	      getregval();

	      while(1)
	      {
		displaydata();
		getmousepos(&button,&x,&y);
		if(button==1&&x>100&&x<150&&y>200&&y<250){shiftldata();pos6='C';delay(500);}
		if(button==1&&x>150&&x<200&&y>200&&y<250){shiftldata();pos6='D';delay(500);}
		if(button==1&&x>200&&x<250&&y>200&&y<250){shiftldata();pos6='E';delay(500);}
		if(button==1&&x>250&&x<300&&y>200&&y<250){shiftldata();pos6='F';delay(500);}
		if(button==1&&x>100&&x<150&&y>250&&y<300){shiftldata();pos6='8';delay(500);}
		if(button==1&&x>150&&x<200&&y>250&&y<300){shiftldata();pos6='9';delay(500);}
		if(button==1&&x>200&&x<250&&y>250&&y<300){shiftldata();pos6='A';delay(500);}
		if(button==1&&x>250&&x<300&&y>250&&y<300){shiftldata();pos6='B';delay(500);}
		if(button==1&&x>100&&x<150&&y>300&&y<350){shiftldata();pos6='4';delay(500);}
		if(button==1&&x>150&&x<200&&y>300&&y<350){shiftldata();pos6='5';delay(500);}
		if(button==1&&x>200&&x<250&&y>300&&y<350){shiftldata();pos6='6';delay(500);}
		if(button==1&&x>250&&x<300&&y>300&&y<350){shiftldata();pos6='7';delay(500);}
		if(button==1&&x>100&&x<150&&y>350&&y<400){shiftldata();pos6='0';delay(500);}
		if(button==1&&x>150&&x<200&&y>350&&y<400){shiftldata();pos6='1';delay(500);}
		if(button==1&&x>200&&x<250&&y>350&&y<400){shiftldata();pos6='2';delay(500);}
		if(button==1&&x>250&&x<300&&y>350&&y<400){shiftldata();pos6='3';delay(500);}
		if(button==1&&x>300&&x<350&&y>250&&y<300){delay(500);regnext();}//next
		if(button==1&&x>350&&x<400&&y>250&&y<300){delay(500);regprev();} //prev
		if(button==1&&x>300&&x<350&&y>350&&y<400){delay(500);exregno=0;gono=1;break;}
		if(button==1&&x>300&&x<350&&y>200&&y<250){delay(500);exregno=0;rsetno=1;break;}//reset
		if(button==1&&x>350&&x<400&&y>200&&y<250){delay(500);exregno=0;endno=1;break;}//end
	      }
	}
//*******************************************************************
//it displays register name in the address field
void displayreg()
	{     switch(regnum)
	      {
	      case 1:pos1=' ';pos2=' ';pos3=' ';pos4='A';       break;
	      case 2:pos1=' ';pos2=' ';pos3=' ';pos4='B';       break;
	      case 3:pos1=' ';pos2=' ';pos3=' ';pos4='C';       break;
	      case 4:pos1=' ';pos2=' ';pos3=' ';pos4='D';       break;
	      case 5:pos1=' ';pos2=' ';pos3=' ';pos4='E';       break;
	      case 6:pos1=' ';pos2=' ';pos3=' ';pos4='H';       break;
	      case 7:pos1=' ';pos2=' ';pos3=' ';pos4='L';       break;
	      case 8:pos1=' ';pos2='P';pos3='C';pos4='H';       break;
	      case 9:pos1=' ';pos2='P';pos3='C';pos4='L';       break;
	     case 10:pos1=' ';pos2='S';pos3='P';pos4='H';       break;
	     case 11:pos1=' ';pos2='S';pos3='P';pos4='L';       break;
	     case 12:pos1='F';pos2='L';pos3='A';pos4='G';       break;
	      }
	      displayaddr();
	}
//*****************************************************************
//it retrives data value present in registers
void getregval()
	{
	      switch(regnum)
	      {
	      case 1:pos5=Ah;pos6=Al;   break;
	      case 2:pos5=Bh;pos6=Bl;   break;
	      case 3:pos5=Ch;pos6=Cl;   break;
	      case 4:pos5=Dh;pos6=Dl;   break;
	      case 5:pos5=Eh;pos6=El;   break;
	      case 6:pos5=Hh;pos6=Hl;   break;
	      case 7:pos5=Lh;pos6=Ll;   break;
	      case 8:pos5=PCHh;pos6=PCHl;   break;
	      case 9:pos5=PCLh;pos6=PCLl;   break;
	     case 10:pos5=SPHh;pos6=SPHl;   break;
	     case 11:pos5=SPLh;pos6=SPLl;   break;
	     case 12:pos5=Fh;pos6=Fl;   break;
	      }
	}
//*****************************************************************
//it writes the data value present at data field to the register
//(shown by address field)
void writeregdata()
	{
	      switch(regnum)
	      {
	      case 1:Ah=pos5;   Al=pos6; A=hex8toint(Ah,Al);  break;
	      case 2:Bh=pos5;   Bl=pos6; B=hex8toint(Bh,Bl);  break;
	      case 3:Ch=pos5;   Cl=pos6; C=hex8toint(Ch,Cl);  break;
	      case 4:Dh=pos5;   Dl=pos6; D=hex8toint(Dh,Dl);  break;
	      case 5:Eh=pos5;   El=pos6; E=hex8toint(Eh,El);  break;
	      case 6:Hh=pos5;   Hl=pos6; H=hex8toint(Hh,Hl);  break;
	      case 7:Lh=pos5;   Ll=pos6; L=hex8toint(Lh,Ll);  break;
	      case 8:PCHh=pos5; PCHl=pos6; PC=hex16toint(PCHh,PCHl,PCLh,PCLl);  break;
	      case 9:PCLh=pos5; PCLl=pos6; PC=hex16toint(PCHh,PCHl,PCLh,PCLl);  break;
	     case 10:SPHh=pos5; SPHl=pos6; SP=hex16toint(SPHh,SPHl,SPLh,SPLl);  break;
	     case 11:SPLh=pos5; SPLl=pos6; SP=hex16toint(SPHh,SPHl,SPLh,SPLl);  break;
	     case 12:Fh=pos5;   Fl=pos6; break;
	     }
	}
//******************************************************************
//it writes the data value present at data field to the register
//(shown by address field) and displays data value present
//in the next register
void regnext()
	{
	      writeregdata();
	      if(regnum==12)regnum=1;
	      else regnum++;
	      displayreg();
	      getregval();
	}
//****************************************************************
//it displays data value present in the preveous register
void regprev()
	{
	      if(regnum==1) regnum=12;
	      else regnum--;
	      displayreg();
	      getregval();
	}
//*****************************************************************
//this function is called when 'reset' key is pressed.It resets all registers
//and all flags,but NOT memory locations
void rset()
	{
	A=0;B=0;C=0;D=0;E=0;H=0;L=0;PC=0;SP=0;BC=0;DE=0;HL=0;
	Ah='0';Al='0';Bh='0';Bl='0';Ch='0';Cl='0';Dh='0';Dl='0';
	Eh='0';El='0';Hh='0';Hl='0';Lh='0';Ll='0',IRh='0',IRl='0';
	PCHh='0';PCLh='0';SPHh='0';SPLh='0';   Fh='0';Fl='0';
	PCHl='0';PCLl='0';SPHl='0';SPLl='0';
	SFLAG=0;ZFLAG=0;ACFLAG=0;PFLAG=0;CYFLAG=0;
	pos1=1;pos2='U';pos3='S';pos4='E';pos5='M';pos6='E';
	pos1val=0;pos2val=0;pos3val=0;pos4val=0;pos5val=0;pos6val=0;
	firstnext=1;regnum=1;addr=0;
	displayaddr();
	displaydata();
	pos1='0';pos2='0';pos3='0';pos4='0';pos5='M';pos6='E';
	halt=0;
	rsetno=0;
	}
//************************************************************************
//this function is called when 'end' key is pressed . This one calls exit()
//to terminate the main() function i.e.the simulator is closed
void end()
	{exit(0);}
//************************************************************************
//it converts address from hexadecimal format to integer format
void hexaddr()
	{
	   addr=16*16*16*pos1val+16*16*pos2val+16*pos3val+pos4val;
	}
//***********************************************************************
//it converts address from integer format to hexadecimal format
void addrhex()
	{   int a,b,c,d,temp;
	    temp=addr;
	    d=temp%16;temp=temp-d;temp=temp/16;
	    c=temp%16;temp=temp-c;temp=temp/16;
	    b=temp%16;temp=temp-b;temp=temp/16;
	    a=temp%16;

	    pos1=inttochar(a);pos2=inttochar(b);pos3=inttochar(c);pos4=inttochar(d);
	    pos1val=a;pos2val=b;pos3val=c;pos4val=d;
	}
//*********************************************************************
//it shifts the positional characters (showing the address in address field)
//to left and also the positional values(integer value)
void shiftladdr()
	{
	pos1=pos2;pos2=pos3;pos3=pos4;
	pos1val=pos2val;pos2val=pos3val;pos3val=pos4val;
	}
//************************************************************************
//it prints hexadecimal address at address field
void displayaddr()
	{ 	 gotoxy(17,10);printf("%c",pos1); //pos 1
		 gotoxy(22,10);printf("%c",pos2); //pos 2
		 gotoxy(27,10);printf("%c",pos3); //pos 3
		 gotoxy(32,10);printf("%c",pos4); //pos 4
	}
//***********************************************************************
//it shifts the positional characters (showing the data in memory location
//at data field) to left
void shiftldata()
	{
	pos5=pos6;
	}
//***********************************************************************
//it prints data value at data field
void displaydata()
	{
		 gotoxy(42,10);printf("%c",pos5);  //pos 5
		 gotoxy(47,10);printf("%c",pos6);  //pos 6

	}
//**********************************************************************
//it goes to specific memory location and gets the data present there
void getmemval()
	{
	   hexaddr();
	   pos5=mem[addr][0];
	   pos6=mem[addr][1];
	}
//**********************************************************************
//it writes the data given by user(at data field) in to memory
void writedata()
	{  hexaddr();
	   mem[addr][0]=pos5;
	   mem[addr][1]=pos6;
	}
//*********************************************************************
//it converts integer values to its corresponding hexadecimal digits
char inttochar(int getval)
	{  char a;
	   switch(getval)
	    {
	case 0:a='0';break;
	case 1:a='1';break;
	case 2:a='2';break;
	case 3:a='3';break;
	case 4:a='4';break;
	case 5:a='5';break;
	case 6:a='6';break;
	case 7:a='7';break;
	case 8:a='8';break;
	case 9:a='9';break;
	case 10:a='A';break;
	case 11:a='B';break;
	case 12:a='C';break;
	case 13:a='D';break;
	case 14:a='E';break;
	case 15:a='F';break;
	    }
	return(a);
	}
//*******************************************************************
//it converts hexadecimal digits to its corresponding integer values
int chartoint(char ch)
	{ int val;
	 switch(ch)
	 {
	  case '0':val=0;break;
	  case '1':val=1;break;
	  case '2':val=2;break;
	  case '3':val=3;break;
	  case '4':val=4;break;
	  case '5':val=5;break;
	  case '6':val=6;break;
	  case '7':val=7;break;
	  case '8':val=8;break;
	  case '9':val=9;break;
	  case 'A':val=10;break;
	  case 'B':val=11;break;
	  case 'C':val=12;break;
	  case 'D':val=13;break;
	  case 'E':val=14;break;
	  case 'F':val=15;break;
	 }
	return(val);
	}
//****************************************************************
//it converts four digit hexadecimal values(eg.1A4Dh) to its corresponding
//integer values
int hex16toint(char a,char b,char c,char d)
	{  int a1,b1,c1,d1;
	   a1=chartoint(a);b1=chartoint(b);c1=chartoint(c);d1=chartoint(d);
	   a1=16*16*16*a1+16*16*b1+16*c1+d1;
	   return(a1);
	}
//***************************************************************
//it converts two digit hexadecimal values(eg.A4h) to its corresponding
//integer values
int hex8toint(char a,char b)
	{  int a1,b1;
	   a1=chartoint(a);b1=chartoint(b);
	   a1=16*a1+b1;
	   return(a1);
	}

///******************************************************************
//it converts integer value to four digit hexadecimal value
inttohex16(int val)
	{
	   int a,b,c,d,temp;
	    temp=val;
	    d=temp%16;temp=temp-d;temp=temp/16;
	    c=temp%16;temp=temp-c;temp=temp/16;
	    b=temp%16;temp=temp-b;temp=temp/16;
	    a=temp%16;
	    hex16[0]=inttochar(a);hex16[1]=inttochar(b);hex16[2]=inttochar(c);hex16[3]=inttochar(d);
	return;
	}

//*****************************************************************
//it converts integer value to two digit hexadecimal value
inttohex8(int val)
	{
	   int a,b,c,d,temp;
	    temp=val;
	    b=temp%16;temp=temp-b;temp=temp/16;
	    a=temp%16;
	    hex8[0]=inttochar(a);hex8[1]=inttochar(b);
	return;
	}
//************************************************************
//it converts an integer value to its 8 bit binary eqivalent
inttobits(int temp)
	{
	int i;
	for(i=0;i<=7;i++){d[i]=temp%2;temp=temp-d[i];temp=temp/2;}
	return;
	}
//**********************************************************
//it converts 8 bits  to its eqivalent integer value
bitstoint()
	{
	  int a;
	  a=128*d[7]+64*d[6]+32*d[5]+16*d[4]+8*d[3]+4*d[2]+2*d[1]+d[0];
	  return(a);
	}
//**********************************************************
	//  7-s 6-z 5-* 4-ac 3-* 2-p 1-* 0-cy
//it converts hexadecimal value present in Fh & Fl into its equivalent
//integer value
FhFltoflags()
	{int a,b,c,d,temp;
	    temp=chartoint(Fh);
	    d=temp%2;temp=temp-d;temp=temp/2;
	    c=temp%2;temp=temp-c;temp=temp/2;
	    b=temp%2;temp=temp-b;temp=temp/2;
	    a=temp%2;
		     SFLAG=a;ZFLAG=b;ACFLAG=d;
	    temp=chartoint(Fl);
	    d=temp%2;temp=temp-d;temp=temp/2;
	    c=temp%2;temp=temp-c;temp=temp/2;
	    b=temp%2;temp=temp-b;temp=temp/2;
	    a=temp%2;
		 PFLAG=b;CYFLAG=d;
	return;}
//***********************************************************
	//  7-s 6-z 5-* 4-ac 3-* 2-p 1-* 0-cy
//it converts integer value in flag register to its corresponding hexadecimal
//digits and stores them in Fh & Fl
flagstoFhFl()
	{int a,b;
	  a=2*2*2*SFLAG+2*2*ZFLAG+ACFLAG;  Fh=inttochar(a);
	   b=2*2*PFLAG+CYFLAG;  Fl=inttochar(b);
	return;}
//**********************************************************
//this function is called when 'go' key is pressed.It waits for an address
//to be given by user from where the 8085 assembly language program
//should be executed
void go()
	{
	pos1='0';pos2='0';pos3='0';pos4='0';pos5=' ';pos6=' ';
	displaydata();
	pos1val=0;pos2val=0;pos3val=0;pos4val=0;pos5val=0;pos6val=0;
	hexaddr();
	      while(1)
	      {
		displayaddr();
		getmousepos(&button,&x,&y);
		if(button==1&&x>100&&x<150&&y>200&&y<250){shiftladdr();pos4='C';pos4val=12;delay(500);}
		if(button==1&&x>150&&x<200&&y>200&&y<250){shiftladdr();pos4='D';pos4val=13;delay(500);}
		if(button==1&&x>200&&x<250&&y>200&&y<250){shiftladdr();pos4='E';pos4val=14;delay(500);}
		if(button==1&&x>250&&x<300&&y>200&&y<250){shiftladdr();pos4='F';pos4val=15;delay(500);}
		if(button==1&&x>100&&x<150&&y>250&&y<300){shiftladdr();pos4='8';pos4val=8;delay(500);}
		if(button==1&&x>150&&x<200&&y>250&&y<300){shiftladdr();pos4='9';pos4val=9;delay(500);}
		if(button==1&&x>200&&x<250&&y>250&&y<300){shiftladdr();pos4='A';pos4val=10;delay(500);}
		if(button==1&&x>250&&x<300&&y>250&&y<300){shiftladdr();pos4='B';pos4val=11;delay(500);}
		if(button==1&&x>100&&x<150&&y>300&&y<350){shiftladdr();pos4='4';pos4val=4;delay(500);}
		if(button==1&&x>150&&x<200&&y>300&&y<350){shiftladdr();pos4='5';pos4val=5;delay(500);}
		if(button==1&&x>200&&x<250&&y>300&&y<350){shiftladdr();pos4='6';pos4val=6;delay(500);}
		if(button==1&&x>250&&x<300&&y>300&&y<350){shiftladdr();pos4='7';pos4val=7;delay(500);}
		if(button==1&&x>100&&x<150&&y>350&&y<400){shiftladdr();pos4='0';pos4val=0;delay(500);}
		if(button==1&&x>150&&x<200&&y>350&&y<400){shiftladdr();pos4='1';pos4val=1;delay(500);}
		if(button==1&&x>200&&x<250&&y>350&&y<400){shiftladdr();pos4='2';pos4val=2;delay(500);}
		if(button==1&&x>250&&x<300&&y>350&&y<400){shiftladdr();pos4='3';pos4val=3;delay(500);}
		if(button==1&&x>300&&x<350&&y>200&&y<250){delay(500);gono=0;rsetno=1;break;}//reset
		if(button==1&&x>350&&x<400&&y>200&&y<250){delay(500);gono=0;endno=1;break;}//end
		if(button==1&&x>350&&x<400&&y>350&&y<400)
		    {delay(500);exec();gono=0;break;}//calls exec() to execute the program
		} //while end
	}
//************************************************************************
//it executes the 8085 assembly language code
void exec()
	{
	hexaddr();
	PC=addr;
	pos1='E';pos2='X';pos3='E';pos4='C';pos5=' ';pos6=' ';
	displayaddr();displaydata();
	while(halt==0)
	{
	IRh=mem[PC][0];IRl=mem[PC][1];
	//*****************************
	     if(IRh=='C' && IRl=='E'  )
	aci_8bit();
	else if(IRh=='8' && IRl=='8' ||IRh=='8' && IRl=='9' ||IRh=='8' && IRl=='A' ||IRh=='8' && IRl=='B' ||IRh=='8' && IRl=='C' ||IRh=='8' && IRl=='D' ||IRh=='8' && IRl=='E'||IRh=='8' && IRl=='F' )
	adc_x();
	else if(IRh=='8' && IRl=='0' ||IRh=='8' && IRl=='1' ||IRh=='8' && IRl=='2' ||IRh=='8' && IRl=='3' ||IRh=='8' && IRl=='4' ||IRh=='8' && IRl=='5' ||IRh=='8' && IRl=='6' ||IRh=='8' && IRl=='7' )
	add_x();
	else if(IRh=='C' && IRl=='6')
	adi_8bit();
	else if(IRh=='A' && IRl=='0' ||IRh=='A' && IRl=='1' ||IRh=='A' && IRl=='2' ||IRh=='A' && IRl=='3' ||IRh=='A' && IRl=='4' ||IRh=='A' && IRl=='5' ||IRh=='A' && IRl=='6' ||IRh=='A' && IRl=='7' )
	ana_x();
	else if(IRh=='E' && IRl=='6'  )
	ani_8bit();
	else if(IRh=='C'&&IRl=='D'||IRh=='D'&&IRl=='C'||IRh=='D'&&IRl=='4'||IRh=='F'&&IRl=='4'||IRh=='F'&&IRl=='C'||IRh=='E'&&IRl=='C'||IRh=='E'&&IRl=='4'||IRh=='C'&&IRl=='C'||IRh=='C'&&IRl=='4')
	call();
	else if(IRh=='2' && IRl=='F'  )
	cma();
	else if(IRh=='3' && IRl=='F'  )
	cmc();
	else if(IRh=='B' && IRl=='8' ||IRh=='B' && IRl=='9' ||IRh=='B' && IRl=='A' ||IRh=='B' && IRl=='B' ||IRh=='B' && IRl=='C' ||IRh=='B' && IRl=='D' ||IRh=='B' && IRl=='E' ||IRh=='B' && IRl=='F'  )
	cmp_x();
	else if(IRh=='F' && IRl=='E'  )
	cpi_8bit();
	else if(IRh=='2'&&IRl=='7')
	daa();
	else if(IRh=='0' && IRl=='9' ||IRh=='1' && IRl=='9' ||IRh=='2' && IRl=='9' ||IRh=='3' && IRl=='9'  )
	dad_x();
	else if(IRh=='0' && IRl=='5' ||IRh=='0' && IRl=='D'|| IRh=='1' && IRl=='5' ||IRh=='1' && IRl=='D' ||IRh=='2' && IRl=='5' ||IRh=='2' && IRl=='D' ||IRh=='3' && IRl=='5' ||IRh=='3' && IRl=='D' )
	dcr_x();
	else if(IRh=='0' && IRl=='B' ||IRh=='1' && IRl=='B' ||IRh=='2' && IRl=='B' ||IRh=='3' && IRl=='B'  )
	dcx_x();
	else if(IRh=='F' && IRl=='3'  )
	di();
	else if(IRh=='F' && IRl=='B'  )
	ei();
	else if(IRh=='7'&&IRl=='6')
	hlt();
	else if(IRh=='D'&&IRl=='B')
	in();
	else if(IRh=='0' && IRl=='4'|| IRh=='0' && IRl=='C' ||IRh=='1' && IRl=='4' ||IRh=='1' && IRl=='C'||IRh=='2' && IRl=='4'  ||IRh=='2' && IRl=='C' ||IRh=='3' && IRl=='4' ||IRh=='3' && IRl=='C' )
	inr_x();
	else if(IRh=='0' && IRl=='3' ||IRh=='1' && IRl=='3' ||IRh=='2' && IRl=='3' ||IRh=='3' && IRl=='3'  )
	inx_x();
	else if(IRh=='C' && IRl=='3'||IRh=='D' && IRl=='A'|| IRh=='D' && IRl=='2' ||IRh=='F' && IRl=='2' ||IRh=='F' && IRl=='A' ||IRh=='E' && IRl=='A' ||IRh=='E' && IRl=='2'||IRh=='C' && IRl=='A'  ||IRh=='C' && IRl=='2' )
	jmp();
	else if(IRh=='3' && IRl=='A'  )
	lda_16bit();
	else if(IRh=='0' && IRl=='A'|| IRh=='1' && IRl=='A'  )
	ldax_x();
	else if(IRh=='2' && IRl=='A'  )
	lhld_16bit();
	else if(IRh=='0'&&IRl=='1'||IRh=='1'&&IRl=='1'||IRh=='2'&&IRl=='1'||IRh=='3'&&IRl=='1')
	lxi_x_16bit();
	else if(IRh=='7'&&IRl=='F'||IRh=='7'&&IRl=='8'||IRh=='7'&&IRl=='9'||IRh=='7'&&IRl=='A'||IRh=='7'&&IRl=='B'||IRh=='7'&&IRl=='C'||IRh=='7'&&IRl=='D'||IRh=='7'&&IRl=='E')
	mov_a_x();
	else if(IRh=='4'&&IRl=='7'||IRh=='4'&&IRl=='0'||IRh=='4'&&IRl=='1'||IRh=='4'&&IRl=='2'||IRh=='4'&&IRl=='3'||IRh=='4'&&IRl=='4'||IRh=='4'&&IRl=='5'||IRh=='4'&&IRl=='6')
	mov_b_x();
	else if(IRh=='4'&&IRl=='F'||IRh=='4'&&IRl=='8'||IRh=='4'&&IRl=='9'||IRh=='4'&&IRl=='A'||IRh=='4'&&IRl=='B'||IRh=='4'&&IRl=='C'||IRh=='4'&&IRl=='D'||IRh=='4'&&IRl=='E')
	mov_c_x();
	else if(IRh=='5'&&IRl=='7'||IRh=='5'&&IRl=='0'||IRh=='5'&&IRl=='1'||IRh=='5'&&IRl=='2'||IRh=='5'&&IRl=='3'||IRh=='5'&&IRl=='4'||IRh=='5'&&IRl=='5'||IRh=='5'&&IRl=='6')
	mov_d_x();
	else if(IRh=='5'&&IRl=='F'||IRh=='5'&&IRl=='8'||IRh=='5'&&IRl=='9'||IRh=='5'&&IRl=='A'||IRh=='5'&&IRl=='B'||IRh=='5'&&IRl=='C'||IRh=='5'&&IRl=='D'||IRh=='5'&&IRl=='E')
	mov_e_x();
	else if(IRh=='6'&&IRl=='7'||IRh=='6'&&IRl=='0'||IRh=='6'&&IRl=='1'||IRh=='6'&&IRl=='2'||IRh=='6'&&IRl=='3'||IRh=='6'&&IRl=='4'||IRh=='6'&&IRl=='5'||IRh=='6'&&IRl=='6')
	mov_h_x();
	else if(IRh=='6'&&IRl=='F'||IRh=='6'&&IRl=='8'||IRh=='6'&&IRl=='9'||IRh=='6'&&IRl=='A'||IRh=='6'&&IRl=='B'||IRh=='6'&&IRl=='C'||IRh=='6'&&IRl=='D'||IRh=='6'&&IRl=='E')
	mov_l_x();
	else if(IRh=='7'&&IRl=='7'||IRh=='7'&&IRl=='0'||IRh=='7'&&IRl=='1'||IRh=='7'&&IRl=='2'||IRh=='7'&&IRl=='3'||IRh=='7'&&IRl=='4'||IRh=='7'&&IRl=='5')
	mov_m_x();
	else if(IRh=='0'&&IRl=='6'||IRh=='0'&&IRl=='E'||IRh=='1'&&IRl=='6'||IRh=='1'&&IRl=='E'||IRh=='2'&&IRl=='6'||IRh=='2'&&IRl=='E'||IRh=='3'&&IRl=='6'||IRh=='3'&&IRl=='E')
	mvi_x_8bit();
	else if(IRh=='0'&&IRl=='0')
	nop();
	else if(IRh=='B'&&IRl=='0'||IRh=='B'&&IRl=='1'||IRh=='B'&&IRl=='2'||IRh=='B'&&IRl=='3'||IRh=='B'&&IRl=='4'|IRh=='B'&&IRl=='5'|IRh=='B'&&IRl=='6'||IRh=='B'&&IRl=='7')
	ora_x();
	else if(IRh=='F'&&IRl=='6')
	ori_8bit();
	else if(IRh=='D'&&IRl=='3')
	out_8bit();
	else if(IRh=='E'&&IRl=='9')
	pchl();
	else if(IRh=='C'&&IRl=='1'||IRh=='D'&&IRl=='1'||IRh=='E'&&IRl=='1'||IRh=='F'&&IRl=='1')
	pop_x();
	else if(IRh=='C'&&IRl=='5'||IRh=='D'&&IRl=='5'||IRh=='E'&&IRl=='5'||IRh=='F'&&IRl=='5')
	push_x();
	else if(IRh=='C'&&IRl=='9'||IRh=='D'&&IRl=='8'||IRh=='D'&&IRl=='0'||IRh=='F'&&IRl=='8'||IRh=='F'&&IRl=='0'||IRh=='E'&&IRl=='8'||IRh=='E'&&IRl=='0'||IRh=='C'&&IRl=='8'||IRh=='C'&&IRl=='0')
	ret();
	else if(IRh=='2'&&IRl=='0')
	rim();
	else if(IRh=='1'&&IRl=='7'||IRh=='1'&&IRl=='F'||IRh=='0'&&IRl=='7'||IRh=='0'&&IRl=='F')
	rotate();//ral 17, rar 1f,rlc 07,rrc 0f.
	else if(IRh=='C'&&IRl=='7'||IRh=='C'&&IRl=='F'||IRh=='D'&&IRl=='7'||IRh=='D'&&IRl=='F'||IRh=='E'&&IRl=='7'||IRh=='E'&&IRl=='F'||IRh=='F'&&IRl=='7'||IRh=='F'&&IRl=='F')
	rst();
	else if(IRh=='9' && IRl=='8' ||IRh=='9' && IRl=='9' ||IRh=='9' && IRl=='A' ||IRh=='9' && IRl=='B' ||IRh=='9' && IRl=='C' ||IRh=='9' && IRl=='D' ||IRh=='9' && IRl=='E'||IRh=='9' && IRl=='F' )
	sbb_x();
	else if(IRh=='D' && IRl=='E'  )
	sbi_8bit();
	else if(IRh=='2' && IRl=='2'  )
	shld_16bit();
	else if(IRh=='3'&&IRl=='0')
	sim();
	else if(IRh=='F' && IRl=='9'  )
	sphl();
	else if(IRh=='3'&&IRl=='2')
	sta_16bit();
	else if(IRh=='0' && IRl=='2' || IRh=='1' && IRl=='2'  )
	stax_x();
	else if(IRh=='3' && IRl=='7'  )
	stc();
	else if(IRh=='9' && IRl=='0' ||IRh=='9' && IRl=='1' ||IRh=='9' && IRl=='2' ||IRh=='9' && IRl=='3' ||IRh=='9' && IRl=='4' ||IRh=='9' && IRl=='5' ||IRh=='9' && IRl=='6' ||IRh=='9' && IRl=='7' )
	sub_x();
	else if(IRh=='D' && IRl=='6'  )
	sui_8bit();
	else if(IRh=='E' && IRl=='B'  )
	xchg();
	else if(IRh=='A'&&IRl=='8'||IRh=='A'&&IRl=='9'||IRh=='A'&&IRl=='A'||IRh=='A'&&IRl=='B'||IRh=='A'&&IRl=='C'||IRh=='A'&&IRl=='D'||IRh=='A'&&IRl=='E'||IRh=='A'&&IRl=='F')
	xra_x();
	else if(IRh=='E'&&IRl=='E')
	xri_8bit();
	else if(IRh=='E' && IRl=='3'  )
	xthl();
	}
	//***********
	pos1=' ';pos2='E';pos3='N';pos4='D';pos5=' ';pos6=' ';
	displayaddr(); displaydata();
	}
//***********************************************************
//this is a 2 byte instruction.
aci_8bit()
	{
	 int a,b,c;
	 a=hex8toint(Ah,Al);
	 b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	 c=a+b+CYFLAG;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];
	 PC+=2;
	 return ;
	}
//**********************************************************
//this is a 1 byte instruction.
adc_x()
	{
	int a,b,c,addr1;
	if(IRh=='8' && IRl=='F') //adc_a
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ah,Al);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='8') //adc_b
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Bh,Bl);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='9') //adc_c
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ch,Cl);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='A') //adc_d
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Dh,Dl);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='B') //adc_e
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Eh,El);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='C') //adc_h
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Hh,Hl);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='D') //adc_l
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Lh,Ll);
	     c=a+b+CYFLAG;
	     //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='E') //adc_m
	   {
	      addr1=hex16toint(Hh,Hl,Lh,Ll);
	      a=hex8toint(Ah,Al);
	      b=hex8toint(mem[addr1][0],mem[addr1][1]);
	      c=a+b+CYFLAG;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	      inttohex8(c);
	      Ah=hex8[0];Al=hex8[1];
	   }
	PC+=1;return;
	}
//*********************************************************
//this is a 1 byte instruction.
add_x()
	{
	int a,b,c,addr1;
	if(IRh=='8' && IRl=='7') //add_a
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ah,Al);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='0') //add_b
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Bh,Bl);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='1') //add_c
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ch,Cl);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='2') //add_d
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Dh,Dl);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='3') //add_e
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Eh,El);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='4') //add_h
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Hh,Hl);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='5') //add_l
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Lh,Ll);
	     c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='8' && IRl=='6') //add_m
	   {
	      addr1=hex16toint(Hh,Hl,Lh,Ll);
	      a=hex8toint(Ah,Al);
	      b=hex8toint(mem[addr1][0],mem[addr1][1]);
	      c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	      inttohex8(c);
	      Ah=hex8[0];Al=hex8[1];
	   }
	PC+=1;return;
	}
//*************************************************************
//this is a 2 byte instruction.
adi_8bit()
	{
	int a,b,c;
	 a=hex8toint(Ah,Al);
	 b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	 c=a+b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];
	 PC+=2;
	 return;
	}
//***********************************************************
//this is a 1 byte instruction.
ana_x()
	{ int a,b,c,addr1;
	if(IRh=='A' && IRl=='7') //ana_a
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Ah,Al);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='0') //ana_b
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Bh,Bl);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='1') //ana_c
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Ch,Cl);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='2') //ana_d
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Dh,Dl);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='3') //ana_e
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Eh,El);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='4') //ana_h
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Hh,Hl);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='5') //ana_l
	     {
	     a=hex8toint(Ah,Al);
	     b=hex8toint(Lh,Ll);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	if(IRh=='A' && IRl=='6') //ana_m
	     {
	     addr1=hex16toint(Hh,Hl,Lh,Ll);
	     a=hex8toint(Ah,Al);
	     b=hex8toint(mem[addr1][0],mem[addr1][1]);
	     c=a&b;
	      //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	     }
	PC+=1;return;
	}
//***********************************************************
//this is a 2 byte instruction.
ani_8bit()
	{ int a,b,c;
	 a=hex8toint(Ah,Al);
	 b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	 c=a&b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];
	 PC+=2;
	 return;
	}
//**********************************************************
//this is a 3 byte instruction.
call()
	{
	     if(IRh=='C'&&IRl=='D')// call 16bit;
		{
		inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
	else if(IRh=='D'&&IRl=='C')//  cc  ; if cy=1;
	{
		if(CYFLAG==1)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='D'&&IRl=='4')//  cnc ; if cy=0;
	{
		if(CYFLAG==0)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='F'&&IRl=='4')//  cp  ; if s=0;
	{
		if(SFLAG==0)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='F'&&IRl=='C')//  cm  ; if s=1;
	{
		if(SFLAG==1)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='E'&&IRl=='C')//  cpe ; if p=1 (even);
	{
		if(PFLAG==1)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='E'&&IRl=='4')//  cpo ; if p=0;
	{
		if(PFLAG==0)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='C'&&IRl=='C')//  cz  ; if z=1;
	{
		if(ZFLAG==1)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	else if(IRh=='C'&&IRl=='4')//  cnz ; if z=0;
	{
		if(ZFLAG==0)
		{inttohex16(PC+3);
		SP--;stack[SP][0]=hex16[0];stack[SP][1]=hex16[1];
		SP--;stack[SP][0]=hex16[2];stack[SP][1]=hex16[3];
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
		else {PC+=3;return;}
	}
	return;
	}
//*****************************************************************
//this is a 1 byte instruction.
cma()
	{     int a,c,i;
	      a=hex8toint(Ah,Al);
	      inttobits(a);
	      for(i=0;i<=7;i++)
		 {
		 if(d[i]==0) d[i]=1;
		 if(d[i]==1) d[i]=0;
		 }
	      c=bitstoint();
	      inttohex8(c);
	      Ah=hex8[0];Al=hex8[1];
	PC+=1;return;
	}
//*******************************************************
//this is a 1 byte instruction.
cmc()
	{
	  if(CYFLAG==1)  CYFLAG=0;
	  if(CYFLAG==0)  CYFLAG=1;
	  PC+=1;return;
	}
//********************************************************
//this is a 1 byte instruction.
cmp_x()
	{int  a,b,addr1;
	if(IRh=='B' && IRl=='F') //cmp_a
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Ah,Al);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='8') //cmp_b
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Bh,Bl);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='9') //cmp_c
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Ch,Cl);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='A') //cmp_d
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Dh,Dl);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='B') //cmp_e
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Eh,El);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='C') //cmp_h
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Hh,Hl);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='D') //cmp_l
		{
		  a=hex8toint(Ah,Al);
		  b=hex8toint(Lh,Ll);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	if(IRh=='B' && IRl=='E') //cmp_m
		{
		  addr1=hex16toint(Hh,Hl,Lh,Ll);
		  a=hex8toint(Ah,Al);
		  b=hex8toint(mem[addr1][0],mem[addr1][1]);
		  if(a<b) {CYFLAG=1;ZFLAG=0;}
		  if(a==b) {CYFLAG=0;ZFLAG=1;}
		  if(a>b) {CYFLAG=0;ZFLAG=0;}
		}
	PC+=1;return;
	}
//*************************************************************
//this is a 2 byte instruction.
cpi_8bit()
	{     int a,b,c;
	      a=hex8toint(Ah,Al);
	      b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	      if(a<b) {CYFLAG=1;ZFLAG=0;}
	      if(a==b) {CYFLAG=0;ZFLAG=1;}
	      if(a>b) {CYFLAG=0;ZFLAG=0;}
	      PC+=2;return;
	}
//***********************************************************
//this is a 1 byte instruction.
daa()
	{ int a,b,c,d;
	  char ch1,ch2;
	  a=hex8toint(Ah,Al);
	  b=a%10;a=a-b;a=a/10;
	  c=a%10;a=a-c;a=a/10;
	  d=a%10;
	  if(d>0)CYFLAG=1;
	  ch1=inttochar(c);ch2=inttochar(b);
	  Ah=ch1;Al=ch2;
	  PC+=1;
	  return;
	}
//***********************************************************
//this is a 1 byte instruction.
dad_x()
	{int a,b,c;
	if(IRh=='0' && IRl=='9') //dad_b
	   { a=hex16toint(Hh,Hl,Lh,Ll);
	     b=hex16toint(Bh,Bl,Ch,Cl);
	     c=a+b;
	     inttohex16(c);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	if(IRh=='1' && IRl=='9') //dad_d
	   { a=hex16toint(Hh,Hl,Lh,Ll);
	     b=hex16toint(Dh,Dl,Eh,El);
	     c=a+b;
	     inttohex16(c);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	if(IRh=='3' && IRl=='9') //dad_sp
	   { a=hex16toint(Hh,Hl,Lh,Ll);
	     b=hex16toint(SPHh,SPHl,SPLh,SPLl);
	     c=a+b;
	     inttohex16(c);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	if(IRh=='2' && IRl=='9') //dad_h
	   { a=hex16toint(Hh,Hl,Lh,Ll);
	     b=hex16toint(Hh,Hl,Lh,Ll);
	     c=a+b;
	     inttohex16(c);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	PC+=1;
	return;
	}
//*********************************************************
//this is a 1 byte instruction.
dcr_x()
	{int a,addr1;
	if(IRh=='3' && IRl=='D') //dcr_a
	   {
	     a=hex8toint(Ah,Al);
	     a=a-1;
	     //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='0' && IRl=='5') //dcr_b
	   {
	     a=hex8toint(Bh,Bl);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Bh=hex8[0];Bl=hex8[1];
	   }
	if(IRh=='0' && IRl=='D') //dcr_c
	   {
	     a=hex8toint(Ch,Cl);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Ch=hex8[0];Cl=hex8[1];
	   }
	if(IRh=='1' && IRl=='5') //dcr_d
	   {
	     a=hex8toint(Dh,Dl);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Dh=hex8[0];Dl=hex8[1];
	   }
	if(IRh=='1' && IRl=='D') //dcr_e
	   {
	     a=hex8toint(Eh,El);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Eh=hex8[0];El=hex8[1];
	   }
	if(IRh=='2' && IRl=='5') //dcr_h
	   {
	     a=hex8toint(Hh,Hl);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Hh=hex8[0];Hl=hex8[1];
	   }
	if(IRh=='2' && IRl=='D') //dcr_l
	   {
	     a=hex8toint(Lh,Ll);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Lh=hex8[0];Ll=hex8[1];
	   }
	if(IRh=='3' && IRl=='5') //dcr_m
	   {
	     addr1=hex16toint(Hh,Hl,Lh,Ll);
	     a=hex8toint(mem[addr1][0],mem[addr1][1]);
	     a=a-1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     mem[addr1][0]=hex8[0];mem[addr1][1]=hex8[1];
	   }
	PC+=1;return;
	}
//************************************************************
//this is a 1 byte instruction.
dcx_x()
	{int a;
	if(IRh=='2' && IRl=='B') //dcx_h
	   {
	     a=hex16toint(Hh,Hl,Lh,Ll);
	     a=a-1;
	     inttohex16(a);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	if(IRh=='0' && IRl=='B') //dcx_b
	   {
	     a=hex16toint(Bh,Bl,Ch,Cl);
	     a=a-1;
	     inttohex16(a);
	     Bh=hex16[0];Bl=hex16[1];Ch=hex16[2];Cl=hex16[3];
	   }
	if(IRh=='1' && IRl=='B') //dcx_d
	   {
	     a=hex16toint(Dh,Dl,Eh,El);
	     a=a-1;
	     inttohex16(a);
	     Dh=hex16[0];Dl=hex16[1];Eh=hex16[2];El=hex16[3];
	   }
	if(IRh=='3' && IRl=='B') //dcx_SP
	   {
	     a=hex16toint(SPHh,SPHl,SPLh,SPLl);
	     a=a-1;
	     inttohex16(a);
	     SPHh=hex16[0];SPHl=hex16[1];SPLh=hex16[2];SPLl=hex16[3];
	   }
	PC+=1;return;
	}
//*************************************************************
//this is a 1 byte instruction.
di()   {PC+=1;return;}  //not completed
//**************************************************************
//this is a 1 byte instruction.
ei()   {PC+=1;return;}  //not completed
//**************************************************************
//this is a 1 byte instruction.
hlt()	{halt=1;PC=0;return;}
//**************************************************************
//this is a 2 byte instruction.
in()   {PC+=2;return;}  //not completed
//**************************************************************
//this is a 1 byte instruction.
inr_x()
	{int a,addr1;
	if(IRh=='3' && IRl=='C') //inr_a
	   {
	     a=hex8toint(Ah,Al);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='0' && IRl=='4') //inr_b
	   {
	     a=hex8toint(Bh,Bl);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Bh=hex8[0];Bl=hex8[1];
	   }
	if(IRh=='0' && IRl=='C') //inr_c
	   {
	     a=hex8toint(Ch,Cl);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Ch=hex8[0];Cl=hex8[1];
	   }
	if(IRh=='1' && IRl=='4') //inr_d
	   {
	     a=hex8toint(Dh,Dl);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Dh=hex8[0];Dl=hex8[1];
	   }
	if(IRh=='1' && IRl=='C') //inr_e
	   {
	     a=hex8toint(Eh,El);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Eh=hex8[0];El=hex8[1];
	   }
	if(IRh=='2' && IRl=='4') //inr_h
	   {
	     a=hex8toint(Hh,Hl);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Hh=hex8[0];Hl=hex8[1];
	   }
	if(IRh=='2' && IRl=='C') //inr_l
	   {
	     a=hex8toint(Lh,Ll);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     Lh=hex8[0];Ll=hex8[1];
	   }
	if(IRh=='3' && IRl=='4') //inr_m
	   {
	     addr1=hex16toint(Hh,Hl,Lh,Ll);
	     a=hex8toint(mem[addr1][0],mem[addr1][1]);
	     a=a+1;
	      //////////
	 if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;}
	 //////////
	     inttohex8(a);
	     mem[addr1][0]=hex8[0];mem[addr1][1]=hex8[1];
	   }
	PC+=1;return(0);
	}
//**************************************************************
//this is a 1 byte instruction.
inx_x()
	{int a;
	if(IRh=='2' && IRl=='3') //inx_h
	   {
	     a=hex16toint(Hh,Hl,Lh,Ll);
	     a=a+1;
	     inttohex16(a);
	     Hh=hex16[0];Hl=hex16[1];Lh=hex16[2];Ll=hex16[3];
	   }
	if(IRh=='0' && IRl=='3') //inx_b
	   {
	     a=hex16toint(Bh,Bl,Ch,Cl);
	     a=a+1;
	     inttohex16(a);
	     Bh=hex16[0];Bl=hex16[1];Ch=hex16[2];Cl=hex16[3];
	   }
	if(IRh=='1' && IRl=='3') //inx_d
	   {
	     a=hex16toint(Dh,Dl,Eh,El);
	     a=a+1;
	     inttohex16(a);
	     Dh=hex16[0];Dl=hex16[1];Eh=hex16[2];El=hex16[3];
	   }
	if(IRh=='3' && IRl=='3') //inx_SP
	   {
	     a=hex16toint(SPHh,SPHl,SPLh,SPLl);
	     a=a+1;
	     inttohex16(a);
	     SPHh=hex16[0];SPHl=hex16[1];SPLh=hex16[2];SPLl=hex16[3];
	   }
	PC+=1;return;
	}
//*****************************************************************
//this is a 3 byte instruction.
jmp()
	{     if(IRh=='C' && IRl=='3') //jmp NO CONDITION
		{
		PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
		}
	      if(IRh=='D' && IRl=='A') //jc  IF CY=1
		{
		if(CYFLAG==1){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='D' && IRl=='2') //jnc IF CY=0
		{
		if(CYFLAG==0){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='F' && IRl=='2') //jp  IF S=0
		{
		if(SFLAG==0){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='F' && IRl=='A') //jm  IF S=1
		{
		if(SFLAG==1){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='C' && IRl=='2') //jnz IF Z=0
		{
		if(ZFLAG==0){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='C' && IRl=='A') //jz   IF Z=1
		{
		if(ZFLAG==1){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='E' && IRl=='A') //jpe  IF P=1
		{
		if(PFLAG==1){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	      if(IRh=='E' && IRl=='2') //jpo  IF P=0
		{
		if(PFLAG==0){PC=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);}
		else{PC+=3;return;}
		}
	return;
	}
//**************************************************************
//this is a 3 byte instruction.
lda_16bit()
	{     int addr1;
	      addr1=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
	      Ah=mem[addr1][0];Al=mem[addr1][1];
	PC+=3;return;
	}
//**************************************************************
//this is a 1 byte instruction.
ldax_x()
	{     int addr1;
	      if(IRh=='0' && IRl=='A') //ldax_b
		 {
		  addr1=hex16toint(Bh,Bl,Ch,Cl);
		  Ah=mem[addr1][0]; Al=mem[addr1][1];
		 }
	      if(IRh=='1' && IRl=='A') //ldax_d
		 {
		   addr1=hex16toint(Dh,Dl,Eh,El);
		   Ah=mem[addr1][0]; Al=mem[addr1][1];
		 }
	PC+=1;return;
	}
//************************************************************
//this is a 3 byte instruction.
lhld_16bit()
	{     int addr1;
	      addr1=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]);
	      Lh=mem[addr1][0]; Ll=mem[addr1][1];
	      Hh=mem[addr1+1][0]; Hl=mem[addr1+1][1];
	PC+=3;return;
	}
//****************************************************************
//this is a 3 byte instruction.
lxi_x_16bit()
	{
	if(IRh=='0'&&IRl=='1'){//lxi b,16bit
	     Ch=mem[PC+1][0];Cl=mem[PC+1][1];
	     Bh=mem[PC+2][0];Bl=mem[PC+2][1];}
	else if(IRh=='1'&&IRl=='1'){//lxi d,16bit
	     Eh=mem[PC+1][0];El=mem[PC+1][1];
	     Dh=mem[PC+2][0];Dl=mem[PC+2][1];}
	else if(IRh=='2'&&IRl=='1'){//lxi h,16bit
	     Lh=mem[PC+1][0];Ll=mem[PC+1][1];
	     Hh=mem[PC+2][0];Hl=mem[PC+2][1];}
	else if(IRh=='3'&&IRl=='1'){//lxi sp,16bit
	     SPLh=mem[PC+1][0];SPLl=mem[PC+1][1];
	     SPHh=mem[PC+2][0];SPHl=mem[PC+2][1];}
	PC+=3;
	return;
	}
//**************************************************************
//this is a 1 byte instruction.
mov_a_x()
	{
	int addr1;
	     if(IRh=='7'&&IRl=='F'){//mov a,a
			Ah=Ah;Al=Al;}
	else if(IRh=='7'&&IRl=='8'){//mov a,b
			Ah=Bh;Al=Bl;}
	else if(IRh=='7'&&IRl=='9'){//mov a,c
			Ah=Ch;Al=Cl;}
	else if(IRh=='7'&&IRl=='A'){//mov a,d
			Ah=Dh;Al=Dl;}
	else if(IRh=='7'&&IRl=='B'){//mov a,e
			Ah=Eh;Al=El;}
	else if(IRh=='7'&&IRl=='C'){//mov a,h
			Ah=Hh;Al=Hl;}
	else if(IRh=='7'&&IRl=='D'){//mov a,l
			Ah=Lh;Al=Ll;}
	else if(IRh=='7'&&IRl=='E'){//mov a,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Ah=mem[addr1][0]; Al=mem[addr1][1];}
	PC+=1;
	return;
	}
//*************************************************************
//this is a 1 byte instruction.
mov_b_x()
	{
	int addr1;
	     if(IRh=='4'&&IRl=='7'){//mov b,a
			Bh=Ah;Bl=Al;}
	else if(IRh=='4'&&IRl=='0'){//mov b,b
			Bh=Bh;Bl=Bl;}
	else if(IRh=='4'&&IRl=='1'){//mov b,c
			Bh=Ch;Bl=Cl;}
	else if(IRh=='4'&&IRl=='2'){//mov b,d
			Bh=Dh;Bl=Dl;}
	else if(IRh=='4'&&IRl=='3'){//mov b,e
			Bh=Eh;Bl=El;}
	else if(IRh=='4'&&IRl=='4'){//mov b,h
			Bh=Hh;Bl=Hl;}
	else if(IRh=='4'&&IRl=='5'){//mov b,l
			Bh=Lh;Bl=Ll;}
	else if(IRh=='4'&&IRl=='6'){//mov b,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Bh=mem[addr1][0]; Bl=mem[addr1][1];}
	PC+=1;
	return;
	}
//*************************************************************
//this is a 1 byte instruction.
mov_c_x()
	{
	int addr1;
	     if(IRh=='4'&&IRl=='F'){//mov c,a
			Ch=Ah;Cl=Al;}
	else if(IRh=='4'&&IRl=='8'){//mov c,b
			Ch=Bh;Cl=Bl;}
	else if(IRh=='4'&&IRl=='9'){//mov c,c
			Ch=Ch;Cl=Cl;}
	else if(IRh=='4'&&IRl=='A'){//mov c,d
			Ch=Dh;Cl=Dl;}
	else if(IRh=='4'&&IRl=='B'){//mov c,e
			Ch=Eh;Cl=El;}
	else if(IRh=='4'&&IRl=='C'){//mov c,h
			Ch=Hh;Cl=Hl;}
	else if(IRh=='4'&&IRl=='D'){//mov c,l
			Ch=Lh;Cl=Ll;}
	else if(IRh=='4'&&IRl=='E'){//mov c,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Ch=mem[addr1][0]; Cl=mem[addr1][1];}
	PC+=1;
	return;
	}
//********************************************************
//this is a 1 byte instruction.
mov_d_x()
	{
	int addr1;

	     if(IRh=='5'&&IRl=='7'){//mov d,a
			Dh=Ah;Dl=Al;}
	else if(IRh=='5'&&IRl=='0'){//mov d,b
			Dh=Bh;Dl=Bl;}
	else if(IRh=='5'&&IRl=='1'){//mov d,c
			Dh=Ch;Dl=Cl;}
	else if(IRh=='5'&&IRl=='2'){//mov d,d
			Dh=Dh;Dl=Dl;}
	else if(IRh=='5'&&IRl=='3'){//mov d,e
			Dh=Eh;Dl=El;}
	else if(IRh=='5'&&IRl=='4'){//mov d,h
			Dh=Hh;Dl=Hl;}
	else if(IRh=='5'&&IRl=='5'){//mov d,l
			Dh=Lh;Dl=Ll;}
	else if(IRh=='5'&&IRl=='6'){//mov d,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Dh=mem[addr1][0]; Dl=mem[addr1][1];}
	PC+=1;
	return;
	}
//************************************************************
//this is a 1 byte instruction.
mov_e_x()
	{
	int addr1;
	     if(IRh=='5'&&IRl=='F'){//mov e,a
			Eh=Ah;El=Al;}
	else if(IRh=='5'&&IRl=='8'){//mov e,b
			Eh=Bh;El=Bl;}
	else if(IRh=='5'&&IRl=='9'){//mov e,c
			Eh=Ch;El=Cl;}
	else if(IRh=='5'&&IRl=='A'){//mov e,d
			Eh=Dh;El=Dl;}
	else if(IRh=='5'&&IRl=='B'){//mov e,e
			Eh=Eh;El=El;}
	else if(IRh=='5'&&IRl=='C'){//mov e,h
			Eh=Hh;El=Hl;}
	else if(IRh=='5'&&IRl=='D'){//mov e,l
			Eh=Lh;El=Ll;}
	else if(IRh=='5'&&IRl=='E'){//mov e,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Eh=mem[addr1][0]; El=mem[addr1][1];}
	PC+=1;
	return;
	}
//*************************************************************
//this is a 1 byte instruction.
mov_h_x()
	{
	int addr1;

	     if(IRh=='6'&&IRl=='7'){//mov h,a
			Hh=Ah;Hl=Al;}
	else if(IRh=='6'&&IRl=='0'){//mov h,b
			Hh=Bh;Hl=Bl;}
	else if(IRh=='6'&&IRl=='1'){//mov h,c
			Hh=Ch;Hl=Cl;}
	else if(IRh=='6'&&IRl=='2'){//mov h,d
			Hh=Dh;Hl=Dl;}
	else if(IRh=='6'&&IRl=='3'){//mov h,e
			Hh=Eh;Hl=El;}
	else if(IRh=='6'&&IRl=='4'){//mov h,h
			Hh=Hh;Hl=Hl;}
	else if(IRh=='6'&&IRl=='5'){//mov h,l
			Hh=Lh;Hl=Ll;}
	else if(IRh=='6'&&IRl=='6'){//mov h,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Hh=mem[addr1][0]; Hl=mem[addr1][1];}
	PC+=1;
	return;
	}
//**************************************************************
//this is a 1 byte instruction.
mov_l_x()
	{
	int addr1;
	     if(IRh=='6'&&IRl=='F'){//mov l,a
			Lh=Ah;Ll=Al;}
	else if(IRh=='6'&&IRl=='8'){//mov l,b
			Lh=Bh;Ll=Bl;}
	else if(IRh=='6'&&IRl=='9'){//mov l,c
			Lh=Ch;Ll=Cl;}
	else if(IRh=='6'&&IRl=='A'){//mov l,d
			Lh=Dh;Ll=Dl;}
	else if(IRh=='6'&&IRl=='B'){//mov l,e
			Lh=Eh;Ll=El;}
	else if(IRh=='6'&&IRl=='C'){//mov l,h
			Lh=Hh;Ll=Hl;}
	else if(IRh=='6'&&IRl=='D'){//mov l,l
			Lh=Lh;Ll=Ll;}
	else if(IRh=='6'&&IRl=='E'){//mov l,m
		addr1=hex16toint(Hh,Hl,Lh,Ll);
		Lh=mem[addr1][0]; Ll=mem[addr1][1];}
	PC+=1;
	return;
	}
//*************************************************************
//this is a 1 byte instruction.
mov_m_x()
	{
	int addr1;
	addr1=hex16toint(Hh,Hl,Lh,Ll);
	     if(IRh=='7'&&IRl=='7'){//mov m,a
			mem[addr1][0]=Ah;mem[addr1][1]=Al;}
	else if(IRh=='7'&&IRl=='0'){//mov m,b
			mem[addr1][0]=Bh;mem[addr1][1]=Bl;}
	else if(IRh=='7'&&IRl=='1'){//mov m,c
			mem[addr1][0]=Ch;mem[addr1][1]=Cl;}
	else if(IRh=='7'&&IRl=='2'){//mov m,d
			mem[addr1][0]=Dh;mem[addr1][1]=Dl;}
	else if(IRh=='7'&&IRl=='3'){//mov m,e
			mem[addr1][0]=Eh;mem[addr1][1]=El;}
	else if(IRh=='7'&&IRl=='4'){//mov m,h
			mem[addr1][0]=Hh;mem[addr1][1]=Hl;}
	else if(IRh=='7'&&IRl=='5'){//mov m,l
			mem[addr1][0]=Lh;mem[addr1][1]=Ll;}
	PC+=1;
	return;
	}
//************************************************************
//this is a 2 byte instruction.
mvi_x_8bit()
	{
	int addr1;
	addr1=hex16toint(Hh,Hl,Lh,Ll);
	     if(IRh=='3'&&IRl=='E')//mvi_a_8bit
	     { Ah=mem[PC+1][0];Al=mem[PC+1][1];}
	else if(IRh=='0'&&IRl=='6')//mvi_b_8bit
	{ Bh=mem[PC+1][0];Bl=mem[PC+1][1];}
	else if(IRh=='0'&&IRl=='E')//mvi_c_8bit
	{ Ch=mem[PC+1][0];Cl=mem[PC+1][1];}
	else if(IRh=='1'&&IRl=='6')//mvi_d_8bit
	{ Dh=mem[PC+1][0];Dl=mem[PC+1][1];}
	else if(IRh=='1'&&IRl=='E')//mvi_e_8bit
	{ Eh=mem[PC+1][0];El=mem[PC+1][1];}
	else if(IRh=='2'&&IRl=='6')//mvi_h_8bit
	{ Hh=mem[PC+1][0];Hl=mem[PC+1][1];}
	else if(IRh=='2'&&IRl=='E')//mvi_l_8bit
	{ Lh=mem[PC+1][0];Ll=mem[PC+1][1];}
	else if(IRh=='3'&&IRl=='6')//mvi_m_8bit
	{ mem[addr1][0]=mem[PC+1][0];
			     mem[addr1][1]=mem[PC+1][1];}
	PC+=2;
	return;
	}
//***********************************************************
//this is a 1 byte instruction.
nop()	{PC+=1;return;}
//***********************************************************
//this is a 1 byte instruction.
ora_x()
	{
	int a,b,c,addr1;
	addr1=hex16toint(Hh,Hl,Lh,Ll);
	if(IRh=='B'&&IRl=='7')//ora_a
	 {a=hex8toint(Ah,Al);b=hex8toint(Ah,Al);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='0')//ora_b
	 {a=hex8toint(Ah,Al);b=hex8toint(Bh,Bl);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='1')//ora_c
	 {a=hex8toint(Ah,Al);b=hex8toint(Ch,Cl);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='2')//ora_d
	 {a=hex8toint(Ah,Al);b=hex8toint(Dh,Dl);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='3')//ora_e
	 {a=hex8toint(Ah,Al);b=hex8toint(Eh,El);	c=a|b;
	 /////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='4')//ora_h
	 {a=hex8toint(Ah,Al);b=hex8toint(Hh,Hl);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='5')//ora_l
	 {a=hex8toint(Ah,Al);b=hex8toint(Lh,Ll);	c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='B'&&IRl=='6')//ora_m
	 {a=hex8toint(Ah,Al);b=hex8toint(mem[addr1][0],mem[addr1][1]);c=a|b;
	 //////////
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	 //////////
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];}
	PC+=1;
	return;
	}
//*************************************************************
//this is a 2 byte instruction.
ori_8bit()
	{ int a,b,c;
	a=hex8toint(Ah,Al);b=hex8toint(mem[PC+1][0],mem[PC+1][1]);c=a|b;
	/////////
	if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;} CYFLAG=0;
	//////////
	inttohex8(c);Ah=hex8[0];Al=hex8[1];
	PC+=2;
	return;
	}
//************************************************************
//this is a 2 byte instruction.
out_8bit(){PC+=2;return;}//not complete
//***********************************************************
//this is a 1 byte instruction.
pchl()
	{PCHh=Hh;PCHl=Hl;PCLh=Lh;PCLl=Ll;
	PC=hex16toint(Hh,Hl,Lh,Ll);
	//NOT PC+=1;
	return;}
//***********************************************************
//this is a 1 byte instruction.
pop_x()
	{
	if(IRh=='C'&&IRl=='1')//pop b;
		{Ch=stack[SP][0];Cl=stack[SP][1];SP++;
		 Bh=stack[SP][0];Bl=stack[SP][1];SP++; }
	else if(IRh=='D'&&IRl=='1')//pop d;
		{Eh=stack[SP][0];El=stack[SP][1];SP++;
		 Dh=stack[SP][0];Dl=stack[SP][1];SP++; }
	else if(IRh=='E'&&IRl=='1')//pop h;
		{Lh=stack[SP][0];Ll=stack[SP][1];SP++;
		 Hh=stack[SP][0];Hl=stack[SP][1];SP++; }
	else if(IRh=='F'&&IRl=='1')//pop psw;
		{Fh=stack[SP][0];Fl=stack[SP][1];SP++;
		 Ah=stack[SP][0];Al=stack[SP][1];SP++; }
	FhFltoflags();
	PC+=1;
	return;}
//****************************************************************
//this is a 1 byte instruction.
push_x()
	{
	     flagstoFhFl();
	     if(IRh=='C'&&IRl=='5')//push b;
	     {SP--;stack[SP][0]=Bh;stack[SP][1]=Bl;
	      SP--;stack[SP][0]=Ch;stack[SP][1]=Cl;}
	else if(IRh=='D'&&IRl=='5')//push d;
	     {SP--;stack[SP][0]=Dh;stack[SP][1]=Dl;
	      SP--;stack[SP][0]=Eh;stack[SP][1]=El;}
	else if(IRh=='E'&&IRl=='5')//push h;
	     {SP--;stack[SP][0]=Hh;stack[SP][1]=Hl;
	      SP--;stack[SP][0]=Lh;stack[SP][1]=Ll;}
	else if(IRh=='F'&&IRl=='5')//push psw;
	     {SP--;stack[SP][0]=Ah;stack[SP][1]=Al;
	      SP--;stack[SP][0]=Fh;stack[SP][1]=Fl;}
	PC+=1;
	return;
	}
//*******************************************************************
//this is a 1 byte instruction.
ret()
	{
	 if(IRh=='C'&&IRl=='9')//ret
		{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
		 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
		 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
		}
	else if(IRh=='D'&&IRl=='8')//rc
		{if(CYFLAG==1)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}
		}
	else if(IRh=='D'&&IRl=='0')//rnc
		{if(CYFLAG==0)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='F'&&IRl=='0')//rp
		{if(SFLAG==0)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='F'&&IRl=='8')//rm
		{if(SFLAG==1)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='E'&&IRl=='8')//rpe
		{if(PFLAG==1)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='E'&&IRl=='0')//rpo
		{if(PFLAG==0)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='C'&&IRl=='8')//rz
		{if(ZFLAG==1)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	else if(IRh=='C'&&IRl=='0')//rnz
		{if(ZFLAG==0)
			{PCLl=stack[SP][0];PCLl=stack[SP][1];SP++;
			 PCHh=stack[SP][0];PCHl=stack[SP][1];SP++;
			 PC=hex16toint(PCHh,PCHl,PCLh,PCLl);return;
			}
		 else{PC++;return;}}
	return;
	}
//***************************************************************
//this is a 1 byte instruction.
rim()
	{PC+=1;return;}       //not complete;
//****************************************************************
//this is a 1 byte instruction.
rotate()
	{int a,i,d1[8];
	a=hex8toint(Ah,Al);
	inttobits(a);
	for(i=0;i<=7;i++){d1[i]=d[i];}
	if(IRh=='0'&&IRl=='7')//rlc;  cyflag=d[7]
		{d[0]=d1[7];d[1]=d1[0];d[2]=d1[1];d[3]=d1[2];
		 d[4]=d1[3];d[5]=d1[4];d[6]=d1[5];d[7]=d1[6];CYFLAG=d1[7];}
	else if(IRh=='0'&&IRl=='F')//rrc; cyflag=d[0]
		{d[0]=d1[1];d[1]=d1[2];d[2]=d1[3];d[3]=d1[4];
		 d[4]=d1[5];d[5]=d1[6];d[6]=d1[7];d[7]=d1[0];CYFLAG=d1[0];}
	else if(IRh=='1'&&IRl=='7')//ral;  through carry;
		{d[0]=CYFLAG;d[1]=d1[0];d[2]=d1[1];d[3]=d1[2];
		 d[4]=d1[3];d[5]=d1[4];d[6]=d1[5];d[7]=d1[6];CYFLAG=d1[7];}
	else if(IRh=='1'&&IRl=='F')//rar;  through carry;
		{d[0]=d1[1];d[1]=d1[2];d[2]=d1[3];d[3]=d1[4];
		 d[4]=d1[5];d[5]=d1[6];d[6]=d1[7];CYFLAG=d1[0];d[7]=CYFLAG;}
	PC+=1;
	return;
	}
//**********************************************************
//this is a 1 byte instruction.
rst()        //  not complete
	{
	  if(IRh=='C'&&IRl=='7'){}//rst0
	else if(IRh=='C'&&IRl=='F'){}//rst1
	else if(IRh=='D'&&IRl=='7'){}//rst2
	else if(IRh=='D'&&IRl=='F'){}//rst3
	else if(IRh=='E'&&IRl=='7'){}//rst4
	else if(IRh=='E'&&IRl=='F'){}//rst5
	else if(IRh=='F'&&IRl=='7'){}//rst6
	else if(IRh=='F'&&IRl=='F'){}//rst7
	PC+=1;
	return;
	}
//***********************************************************
//this is a 1 byte instruction.
sbb_x()
	{int a,b,c,addr1;
	if(IRh=='9' && IRl=='F') //sbb_a
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ah,Al);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='8') //sbb_b
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Bh,Bl);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='9') //sbb_c
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ch,Cl);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='A') //sbb_d
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Dh,Dl);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='B') //sbb_e
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Eh,El);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='C') //sbb_h
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Hh,Hl);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }

	if(IRh=='9' && IRl=='D') //sbb_l
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Lh,Ll);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='E') //sbb_m
	   {
	      addr1=hex16toint(Hh,Hl,Lh,Ll);
	      a=hex8toint(Ah,Al);
	      b=hex8toint(mem[addr1][0],mem[addr1][1]);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	      inttohex8(c);
	      Ah=hex8[0];Al=hex8[1];
	   }
	PC+=1;return;
	}
//*************************************************************
//this is a 2 byte instruction.
sbi_8bit()
	{int a,b,c;
	 a=hex8toint(Ah,Al);
	 b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	     c=a+(~(b+CYFLAG)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];
	 PC+=2;
	 return ;
	}
//***************************************************************
//this is a 3 byte instruction.
shld_16bit()
	{     int addr1;
	      addr1=hex16toint(mem[PC+2][0],mem[PC+2][1],mem[PC+1][0],mem[PC+1][1]) ;
	      mem[addr1][0]=Lh;
	      mem[addr1][1]=Ll ;
	      mem[addr1+1][0]=Hh;
	      mem[addr1+1][1]=Hl;
	      PC+=3;return;
	}
//*************************************************************
//this is a 1 byte instruction.
sim()	{PC+=1;return;}// not complete
//*************************************************************
//this is a 1 byte instruction.
sphl()
	{     SPHh=Hh;SPHl=Hl;SPLh=Lh;SPLh=Ll;
	      PC+=1;return;
	}
//**************************************************************
//this is a 3 byte instruction.
sta_16bit()
	{int addr1;
	char a1,b1,c1,d1;
	a1=mem[PC+2][0];b1=mem[PC+2][1];
	c1=mem[PC+1][0];d1=mem[PC+1][1];
	addr1=hex16toint(a1,b1,c1,d1);
	mem[addr1][0]=Ah;mem[addr1][1]=Al;
	PC+=3;
	return;
	}
//*************************************************************
//this is a 1 byte instruction.
stax_x()
	{     int  addr1;
	      if(IRh=='0' && IRl=='2') //stax_b
		 {
		   addr1=hex16toint(Bh,Bl,Ch,Cl) ;
		   mem[addr1][0]=Ah;
		   mem[addr1][1]=Al;
		 }
	      if(IRh=='1' && IRl=='2') //stax_d
		 {
		   addr1=hex16toint(Dh,Dl,Eh,El) ;
		   mem[addr1][0]=Ah;
		   mem[addr1][1]=Al;
		 }
	PC+=1;return;
	}
//****************************************************************
//this is a 1 byte instruction.
stc()
	{  CYFLAG=1;
	   PC+=1;return;
	}
//****************************************************************
//this is a 1 byte instruction.
sub_x()
	{int a,b,c,addr1;
	if(IRh=='9' && IRl=='7') //sub_a
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ah,Al);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='0') //sub_b
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Bh,Bl);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }

	if(IRh=='9' && IRl=='1') //sub_c
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Ch,Cl);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='2') //sub_d
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Dh,Dl);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='3') //sub_e
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Eh,El);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='4') //sub_h
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Hh,Hl);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }

	if(IRh=='9' && IRl=='5') //sub_l
	   { a=hex8toint(Ah,Al);
	     b=hex8toint(Lh,Ll);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	     inttohex8(c);
	     Ah=hex8[0];Al=hex8[1];
	   }
	if(IRh=='9' && IRl=='6') //sub_m
	   {
	      addr1=hex16toint(Hh,Hl,Lh,Ll);
	      a=hex8toint(Ah,Al);
	      b=hex8toint(mem[addr1][0],mem[addr1][1]);
	     c=a+(~(b)+1);
	     if(c>255) {CYFLAG=1;} else CYFLAG=0;
	     if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	     if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	      inttohex8(c);
	      Ah=hex8[0];Al=hex8[1];
	   }
	PC+=1;return;
	}
//*************************************************************
//this is a 2 byte instruction.
sui_8bit()
	{int a,b,c;
	 a=hex8toint(Ah,Al);
	 b=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	 c=a+(~(b)+1);
	 if(c>255) {CYFLAG=1;} else CYFLAG=0;
	 if(CYFLAG==1)CYFLAG=0;else CYFLAG=1;
	 if(c>127) {SFLAG=1;} if(c==0) {ZFLAG=1;}
	 inttohex8(c);Ah=hex8[0];Al=hex8[1];
	 PC+=2;
	 return;
	}
//***************************************************************
//this is a 1 byte instruction.
xchg()
	{     char ch1,ch2;
	      ch1=Dh;ch2=Dl;
	      Dh=Hh;Dl=Hl;
	      Hh=ch1;Hl=ch2;
	PC+=1;return;
	}
//****************************************************************
//this is a 1 byte instruction.
xra_x()
	{
	    int a,d1[8],i,addr1;
	     addr1=hex16toint(Hh,Hl,Lh,Ll);
	     a=hex8toint(Ah,Al);
	     inttobits(a);for(i=0;i<=7;i++){d1[i]=d[i];}
	     if(IRh=='A'&&IRl=='F')//xra a;
		{
		   a=hex8toint(Ah,Al);
		   inttobits(a);
      //computes XOR of d[] & d1[]  and stores result in d[] ;
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		    //////////
		    if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		    //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];
		}
	else if(IRh=='A'&&IRl=='8')//xra b;
		{a=hex8toint(Bh,Bl);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='9')//xra c;
		{a=hex8toint(Ch,Cl);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='A')//xra d;
		{a=hex8toint(Dh,Dl);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='B')//xra e;
		{a=hex8toint(Eh,El);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='C')//xra h;
		{a=hex8toint(Hh,Hl);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='D')//xra l;
		{a=hex8toint(Lh,Ll);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	else if(IRh=='A'&&IRl=='E')//xra m;
		{a=hex8toint(mem[addr1][0],mem[addr1][1]);
		   inttobits(a);
		   for(i=0;i<=7;i++)
			{
			      if(d1[i]==d[i])d[i]=0;
			      else d[i]=d[i]+d1[i];
			}
		   a=bitstoint();
		   //////////
		   if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
		   //////////
		   inttohex8(a);
		   Ah=hex8[0];Al=hex8[1];}
	PC+=1;return;
	}
//*****************************************************************
//this is a 2 byte instruction.
xri_8bit()
	{
	    int a,d1[8],i;
	     a=hex8toint(Ah,Al);
	     inttobits(a);for(i=0;i<=7;i++){d1[i]=d[i];}
	     a=hex8toint(mem[PC+1][0],mem[PC+1][1]);
	     inttobits(a);
      //computes XOR of d[] & d1[]  and stores result in d[] ;
	     for(i=0;i<=7;i++)
		{
		      if(d1[i]==d[i])d[i]=0;
		      else d[i]=d[i]+d1[i];
		}
	     a=bitstoint();
	     //////////
	     if(a>127) {SFLAG=1;} if(a==0) {ZFLAG=1;} CYFLAG=0;
	     //////////
	     inttohex8(a);
	     Ah=hex8[0];Al=hex8[1];
	PC+=2;return;
	}
//****************************************************************
//this is a 1 byte instruction.
xthl()
	{    char ch1,ch2,ch3,ch4;
	      ch1=Hh;ch2=Hl;ch3=Lh;ch4=Ll;
	      Lh=stack[SP][0];Ll=stack[SP][1];
	      Hh=stack[SP+1][0];Hl=stack[SP+1][1];
	      stack[SP][0]=ch3;   stack[SP][1]=ch4;
	      stack[SP+1][0]=ch1; stack[SP+1][1]=ch2;
	PC+=1;return;
	}
//****************************************************************
