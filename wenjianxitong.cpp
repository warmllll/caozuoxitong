#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BlockSize  512
#define DirSize 32
#define RootSize 2

struct DirBlock
{
    char filename[10];  /*�ļ����޳�10���ַ�*/
    char fillchar[DirSize-4*sizeof(int)-sizeof(long int)-11];/*����ֽ�*/
    long filelen;       /*�ļ�����*/
    int year,month,day;  /*����*/
    int firstblockaddr;  /*�ļ��׿�������*/
};

struct FCBBlock
{
    int fileid;         /*�ļ���ʶ*/
    struct DirBlock fileinfo; /*Ŀ¼��Ϣ*/
    long filepos;             /*�ļ���дָ��*/
    int fdtblockaddr;         /*Ŀ¼�����ڿ��*/
    int fdtblockindex;        /*Ŀ¼�����ڿ������*/
    struct FCBBlock *next;/*ָ����һ���ļ����ƿ��ָ��*/
};

struct ReserveBlock
{
    int sysblocknum;/*�ļ�ϵͳ��������*/
    int resblocknum;/*��������������*/
    int fatblocknum;/*FAT��������*/
    int rootblocknum;/*��Ŀ¼��������*/
    char fillchar[BlockSize-4*sizeof(int)];/*����ֽ�*/
};

struct ReserveBlock sys1;
struct FCBBlock *fcb;
struct DirBlock fil[32],*dir;/*Ŀ¼��*/
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
        fread(fat1,sizeof(int)*sys1.sysblocknum,1,fp);/*�ѻ������ļ�ϵͳ��������*/
        fseek(fp,(sys1.fatblocknum+sys1.resblocknum)*BlockSize,0);/*�޸��ļ�ָ���λ��*/
    dir=fil;/*Ŀ¼ָ��*/
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

int FCREATE(char *filename)/*�����ļ�*/
{
    int i,flag=0,j,k=0,flag1=0,flag2=0,a;
    int n,m;
    while(1)
    {
        a=strlen(filename);/*�ļ�������*/
        if(a>10)
        {
            printf("�ļ�������!\n");
            printf("����������:\n");
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
                printf("���ļ����Ѵ���!\n");
                printf("����������:\n");
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
            flag++;/*ͳ�ƴ�����Ϊ����Ŀ*/
    }
    if(flag==0)
    {
        printf("��������!\n");/*ͳ�ƽ��Ϊ0,���������*/
        return 0;
    }
    printf("���п���:%d\n",flag);
    printf("�������ļ�����:\n");
    for(j=0;j<32;j++)
        if(fil[j].firstblockaddr==0)
            break;
    while(1)
    {
        scanf("%d",&dir[j].filelen);/*����Ŀ¼�����ļ��ĳ���*/
        n=(dir[j].filelen/BlockSize)+(dir[j].filelen%BlockSize?1:0);
        if(n<0||n>flag)/*�ļ�����С��0����ڿ��еĿռ�*/
        {
            printf("�ļ����ȹ���\!n");
            printf("����������:\n");
        }
        else
            break;
    }
    for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)/*�ļ���������*/
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
                    if(k>=n)/*n �ļ�����*/
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
    printf("���������:\n");
    scanf("%d",&dir[j].year);
    printf("�������·�:\n");
    scanf("%d",&dir[j].month);
    printf("����������:\n");
    scanf("%d",&dir[j].day);
    return (1);

}

