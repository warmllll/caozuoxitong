#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BlockSize  512
#define DirSize 32
#define RootSize 2

struct DirBlock
{
    char filename[10];  /*文件名限长10个字符*/
    char fillchar[DirSize-4*sizeof(int)-sizeof(long int)-11];/*填充字节*/
    long filelen;       /*文件长度*/
    int year,month,day;  /*日期*/
    int firstblockaddr;  /*文件首块扇区号*/
};

struct FCBBlock
{
    int fileid;         /*文件标识*/
    struct DirBlock fileinfo; /*目录信息*/
    long filepos;             /*文件读写指针*/
    int fdtblockaddr;         /*目录项所在块号*/
    int fdtblockindex;        /*目录项所在块内序号*/
    struct FCBBlock *next;/*指向下一个文件控制块的指针*/
};

struct ReserveBlock
{
    int sysblocknum;/*文件系统总扇区数*/
    int resblocknum;/*保留扇区扇区数*/
    int fatblocknum;/*FAT表扇区数*/
    int rootblocknum;/*根目录区扇区数*/
    char fillchar[BlockSize-4*sizeof(int)];/*填充字节*/
};

struct ReserveBlock sys1;
struct FCBBlock *fcb;
struct DirBlock fil[32],*dir;/*目录项*/
int *fat1;
char *str,*ptr;
char fillchar[BlockSize];
FILE *fp;


FILE *OPENSYS(char *filename)
{
    int i;
    fp=fopen(filename,"rb+");
    fread(&sys1,1,BlockSize,fp);
    fat1=(int *)malloc(sys1.sysblocknum);
    for(i=0;i<sys1.fatblocknum;i++)
        fread(fat1,sizeof(int)*sys1.sysblocknum,1,fp);/*把基本的文件系统都读进来*/
        fseek(fp,(sys1.fatblocknum+sys1.resblocknum)*BlockSize,0);/*修改文件指针的位置*/
    dir=fil;/*目录指针*/
    fread(dir,DirSize*32,1,fp);

    return fp;
}

int CLOSESYS(FILE *stream)
{
    int i;
    fseek(stream,sys1.resblocknum*BlockSize,0);
    fwrite(fat1,sizeof(int)*sys1.sysblocknum,1,stream);
    fseek(fp,(sys1.fatblocknum+sys1.resblocknum)*BlockSize,0);
    fwrite(dir,DirSize*32,1,fp);
    fclose(fp);
    return 1;
}

int FCREATE(char *filename)/*建立文件*/
{
    int i,flag=0,j,k=0,flag1=0,flag2=0,a;
    int n,m;
    while(1)
    {
        a=strlen(filename);/*文件名长度*/
        if(a>10)
        {
            printf("文件名过长!\n");
            printf("请重新输入:\n");
            scanf("%s",&filename);
        }
        else
            break;
    }
    while(1)
    {
        for(i=0;i<32;i++)
        {
            if(strcmp(filename,fil[i].filename)==0)
            {
                printf("该文件名已存在!\n");
                printf("请重新输入:\n");
                scanf("%s",&filename);
                flag1=1;
                break;
            }
        }
        if(flag1==0)
            break;
        scanf("%s",&filename);
        flag1=0;
    }
    for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)
    {
           if(fat1[i]==0)
            flag++;/*统计磁盘上为空数目*/
    }
    if(flag==0)
    {
        printf("磁盘已满!\n");/*统计结果为0,则磁盘已满*/
        return 0;
    }
    printf("空闲块数:%d\n",flag);
    printf("请输入文件长度:\n");
    for(j=0;j<32;j++)
        if(fil[j].firstblockaddr==0)
            break;
    while(1)
    {
        scanf("%d",&dir[j].filelen);/*输入目录项中文件的长度*/
        n=(dir[j].filelen/BlockSize)+(dir[j].filelen%BlockSize?1:0);
        if(n<0||n>flag)/*文件长度小于0或大于空闲的空间*/
        {
            printf("文件长度过长\!n");
            printf("请重新输入:\n");
        }
        else
            break;
    }
    for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)/*文件内容区域*/
    {

            if(fat1[i]==0){
                k++;
                if(flag2==0)
                {
                    dir[j].firstblockaddr=i+1;
                    flag2=1;
                }
                else
                {
                    if(k>=n)/*n 文件长度*/
                    {
                        fat1[m]=i+1;
                        fat1[i]=-1;
                        break;
                    }
                    else
                        fat1[m]=i+1;
                }
                m=i;
            }
    }
    strcpy(dir[j].filename,filename);
    dir[j].filename[a]='\0';
    printf("请输入年份:\n");
    scanf("%d",&dir[j].year);
    printf("请输入月份:\n");
    scanf("%d",&dir[j].month);
    printf("请输入日期:\n");
    scanf("%d",&dir[j].day);
    return (1);

}