int FRENAME(char *filename)/*������*/
{
    int i,j=0,k,flag=0,a,b;
    struct FCBBlock *p,*q,*r;
    p=fcb;
    char newfilename[10];
    for(i=0;i<32;i++)
    {
        if(strcmp(filename,fil[i].filename)==0)/*�ҵ�Ҫ���������ļ�*/
        {
            flag=1;
            b=i;
            break;
        }
    }
    if(flag==0)
    {
        printf("�ļ�������!\n");
        return 0;
    }
    while(p)
    {
        j++;
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)
        {
            printf("�ļ��Ѵ򿪣����ȹر��ļ�!\n");
            return 0;
        }
        q=p;
        p=p->next;
    }
    printf("��������ĵ��ļ���:\n");
    scanf("%s",&newfilename);
    for(i=0;i<32;i++)
    {
        if(strcmp(newfilename,fil[i].filename)==0)
        {
            printf("���ļ����Ѵ���!\n");
            printf("����������:\n");
            scanf("%s",&newfilename);
            flag=1;
            break;
        }
    }
    if(flag==0)
        scanf("%s",&filename);
    while(1)
    {
        a=strlen(newfilename);/*�ļ�������*/
        if(a>10)
        {
            printf("�ļ�������!\n");
            printf("����������:\n");
            scanf("%s",&newfilename);
        }
        else
            break;
    }
    strcpy(dir[b].filename,newfilename);
    return 1;
}

void LISTDIR(void)/*��ʾĿ¼*/
{
    int i,flag=0;
    for(i=0;i<32;i++)
    {
        if(fil[i].firstblockaddr!=0)
        {
            if(flag==0)
                printf("�ļ���     ��С    ��������\n");
                flag=1;
                printf("%s         %ld     %d/%d/%d\n",fil[i].filename,fil[i].filelen,fil[i].year,fil[i].month,fil[i].day);
        }
    }
}

int FDELETE(char *filename)/*ɾ���ļ�*/
{
    int i,j,k,n,flag=1;
    struct FCBBlock *p;/*�ļ����ƿ�ָ��*/
    p=fcb;
    while(p)
    {
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)/*Ŀ¼�����������*/
        {
            return 1;
        }
        else
            p=p->next;
    }
    for(i=0;i<sys1.rootblocknum*BlockSize/DirSize;i++)/*����Ҫ�رյ��ļ�*/
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
            fseek(fp,(j-1)*BlockSize,0);/*��ָ��ָ���ļ��ĵ�һ��������λ��*/
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

int FOPEN(char *filename)/*���ļ�*/
{
    int i,j=0,k,flag=0;
    struct FCBBlock *p,*q,*r;
    p=fcb;
    for(i=0;i<32;i++)
    {
        if(strcmp(filename,fil[i].filename)==0)/*�ҵ�Ҫ�򿪵��ļ�*/
        {
            flag=1;
            break;
        }
    }
    if(flag==0)
    {
        printf("�ļ�������!\n");
        return 0;
    }
    while(p)
    {
        j++;
        if(strcmp(filename,fil[p->fdtblockindex].filename)==0)
        {
            printf("�ļ��Ѵ�!\n");
            return 0;
        }
        q=p;
        p=p->next;
    }
    if(fcb==NULL)
    {
        fcb=(struct FCBBlock *)malloc(sizeof(struct FCBBlock));
        fcb->fileid=j+1;/*�ļ���ʾ*/
        fcb->filepos=0;/*�ļ���дָ��*/
        fcb->next=NULL;/*ָ����һ���ļ�����ָ��*/
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

long FGETLEN(int fileid)/*��ȡ�ļ�ָ��*/
{
    struct FCBBlock *p;
    p=fcb;
    while(p)
    {
        if(p->fileid==fileid)/*Ѱ��Ҫ�ҵ�ָ��*/
            return(p->filepos);
            p=p->next;
    }
    printf("error!\n");
    return(0);
}

int FCLOSE(char *filename)/*�ر��ļ�*/
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
        printf("�ļ�δ��!\n");
        return flag;
    }
    return 0;
}

int FREAD(char *str, int n, int fileid)    /*���ļ�*/
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
    str=(char *)malloc((m-j+1)*BlockSize+1);  /*�����ȡ�Ļ�����*/
    for(i=0;i<(m-j+1);i++)   /*���ļ�*/
    {
        fseek(fp,(k-1)*BlockSize,0);
        fread(&str[i*BlockSize],BlockSize,1,fp);
        k=fat1[k-1];
    }
    str[n+l-(j-1)*BlockSize]='\0'; /*�ض϶��������*/
    if(l!=0)
        str=&str[l-(j-1)*BlockSize];
    p->filepos=n+l;    /*�޸��ļ�ָ��*/
    printf("%s\n",str);
    return 0;


}