int FRENAME(char *filename)/*重命名*/
{
    int i,j=0,k,flag=0,a,b;
    struct FCBBlock *p,*q,*r;
    p=fcb;
    char newfilename[10];
    for(i=0;i<32;i++)
    {
        if(strcmp(filename,fil[i].filename)==0)/*找到要重命名的文件*/
        {
            flag=1;
            b=i;
            break;
        }
    }
    if(flag==0)
    {
        printf("文件不存在!\n");
        return 0;
    }
    while(p)
    {
        j++;
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)
        {
            printf("文件已打开，请先关闭文件!\n");
            return 0;
        }
        q=p;
        p=p->next;
    }
    printf("请输入更改的文件名:\n");
    scanf("%s",&newfilename);
    for(i=0;i<32;i++)
    {
        if(strcmp(newfilename,fil[i].filename)==0)
        {
            printf("该文件名已存在!\n");
            printf("请重新输入:\n");
            scanf("%s",&newfilename);
            flag=1;
            break;
        }
    }
    if(flag==0)
        scanf("%s",&filename);
    while(1)
    {
        a=strlen(newfilename);/*文件名长度*/
        if(a>10)
        {
            printf("文件名过长!\n");
            printf("请重新输入:\n");
            scanf("%s",&newfilename);
        }
        else
            break;
    }
    strcpy(dir[b].filename,newfilename);
    return 1;
}

void LISTDIR(void)/*显示目录*/
{
    int i,flag=0;
    for(i=0;i<32;i++)
    {
        if(fil[i].firstblockaddr!=0)
        {
            if(flag==0)
                printf("文件名     大小    创建日期\n");
                flag=1;
                printf("%s         %ld     %d/%d/%d\n",fil[i].filename,fil[i].filelen,fil[i].year,fil[i].month,fil[i].day);
        }
    }
}

int FDELETE(char *filename)/*删除文件*/
{
    int i,j,k,n,flag=1;
    struct FCBBlock *p;/*文件控制块指针*/
    p=fcb;
    while(p)
    {
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)/*目录项所在内序号*/
        {
            return 1;
        }
        else
            p=p->next;
    }
    for(i=0;i<sys1.rootblocknum*BlockSize/DirSize;i++)/*查找要关闭的文件*/
        if(strcmp(filename,fil[i].filename)==0)
        {
            flag=1;
            break;
        }
        if(flag==0)
        {
            return 2;
        }
        j=fil[i].firstblockaddr;
        while(1)
        {
            k=fat1[j-1];
            fseek(fp,(j-1)*BlockSize,0);/*将指针指向文件的第一个扇区的位置*/
            fwrite(&fillchar,1,BlockSize,fp);
            fat1[j-1]=0;
            if(k==-1)
                break;
            else
                j=k;
        }
        memset(&fil[i],0,DirSize);
        return 0;

}

int FOPEN(char *filename)/*打开文件*/
{
    int i,j=0,k,flag=0;
    struct FCBBlock *p,*q,*r;
    p=fcb;
    for(i=0;i<32;i++)
    {
        if(strcmp(filename,fil[i].filename)==0)/*找到要打开的文件*/
        {
            flag=1;
            break;
        }
    }
    if(flag==0)
    {
        printf("文件不存在!\n");
        return 0;
    }
    while(p)
    {
        j++;
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)
        {
            printf("文件已打开!\n");
            return 0;
        }
        q=p;
        p=p->next;
    }
    if(fcb==NULL)
    {
        fcb=(struct FCBBlock *)malloc(sizeof(struct FCBBlock));
        fcb->fileid=j+1;/*文件标示*/
        fcb->filepos=0;/*文件读写指针*/
        fcb->next=NULL;/*指向下一个文件控制指针*/
        fcb->fdtblockindex=i;
    }
    else
    {
        r=(struct FCBBlock *)malloc(sizeof(struct FCBBlock));
        r->fileid=j+1;
        r->filepos=0;
        r->next=NULL;
        r->fdtblockindex=i;
        q->next=r;
    }
    return 1;
}

long FGETLEN(int fileid)/*获取文件指针*/
{
    struct FCBBlock *p;
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)/*寻找要找的指针*/
            return(p->filepos);
            p=p->next;
    }
    printf("error!\n");
    return(0);
}

int FCLOSE(char *filename)/*关闭文件*/
{
    int flag=0;
    struct FCBBlock *f,*p;
    f=fcb;
    if(strcmp(filename,fil[fcb->fdtblockindex].filename)==0)
    {
        flag=1;
        fcb=fcb->next;
        p=fcb;
        while(p)
        {
            p->fileid-=1;
            p=p->next;
        }
        return flag;
    }
    p=f;
    f=f->next;
    while(f)
    {
        if(strcmp(filename,fil[f->fdtblockindex].filename)==0)
        {
            flag=1;
            p=f->next;
            while(p)
            {
                p=p->next;
                p->fileid-=1;
            }
            free(f);
            return flag;
        }
    }
    if(flag==0)
    {
        printf("文件未打开!\n");
        return flag;
    }
    return 0;
}

int FREAD(char *str, int n, int fileid)    /*读文件*/
{
    int m,i,j=0,k,l,len,n1,n2=0;
    struct FCBBlock *p;
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)
            break;
        else
            p=p->next;
    }
    if(p==NULL)
        return 1;
    len=fil[p->fdtblockindex].filelen;
    l=p->filepos;
    if((l+n)>len)
        return 2;
    j=(l/BlockSize)+(l%BlockSize?1:0);
    m=((l+n)/BlockSize)+((l+n)%BlockSize?1:0);
    k=fil[p->fdtblockindex].firstblockaddr;
    for(i=1;i<j;i++)

        k=fat1[k-1];
    str=(char *)malloc((m-j+1)*BlockSize+1);  /*分配读取的缓冲区*/
    for(i=0;i<(m-j+1);i++)   /*读文件*/
    {
        fseek(fp,(k-1)*BlockSize,0);
        fread(&str[i*BlockSize],BlockSize,1,fp);
        k=fat1[k-1];
    }
    str[n+l-(j-1)*BlockSize]='\0'; /*截断多读的内容*/
    if(l!=0)
        str=&str[l-(j-1)*BlockSize];
    p->filepos=n+l;    /*修改文件指针*/
    printf("%s\n",str);
    return 0;


}