int FWRITE(char *ptr, int n, int fileid)    /*�ļ�д*/
{
    long l;
    int m,i=0,j,k=0,num,flag=0,flag1=0,b,flag2=0;
    int n1,n2=0,n3,c;
    struct FCBBlock *p;
    char *st1,*st2;
    st1=(char *)malloc(BlockSize);
    p=fcb;
    while(p)   /*�����ļ���fcb*/
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
        n3=(l+n)/BlockSize+((l+n)%BlockSize?1:0)-n2; /*׷�ӿռ�*/
        for(i=(sys1.fatblocknum+sys1.resblocknum+sys1.rootblocknum);i<sys1.sysblocknum;i++)

            if(fat1[i]==0)
                flag++;
            if(flag==0)
            {
                //printf("��������");
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
    for(i=1;i<(n3-j);i++)   /*д��2��-������2��*/
    {
        fseek(fp,(num-1)*BlockSize,0);
        fwrite(st2+i*BlockSize,BlockSize,1,fp);
        num=fat1[num-1];
        fflush(fp);
    }

    if((n3-j)>0)   /*д���һ��*/
    {
        fseek(fp,(num-1)*BlockSize,0);
        fread(st2+i*BlockSize,BlockSize,1,fp);
        for(c=0;c<(n-((j*BlockSize-l)+(i-1)*BlockSize));c++)
            st2[c+i*BlockSize]=ptr[c+(j*BlockSize-l)+(i-1)*BlockSize];
        fseek(fp,(num-1)*BlockSize,0);
        fwrite(st2+i*BlockSize,BlockSize,1,fp);
        fflush(fp);
    }
    p->filepos=l+n;   /*�޸��ļ�ָ��*/
    if((n+l)>fil[p->fdtblockindex].filelen)
        fil[p->fdtblockindex].filelen=n+l;    /*�޸��ļ�����*/
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
    printf("*�����ļ�:creat                                      *\n");
    printf("*��ʾĿ¼:list                                       *\n");
    printf("*�������ļ�:rename                                   *\n");
    printf("*ɾ���ļ�:del                                        *\n");
    printf("*���ļ�:open                                       *\n");
    printf("*�ر��ļ�:close                                      *\n");
    printf("*�ļ����:read                                       *\n");
    printf("*�ļ���д:write                                      *\n");
    printf("*�˳�:exit                                           *\n");
    printf("*��������:                                           *\n");
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
            printf("�������ļ�����\n");
            scanf("%s",&fname);
            flag=FCREATE(fname);
            if(flag==1)
                printf("�ļ�%s �����ɹ�\n",fname);
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
                    printf("�ļ��������ɹ�\n");
            }

        else
            if(!strcmp(cmd,"del"))
            {
                scanf("%s",fname);
                flag=FDELETE(fname);
                if(flag==0)
                    printf("�ļ�%s ɾ���ɹ�\n",fname);
                if(flag==1)
                    printf("�ļ�%s���򿪣�ɾ��ǰ��ر��ļ�!\n",fname);
                if(flag==2)
                    printf("�ļ�������!\n");
            }
            else
                if(!strcmp(cmd,"open"))
                {
                    scanf("%s",&fname);
                    flag=FOPEN(fname);
                    if(flag==1)
                        printf("�ļ�%s ��\n",fname);
                }
                else
                    if(!strcmp(cmd,"close"))
                    {
                        scanf("%s",&fname);
                        flag=FCLOSE(fname);
                        if(flag==1)
                            printf("�ļ�%s �ر�\n",fname);
                    }
                     else
                        if(!strcmp(cmd,"read"))
                        {
                            scanf("%s",fname);
                            p=fcb;
                            printf("������Ҫ�����ļ���С:\n");
                            scanf("%d",&len);
                            id=p->fileid;
                            flag=FREAD(str,len,id);
                            if(flag1==1)
                                printf("�ļ�%s δ�򿪣����ȴ��ļ�\n",fname);
                            if(flag1==2)
                                printf("�����ļ�̫���ˣ�\n");
                        }
                        else
                            if(!strcmp(cmd,"write"))
                            {
                                scanf("%s",fname);
                                p=fcb;
                                id=p->fileid;
                                printf("�������ļ���С:\n");
                                scanf("%d",&len);
                                ptr=(char *)malloc(len+1);
                                printf("������д������ݣ�\n");
                                scanf("%s",ptr);
                                flag=FWRITE(ptr,len,id);
                                if(flag==1)
                                    printf("�ļ�%s δ�򿪣����ȴ��ļ�!\n",fname);
                                if(flag==2)
                                    printf("�ռ䲻��!\n");
                                if(flag==3)
                                    printf("д���ļ�̫��!\n");
                                if(flag==0)
                                    printf("д��ɹ�!\n");
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
    printf("�����봴�����ļ�ϵͳ�Ĵ�С:\n");
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
		//��ʼ��
		cmd = -1;
		Init_block(filename, FILE_NAME_LENGTH);
		if (strncmp("dir", ch, 3) == 0)			//���Ŀ¼dir(non)
		{
			cmd = DIR;
		}
		if (strncmp("creat", ch, 6) == 0)			//�����ļ�����create(filename)
		{
			cmd = CREATE;
			strcat(filename, ch + 7);
		}
		if (strncmp("del", ch, 6) == 0)			//ɾ���ļ�����delete(filename)
		{
			cmd = DELETE;
			strcat(filename, ch + 7);
		}
		if (strncmp("open", ch, 4) == 0)			//���ļ�����open(filename)
		{
			cmd = OPEN;
			strcat(filename, ch + 5);
		}
		if (strncmp("close", ch, 5) == 0)			//�ر��ļ�����close(index)
		{
			cmd = CLOSE;
		}
		if (strncmp("read", ch, 4) == 0)			//���ļ�����read(index)
		{
			cmd = READ;
		}
		if (strncmp("write", ch, 5) == 0)			//д�ļ�����write(index)
		{
			cmd = WRITE;
		}


		}
		if (strncmp("oplist", ch, 6) == 0)			// �鿴���ļ���
		{
			cmd = OPLIST;
		}
		if (strncmp("exit", ch, 4) == 0)			// �˳�����exit
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
				printf("�����ļ��ɹ�|\n");
			printf("----------------------------------------------\n");
			break;
		case DELETE:
			if (destroy(filename) == OK)
				printf("ɾ���ļ��ɹ�|\n");
			printf("----------------------------------------------\n");
			break;
		case OPEN:
			if (open(filename) == OK)
				printf("���ļ��ɹ�|\n");
			printf("----------------------------------------------\n");
			break;
		case CLOSE:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫ�ر��ļ��������ţ�\n");
			scanf("%d", &index);
			if (close(index) == OK)
				printf("�رղ����ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case READ:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����a\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("�������ȡ�ļ��������ţ�\n");
			scanf("%d", &index);
			printf("��������Ҫ��ȡ�ļ����ȣ�\n");
			scanf("%d", &count);
			if (read(index, 0, count) == OK)
				printf("���ļ������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case WRITE:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫд���ļ��������ţ�\n");
			scanf("%d", &index);
			printf("��������Ҫд���ļ����ȣ�\n");
			scanf("%d", &count);
			if (write(index, 0, count) == OK)
				printf("д������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case LSEEK:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫд���ļ���������:\n");
			scanf("%d", &index);
			printf("��������Ҫ���õ��ļ����λ��\n");
			scanf("%d", &pos);
			lseek(index, pos);
			printf("----------------------------------------------\n");
			break;
		case OPLIST:
			if (show_openlist() == 0)
			{
				printf("\t\t\n��ǰû���ļ�����\n");
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
			printf("ָ�����\n");
			printf("----------------------------------------------\n");
			break;
		}
		fflush(stdin);
		Init_block(ch, CH_LENGTH);
	}

}*/ 