int FWRITE(char *ptr, int n, int fileid)    /*文件写*/
{
    long l;
    int m,i=0,j,k=0,num,flag=0,flag1=0,b,flag2=0;
    int n1,n2=0,n3,c;
    struct FCBBlock *p;
    char *st1,*st2;
    st1=(char *)malloc(BlockSize);
    p=fcb;
    while(p)   /*查找文件的fcb*/
    {
        if(p->fileid==fileid)
            break;
        else
            p=p->next;
    }
    if(p==NULL)
        return 1;
    l=p->filepos;
    n1=fil[p->fdtblockindex].firstblockaddr;
    while(n1!=-1)
    {
        n2++;
        m=n1-1;
        n1=fat1[n1-1];
    }
    if((l+n)>n2*BlockSize)
    {
        n3=(l+n)/BlockSize+((l+n)%BlockSize?1:0)-n2; /*追加空间*/
        for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)

            if(fat1[i]==0)
                flag++;
            if(flag==0)
            {
                //printf("磁盘已满");
                return 2;
            }
            if((l+n-fil[p->fdtblockindex].filelen)>flag*BlockSize)
                return 3;

        for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)
        {
            if(fat1[i]==0)
            {
                k++;
                if(k>=n3)
                {
                    fat1[m]=i+1;
                    fat1[i]=-1;
                    break;
                }
                else
                    fat1[m]=i+1;
                m=i;
            }
        }
    }
    flag=0;
    k=0;
    n3=(l+n)/BlockSize+((l+n)%BlockSize?1:0);
    j=(l/BlockSize)+(l%BlockSize?1:0);
    st2=(char *)malloc((n3-j+1)*BlockSize);
    if(l==0)
        j=1;
    num=fil[p->fdtblockindex].firstblockaddr;
    for(i=1;i<j;i++)
        num=fat1[num-1];
        if(!l%BlockSize)
        {
            fseek(fp,(num-1)*BlockSize+l-(i-1)*BlockSize,0);
            fread(st2,BlockSize,1,fp);
            for(c=0;c<n;c++)
                st2[c]=ptr[c];
            fseek(fp,(num-1)*BlockSize+l-(i-1)*BlockSize,0);
            fwrite(st2,BlockSize,1,fp);
            num=fat1[num-1];
            fflush(fp);
        }
        else
        {
            fseek(fp,(num-1)*BlockSize,0);
            fread(st2,BlockSize,1,fp);
            for(c=0;c<n;c++)
                st2[c+l-(j-1)*BlockSize]=ptr[c];
            fseek(fp,(num-1)*BlockSize,0);
            fwrite(st2,BlockSize,1,fp);
            num=fat1[num-1];
            fflush(fp);
        }
    for(i=1;i<(n3-j);i++)   /*写第2块-倒数第2块*/
    {
        fseek(fp,(num-1)*BlockSize,0);
        fwrite(st2+i*BlockSize,BlockSize,1,fp);
        num=fat1[num-1];
        fflush(fp);
    }

    if((n3-j)>0)   /*写最后一块*/
    {
        fseek(fp,(num-1)*BlockSize,0);
        fread(st2+i*BlockSize,BlockSize,1,fp);
        for(c=0;c<(n-((j*BlockSize-l)+(i-1)*BlockSize));c++)
            st2[c+i*BlockSize]=ptr[c+(j*BlockSize-l)+(i-1)*BlockSize];
        fseek(fp,(num-1)*BlockSize,0);
        fwrite(st2+i*BlockSize,BlockSize,1,fp);
        fflush(fp);
    }
    p->filepos=l+n;   /*修改文件指针*/
    if((n+l)>fil[p->fdtblockindex].filelen)
        fil[p->fdtblockindex].filelen=n+l;    /*修改文件长度*/
    return 0;
}

int FEOF(int fileid)
{
    int flag;
    struct FCBBlock *p;
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)
            break;
    }
    if(p->filepos>fil[p->fdtblockindex].filelen)
        return 0;
    else
        return 1;
}

long FGETPOS(int fileid)
{
    struct FCBBlock *p;\
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)
            return (p->filepos);
        else
            p=p->next;
    }
    printf("error!\n");
    return(0);
}

int FSETPOS(int fileid, long offset)
{
    struct FCBBlock *p;
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)
        {
            while(offset>fil[p->fdtblockindex].filelen||offset<0)
            {
                printf("set error!zhe pos >file length\n");
                printf("input file pos ,again:");
                scanf("%d",&offset);
            }
            p->filepos=offset;
            return 1;
        }
        else
            p=p->next;
     }
     printf("error!\n");
     return(0);
}

void DISPLAY()
{
 	printf("******************************************************\n");
    printf("*建立文件:creat                                      *\n");
    printf("*显示目录:list                                       *\n");
    printf("*重命名文件:rename                                   *\n");
    printf("*删除文件:del                                        *\n");
    printf("*打开文件:open                                       *\n");
    printf("*关闭文件:close                                      *\n");
    printf("*文件块读:read                                       *\n");
    printf("*文件块写:write                                      *\n");
    printf("*退出:exit                                           *\n");
    printf("*输入命令:                                           *\n");
   	printf("******************************************************\n");
   }

void test(char *filename)
{
    char *t,cmd[10],fname[12];
    struct FCBBlock *p;
    int i,j,n,flag,len,id,flag1=0,l;
    fp=OPENSYS(filename);
    while(1)
    {
        DISPLAY();
        scanf("%s",cmd);
        if(!strcmp(cmd,"creat"))
        {
            printf("请输入文件名：\n");
            scanf("%s",&fname);
            flag=FCREATE(fname);
            if(flag==1)
                printf("文件%s 创建成功\n",fname);
        }
        else
            if(!strcmp(cmd,"list"))
                LISTDIR();
        else
            if(!strcmp(cmd,"rename"))
            {
                scanf("%s",&fname);
                FRENAME(fname);
                if(flag==1)
                    printf("文件重命名成功\n");
            }

        else
            if(!strcmp(cmd,"del"))
            {
                scanf("%s",fname);
                flag=FDELETE(fname);
                if(flag==0)
                    printf("文件%s 删除成功\n",fname);
                if(flag==1)
                    printf("文件%s被打开，删除前请关闭文件!\n",fname);
                if(flag==2)
                    printf("文件不存在!\n");
            }
            else
                if(!strcmp(cmd,"open"))
                {
                    scanf("%s",&fname);
                    flag=FOPEN(fname);
                    if(flag==1)
                        printf("文件%s 打开\n",fname);
                }
                else
                    if(!strcmp(cmd,"close"))
                    {
                        scanf("%s",&fname);
                        flag=FCLOSE(fname);
                        if(flag==1)
                            printf("文件%s 关闭\n",fname);
                    }
                     else
                        if(!strcmp(cmd,"read"))
                        {
                            scanf("%s",fname);
                            p=fcb;
                            printf("请输入要读的文件大小:\n");
                            scanf("%d",&len);
                            id=p->fileid;
                            flag=FREAD(str,len,id);
                            if(flag1==1)
                                printf("文件%s 未打开，请先打开文件\n",fname);
                            if(flag1==2)
                                printf("读的文件太大了！\n");
                        }
                        else
                            if(!strcmp(cmd,"write"))
                            {
                                scanf("%s",fname);
                                p=fcb;
                                id=p->fileid;
                                printf("请输入文件大小:\n");
                                scanf("%d",&len);
                                ptr=(char *)malloc(len+1);
                                printf("请输入写入的内容：\n");
                                scanf("%s",ptr);
                                flag=FWRITE(ptr,len,id);
                                if(flag==1)
                                    printf("文件%s 未打开，请先打开文件!\n",fname);
                                if(flag==2)
                                    printf("空间不足!\n");
                                if(flag==3)
                                    printf("写入文件太大!\n");
                                if(flag==0)
                                    printf("写入成功!\n");
                            }
                            else
                                if(!strcmp(cmd,"exit"))
                                {
                                    flag=CLOSESYS(fp);
                                        break;
                                }
                                else
                                    printf("error!\n");
    }
}
int main(void)
{
    struct ReserveBlock sys;
    char filename[]="fatsys.dat";
    int *fat;
    int i,j;
    fcb=NULL;
    memset(&sys,255,BlockSize);
    printf("请输入创建的文件系统的大小:\n");
    scanf("%d",&(sys.sysblocknum));
    sys.resblocknum=1;
    sys.fatblocknum=sys.sysblocknum/(BlockSize/sizeof(int))+((sys.sysblocknum%(BlockSize/sizeof(int)))?1:0);
    fat=(int*)malloc(BlockSize*sys.fatblocknum);
    memset(fat,255,BlockSize*sys.fatblocknum);
    memset(fat,0,sizeof(int)*sys.sysblocknum);
    sys.rootblocknum=RootSize;
    j=sys.resblocknum+sys.fatblocknum+sys.rootblocknum;
    for (i=0;i<j;i++)
        fat[i]=-1;
    memset(&fillchar,0,BlockSize);
    fp=fopen(filename,"w+b");
    fwrite(&sys,1,BlockSize,fp);
    for (i=0;i<sys.fatblocknum;i++)
        fwrite(fat+i*BlockSize/sizeof(int),1,BlockSize,fp);
    j=sys.resblocknum+sys.fatblocknum;
    for(i=0;i<(sys.sysblocknum-j);i++)
        fwrite(&fillchar,1,BlockSize,fp);
    fclose(fp);
    free(fat);
    test(filename);
    return 0;
}

/*
{

		int cmd;
		char filename[FILE_NAME_LENGTH];
		//初始化
		cmd = -1;
		Init_block(filename, FILE_NAME_LENGTH);
		if (strncmp("dir", ch, 3) == 0)			//浏览目录dir(non)
		{
			cmd = DIR;
		}
		if (strncmp("creat", ch, 6) == 0)			//创建文件命令create(filename)
		{
			cmd = CREATE;
			strcat(filename, ch + 7);
		}
		if (strncmp("del", ch, 6) == 0)			//删除文件命令delete(filename)
		{
			cmd = DELETE;
			strcat(filename, ch + 7);
		}
		if (strncmp("open", ch, 4) == 0)			//打开文件命令open(filename)
		{
			cmd = OPEN;
			strcat(filename, ch + 5);
		}
		if (strncmp("close", ch, 5) == 0)			//关闭文件命令close(index)
		{
			cmd = CLOSE;
		}
		if (strncmp("read", ch, 4) == 0)			//读文件命令read(index)
		{
			cmd = READ;
		}
		if (strncmp("write", ch, 5) == 0)			//写文件命令write(index)
		{
			cmd = WRITE;
		}


		}
		if (strncmp("oplist", ch, 6) == 0)			// 查看打开文件表
		{
			cmd = OPLIST;
		}
		if (strncmp("exit", ch, 4) == 0)			// 退出命令exit
		{
			cmd = EXIT;
			break;
		}

		{
			cmd = HELP;
		}
		int index, count, pos;
		switch (cmd)
		{
		case DIR:
			directory();
			printf("----------------------------------------------\n");
			break;
		case CREATE:
			if (create(filename) == OK)
				printf("创建文件成功|\n");
			printf("----------------------------------------------\n");
			break;
		case DELETE:
			if (destroy(filename) == OK)
				printf("删除文件成功|\n");
			printf("----------------------------------------------\n");
			break;
		case OPEN:
			if (open(filename) == OK)
				printf("打开文件成功|\n");
			printf("----------------------------------------------\n");
			break;
		case CLOSE:
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("请输入要关闭文件的索引号：\n");
			scanf("%d", &index);
			if (close(index) == OK)
				printf("关闭操作成功\n");
			printf("----------------------------------------------\n");
			break;
		case READ:
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开a\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("请输入读取文件的索引号：\n");
			scanf("%d", &index);
			printf("请输入想要读取文件长度：\n");
			scanf("%d", &count);
			if (read(index, 0, count) == OK)
				printf("读文件操作成功\n");
			printf("----------------------------------------------\n");
			break;
		case WRITE:
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("请输入要写入文件的索引号：\n");
			scanf("%d", &index);
			printf("请输入想要写入文件长度：\n");
			scanf("%d", &count);
			if (write(index, 0, count) == OK)
				printf("写入操作成功\n");
			printf("----------------------------------------------\n");
			break;
		case LSEEK:
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("请输入要写入文件的索引号:\n");
			scanf("%d", &index);
			printf("请输入想要设置的文件相对位置\n");
			scanf("%d", &pos);
			lseek(index, pos);
			printf("----------------------------------------------\n");
			break;
		case OPLIST:
			if (show_openlist() == 0)
			{
				printf("\t\t\n当前没有文件被打开\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("----------------------------------------------\n");
			break;
		case HELP:
			show_help();
			break;
		case LDISK:
			show_ldisk();
			break;
		default:
			printf("指令错误\n");
			printf("----------------------------------------------\n");
			break;
		}
		fflush(stdin);
		Init_block(ch, CH_LENGTH);
	}

}*/ 
