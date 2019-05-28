#include<iostream>
#include<windows.h>
#include<time.h>
#include<malloc.h>
#include<conio.h>

using namespace std;

#define P 20				// �����ڴ��С
#define F 3					// �����ڴ��С

int referenceString[P];		// ���ô�
int M[F];					// �����ڴ�����

float interrupt[6] = { 0.0 };


// ҳ�����ݽṹ
typedef struct Page
{
	int data;			// ҳ����������
	bool access;			// ����λ
	bool modify;			// �޸�λ
	struct Page *next;		// ��һҳ��
} *PageList;	   // ҳ�浥Ԫ������
typedef struct
{
	PageList front;
	PageList rear;
}PageQueue;		   // ҳ�����

// ���ɾ��оֲ����ʵ����ô�
/*
	@param: int p		���������ʼλ��
	@param: int e		���������ҳ��
	@param: int m		�������ƶ���
	@param: double t	�����ֵ
*/	
void getReferenceString(int p, int e, int m, double t)
{
	int pos = 0;
	int last = 0;
	int i = 0; 
	cout << "\n���ô���" << endl;
	while (1) 
	{
		if (last + m > P)
		{
			break;
		}
		// ����m��ȡֵ��Χ��p��p+e֮������������¼�����ô���
		for (i = last; i <last + m; i++, pos++)
		{
			Sleep(500);

			srand((unsigned)time(NULL));
			referenceString[i] = (rand() % e + p) % P;

			cout << referenceString[pos] << "  ";
		}

		// ����һ��������ڣ�0��1����r
		srand((unsigned)time(NULL));
		double r = (double)(rand() % 100) / 100.0;

		// �Ƚ�r��t��ֵ
		if (r < t)	// r < t ����һ����ֵ
		{
			p = rand() % P;
		}
		else		// r ���ڵ��� t p = (p+1) mod P
		{
			p = (p + 1) % P;
		}

		last = i;

	}


}

/*
	-----------------------------------------�������-------------------------------------------
*/

// ��������
bool creatList(PageList &L)
{
	L = (PageList)malloc(sizeof(Page));

	if (!L)
	{
		cout << "Error with establish the List!" << endl;
		return false;
	}
	else
	{
		L->next = L;
		L->access = 0;
		L->modify = 0;

		return true;
	}
}

// ����ڵ�
bool insertList(PageList &L, int data)
{
	// ��ʼ���ڵ�����
	PageList p = (PageList)malloc(sizeof(Page));	// ��������
	PageList q = (PageList)malloc(sizeof(Page));	// ����ڵ�

	// ���ò���ڵ���Ϣ
	q->data = data;									// ������������
	q->access = 1;									// ���÷���λ
	q->modify = 0;									// �����޸�λ

	// ���ñ�������ģ��
	p = L;											// ָ��Ŀ������

	// ���������һ������ڵ�
	while (p->next != L)
	{
		p = p->next;
	}

	// ����q�ڵ�
	p->next = q;
	q->next = L;

	return true;
}

// ���ýڵ�����
bool setAccessBit(PageList &L, int pos, bool access)// ���ýڵ��޸�λ
{
	// �������ݵ���ȷ��
	if (L->next == L)
	{
		cout << "Error with List" << endl;
		return false;
	}

	// ��ʼ������
	PageList p = (PageList)malloc(sizeof(Page));
	p = L->next;

	// �����±�Ϊpos�Ľڵ�
	int i = 1;
	while (p != L && i != pos)
	{
		p = p->next;
	}

	// ���������
	if (p == L)
	{
		return false;
	}
	else
	{
		p->access = access;
	}

	return true;
}
bool setModifyBit(PageList &L, int pos, bool modify)// ���ýڵ��޸�λ
{
	// �������ݵ���ȷ��
	if (L->next == L)
	{
		cout << "Error with List" << endl;
		return false;
	}

	// ��ʼ������
	PageList p = (PageList)malloc(sizeof(Page));
	p = L->next;

	// �����±�Ϊpos�Ľڵ�
	int i = 1;
	while (p != L && i != pos)
	{
		p = p->next;
	}

	// ���������
	if (p == L)
	{
		return false;
	}
	else
	{
		p->modify = modify;
	}
	return true;
}

// ��ѯ�ڵ�
int searchListwithData(PageList &L, int data)
{
	int pos = 1;
	
	// ��ʼ����������
	PageList p = (PageList)malloc(sizeof(Page));
	p = L->next;

	// ��������
	while (p != L && data != p->data)
	{
		p = p->next;
		pos++;
	}

	// �жϲ�ѯ�Ƿ��н��
	if (p == L)
	{
		return -1;
	}
	else
	{
		return pos;
	}
}


// ���ٽڵ�
bool destroyList(PageList &L)
{
	// ��ʼ����ʱ����
	PageList p = (PageList)malloc(sizeof(Page));	// ��ʼ����ʱ��������
	PageList q = (PageList)malloc(sizeof(Page));	// ��ʼ����ʱ�ڵ����

	if (!p || !q)
	{
		cout << "Error with the temp List or Node" << endl;
		return false;
	}

	p = L->next;

	// �ͷ�
	while (p != L->next)
	{
		q = p->next;
		free(p);
		p = q;
	}

	free(q);

	return true;
}
/*
	---------------------------------------------------------------------------------------------
*/


/*
	-----------------------------------------���в���-------------------------------------------
*/
bool initQueue(PageQueue &Q)
{
	// ��ʼ������ָ��
	Q.front = Q.rear = (PageList)malloc(sizeof(Page));

	if (!Q.front || !Q.rear)
	{
		cout << "Error with QueueInitial" << endl;
		return false;
	}

	Q.front->next = NULL;

	return true;
}

bool EnQueue(PageQueue &Q, int data)
{
	// ��ʼ������Ԫ��
	PageList p = (PageList)malloc(sizeof(Page));
	if (!p)
	{
		cout << "Error with malloc in EnQueue" << endl;
		return false;
	}

	// ���ò������Ԫ��
	p->data = data;
	p->next = NULL;

	// ����Ŀ�����
	Q.rear->next = p;
	Q.rear = p;

	return true;
}

bool DeQueue(PageQueue &Q, int &data)
{
	if (Q.front == Q.rear)
	{
		cout << "Error with Queue in DeQueue" << endl;
		return false;
	}

	PageList p = (PageList)malloc(sizeof(Page));
	if (!p)
	{
		cout << "Error with malloc in DeQueue" << endl;
		return false;
	}

	p = Q.front->next;
	data = p->data;

	Q.front->next = p->next;

	if (Q.rear == p)
		Q.rear = Q.front;

	free(p);

	return true;
}

bool DeQueueOne(PageQueue &Q, int &data, int pos, PageQueue &PQ)
{
	if (Q.front == Q.rear)
	{
		cout << "Error with Queue in DeQueue" << endl;
		return false;
	}

	PageList p = (PageList)malloc(sizeof(Page));
	if (!p)
	{
		cout << "Error with malloc in DeQueue" << endl;
		return false;
	}


	p = Q.front->next;
	int i = 1; 
	while (i != pos && p->next != NULL)
	{
		EnQueue(PQ, p->data);
		p = p->next;
	}

	data = p->data;

	if (p == NULL)
	{
		cout << "Error with DeQueueOne" << endl;
		return false;
	}

	while (p->next != NULL)
	{
		EnQueue(PQ, p->next->data);
		p = p->next;
	}

	return true;
}

int searchQueue(PageQueue &Q, int data)
{
	int pos = 1;

	// ������к�����
	if (Q.front == Q.rear)
	{
		cout << "Error with Queue in DeQueue" << endl;
		exit(-1);
	}

	// ��ʼ��������ʱԪ��
	PageList p = (PageList)malloc(sizeof(Page));
	if (!p)
	{
		cout << "Error with malloc in searchQueue" << endl;
		exit(-1);
	}

	// ����Ѱ��Ŀ��Ԫ�ض����±�
	p = Q.front->next;
	while (p != NULL && data != p->data)
	{
		p = p->next;
		pos++;
	}
	if (p == NULL)
	{
		return -1;
	}

	return pos;
}

bool destroyQueue(PageQueue &Q)
{
	while (Q.front)
	{
		Q.rear = Q.front->next;
		free(Q.front);
		Q.front = Q.rear;
	}
	return true;
}

/*
	---------------------------------------------------------------------------------------------
*/

/*
	-----------------------------------------���̷���-------------------------------------------
*/

// ORA�ķ���

// ����Ԫ���������Ƿ����
template<class T>
int checkShowUp(T&v, int target, int begin, int end)	// ����Ԫ���������Ƿ����
{
	// ��ȡԪ�ؼ�����
	size_t length = sizeof(v) / sizeof(int);
	if (end > length || begin < 0 || begin > end)
	{
		cout << "Error with the pos" << endl;
		return -1;
	}

	// ����showup��־
	bool flag = false;

	// ��������������ֵ�
	for (int i = begin; i < end; i++)
	{
		if (v[i] == target)
		{
			return i;
		}
	}

	return -1;

}

// �õ���Զ��ҳ����־���
template<class T>
int getMostFar(T&v, int pos, int target)
{
	// ��ȡԪ�ؼ�����
	size_t length = sizeof(v) / sizeof(int);

	// �ж��Ƿ���Ŀ��
	if (checkShowUp(v, target, pos, length) != -1)
	{
		return checkShowUp(v, target, pos, length) - pos;
	}
	else
	{
		return length - pos + 1;
	}

}

// ��ȡ��������������±�
template<class T>
int getPosOfMost(T&v)
{
	// ��ȡԪ�ؼ�����
	size_t length = sizeof(v) / sizeof(int);

	// �õ�����Ԫ��
	int max = 0;
	for (int i = 0; i < length; i++)
	{
		if (v[i] > max)
		{
			max = v[i];
		}
	}

	// �ҵ����Ԫ�ص��±�
	for (int i = 0; i < length; i++)
	{
		if (v[i] == max)
		{
			return i;
		}
	}

	return -1;
}

// PRA�ķ���
template<class T>
int getRand(T&v)
{
	size_t length = sizeof(v) / sizeof(int);

	Sleep(1000);
	srand(time(NULL));

	return rand() % length;

}

// FIRST_CLOCK����
int searchAndSetAccess(PageList &Q)
{
	int pos = 1;
	PageList p = (PageList)malloc(sizeof(Page));

	if (!p)
	{
		cout << "Error with malloc in searchAndSetAccess" << endl;
		return -1;
	}

	p = Q->next;
	while (p->access != 0)
	{
		p->access = 0;
		p = p->next;

		if (p == Q)
		{
			p = p->next;
		}

		pos++;
		if (pos > F)
		{
			pos = 1;
		}
	}


	return pos;
}

bool replacePage(PageList &L, int pos,  int data)
{
	if (L->next == L)
	{
		cout << "Error with the input in replace page" << endl;
		exit(-1);
	}
	PageList p = (PageList)malloc(sizeof(Page));
	PageList q = (PageList)malloc(sizeof(Page));

	q->data = data;
	p = L;

	for (int j = 0; j < pos - 1; j++)
	{
		p = p->next;
	}

	q->next = p->next->next;
	p->next = q;
	q->access = true;
	q->modify = false;

	return true;

}

// SECOND_CLOCK����
int searchDatawithModify(PageList &L)
{
	int pos_modify = 1;

	PageList p = (PageList)malloc(sizeof(Page));
	p = L->next;

	// ɨ�����λ��Ķ�λͬʱΪ0
	while (p != L)
	{
		if (p->access == 0 && p->modify == 0)
			break;	//�ҵ�
		p = p->next;
		pos_modify++;
	}

	if (p == L)
	{
		pos_modify = 1;
		p = L->next;
		//ɨ�����λΪ0���Ľ�λΪ1�Ľ�㣬ͬʱ�޸ķ��ʹ��Ľ��ķ���λΪ0
		while (p != L)
		{
			if (p->access != 0)
				p->access = 0;
			else if (p->modify == 1)
				break;
			p = p->next;
			pos_modify++;

		}
	}
	if (p == L)
	{
		pos_modify = 1;
		p = L->next;
		//ɨ��A=0����M=0�Ľ��
		while (p != L)		
		{
			if (p->access == 0 && p->modify == 0)
				break;
			p = p->next;
			pos_modify++;
		}
		if (p == L)
		{
			pos_modify = 1;
			p = L->next;

			//ɨ��A=0����M=1�Ľ��
			while (p != L)
			{
				if (p->access != 0)
					p->access = 0;
				else if (p->modify == 1)
					break;
				p = p->next;
				pos_modify++;

			}
		}

	}
	return pos_modify;
}
/*
	---------------------------------------------------------------------------------------------
*/

/*
	-----------------------------------------���̺���-------------------------------------------
*/

void OPT()
{
	cout << "\nExcute the function named Optimal replacement algorithm..." << endl;

	int total_interupt = 0;

	// �����ڴ��λ�������ô�
	for (int i = 0; i < F; i++)
	{
		M[i] = referenceString[i];
	}

	cout << "��ʼ�����ڴ�ҳ�棺 ";
	for (int i = 0; i < F; i++)
	{
		cout << M[i] << " ";
	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		bool flag = true;	// ȱҳ�жϱ�־
		for (int j = 0; j < F; j++)
		{
			if (referenceString[i] == M[j])
			{
				flag = false;
				break;
			}
		}

		// �ж�ȱҳ�жϱ�־�Ƿ���
		if (flag)
		{
			total_interupt++;

			// ��ȡ�����ڴ�����Զ��
			int dist[F];
			for (int j = 0; j < F; j++)
			{
				dist[j] = getMostFar(referenceString, i, M[j]);
			}

			// �ж�����ҳ���±�
			int drop = getPosOfMost(dist);

			// �û�ҳ��
			M[drop] = referenceString[i];

		}

		cout << "��ҳ��" << referenceString[i] << endl;
		cout << "��ǰ�����ڴ�ҳ�������";
		for (int j = 0; j < F; j++)
		{
			cout << M[j] << " ";
		}
		cout << endl;
	}
	cout << "ORA method`s interupt is " << total_interupt << endl;
	
	interrupt[0] = ((float)total_interupt / 20.0)*100.0;
}

void PRA()
{

	cout << "\nExcuted the function named Random replacement algorithm..." << endl;

	int total_interrupt = 0;
	// �����ڴ��λ�������ô�
	for (int i = 0; i < F; i++)
	{
		M[i] = referenceString[i];
	}

	cout << "��ʼ�����ڴ�ҳ�棺 ";
	for (int i = 0; i < F; i++)
	{
		cout << M[i] << " ";
	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		bool flag = true;	// ȱҳ�жϱ�־
		for (int j = 0; j < F; j++)
		{
			if (referenceString[i] == M[j])
			{
				flag = false;
				break;
			}
		}

		// �ж�ȱҳ�жϱ�־�Ƿ���
		if (flag)
		{
			total_interrupt++;

			// �ж�����ҳ���±�
			int drop = getRand(M);

			// �û�ҳ��
			M[drop] = referenceString[i];

		}

		cout << "\n��ҳ��" << referenceString[i] << endl;
		cout << "\n��ǰ�����ڴ�ҳ�������";
		for (int j = 0; j < F; j++)
		{
			cout << M[j] << " ";
		}
		cout << endl;
	}
	cout << "\nPRA method`s interupt is " << total_interrupt << endl;

	interrupt[1] = ((float)total_interrupt / 20.0)*100.0;
}

void FIFO()
{

	cout << "\nExcuted the function named Firstly input Firstly output algorithm..." << endl;

	// ȱҳ�жϼ���
	int total_interrupt = 0;

	// ��ʼ��ҳ�����Ԫ��
	PageQueue Q;
	
	if (!initQueue(Q))
	{
		cout << "FIFO method interrupted by initQueue" << endl;
		system("pause");
	}

	// ��ʼ���������ڴ��λҳ��
	for (int i = 0; i < F; i++)
	{
		if (!EnQueue(Q, referenceString[i]))
		{
			cout << "FIFO method interrupted by EnQueue" << endl;
			system("pause");
		}
	}

	cout << "��ʼ����ҳ�棺";

	// ��ʼ����ʱ���
	PageList p = (PageList)malloc(sizeof(Page));
	p = Q.front->next;

	// �����ʼ����ҳ��
	while(p!=NULL)
	{
		cout << p->data << "  ";
		p = p->next;
	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		// ��ѯ�������Ƿ��д���ҳ��
		if (searchQueue(Q, referenceString[i]) == -1)
		{
			int resultPage = -1;

			// ɾ�������ڴ�ҳ��
			DeQueue(Q, resultPage);

			// ��������ڴ�ҳ��
			EnQueue(Q, referenceString[i]);
			total_interrupt++;

		}

		cout << "\n����ҳ�棺" << referenceString[i] << endl;

		// ���¶���
		p = Q.front->next;

		// �����ǰ�����ڴ�
		cout << "��ǰ�����ڴ棺";
		for (int j = 0; p != NULL && j < F; j++)
		{
			cout << p->data << "  ";
			p = p->next;
		}
	}

	cout << "\nFIFO method`s interupt is " << total_interrupt << endl;
	interrupt[2] = ((float)total_interrupt / 20.0)*100.0;
	free(p);
	destroyQueue(Q);

}

void LRU()
{

	cout << "\nExcuted the function named Firstly input Firstly output algorithm..." << endl;

	// ȱҳ�жϼ���
	int total_interrupt = 0;

	// ��ʼ��ҳ�����Ԫ��
	PageQueue Q;

	int QNode_num = 0;

	// ��ʼ������
	initQueue(Q);

	if (!initQueue(Q))
	{
		cout << "LRU method interrupted by initQueue" << endl;
		system("pause");
	}

	// ��ʼ�������ڴ�
	for (int i = 0; i < F; i++)
	{
		if (!EnQueue(Q, referenceString[i]))
		{
			cout << "LRU method interrupted by EnQueue" << endl;
			system("pause");
		}
	}
	cout << "\n��ʼ����ҳ�棺";
	PageList p = (PageList)malloc(sizeof(Page));
	p = Q.front->next;

	for (int i = 0; i < F&&p != NULL; i++)
	{
		cout << p->data << "  ";
		p = p->next;
	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		// û���ҵ�Ŀ��Ԫ��
		int pos = searchQueue(Q, referenceString[i]);
		if (pos == -1)
		{
			int resultPage = -1;
			DeQueue(Q, resultPage);

			EnQueue(Q, referenceString[i]);


			total_interrupt++;
		}
		// �ҵ�ͬҳ�潫��ɾ�����������
		else
		{
			int resultPage = -1;
			PageQueue temp;
			if (!initQueue(temp))
			{
				cout << "LRU method interrupted by initQueue" << endl;
				system("pause");
			}

			if (!DeQueueOne(Q, resultPage, pos, temp))
			{
				cout << "Error with DeQueueOne in LRU" << endl;
				system("pause");
			}
			else
			{
				Q = temp;
				EnQueue(Q, resultPage);
			}

		}
		cout << "\n����ҳ�棺" << referenceString[i] << endl;

		// ���¶���
		p = Q.front->next;

		// �����ǰ�����ڴ�
		cout << "\n��ǰ�����ڴ棺";
		for (int j = 0; p != NULL && j < F; j++)
		{
			cout << p->data << "  ";
			p = p->next;
		}
	}

	cout << "\nLRU method`s interupt is " << total_interrupt << endl;
	interrupt[3] = ((float)total_interrupt / 20.0)*100.0;
	free(p);
	destroyQueue(Q);
}

void FIRST_CLOCK()
{

	cout << "\nExcuted the function named Firstly input Firstly output algorithm..." << endl;

	// ȱҳ�жϼ���
	int total_interrupt = 0;

	PageList L;
	creatList(L);

	PageList p = (PageList)malloc(sizeof(Page));

	
	for (int i = 0; i < F; i++)
	{
		insertList(L, referenceString[i]);
		setAccessBit(L, i + 1, true);
		setModifyBit(L, i + 1, false);

	}
	cout << "\n��ʼ����ҳ�棺";
	for (p = L->next; p != L; p = p->next)
	{
		cout << p->data << "  ";

	}
	cout << endl;

	cout << "\n����ҳ�����λ��";
	for (p = L->next; p != L; p = p->next)
	{
		cout << p->access << "  ";

	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		int searchPos = searchListwithData(L, referenceString[i]);
		if (searchPos == -1)
		{
			int pos = searchAndSetAccess(L);

			if (!replacePage(L, pos, referenceString[i]))
			{
				cout << "Error in the replacePage" << endl;
			}
			total_interrupt++;

		}
		else
			setAccessBit(L, searchPos, true);

		cout << "����ҳ�棺" << referenceString[i] << endl;

		cout << "����ҳ�棺";
		
		for (p = L->next; p != L; p = p->next)
		{
			cout << p->data << "  ";

		}
		cout << endl;

		cout << "����λ��";
		for (p = L->next; p != L; p = p->next)
		{
			cout <<p->access << "  ";

		}
		cout << endl;
	}

	cout << "\nFIRST_CLOCK method`s interupt is " << total_interrupt << endl;
	interrupt[4] = ((float)total_interrupt / 20.0)*100.0;
	free(p);
	destroyList(L);

}

void SECOND_CLOCK()
{
	cout << "\nExcuted the function named SecondClock algorithm..." << endl;

	// ȱҳ�жϼ���
	int total_interrupt = 0;

	PageList L;
	creatList(L);

	PageList p = (PageList)malloc(sizeof(Page));

	for (int i = 0; i < F; i++)
	{
		insertList(L, referenceString[i]);
	}

	cout << "��ʼ����ҳ�棺";
	for (p = L->next; p != L; p = p->next)
	{
		cout << p->data << "  ";
	}
	cout << endl;

	// ���modifyҳ��
	Sleep(1000);
	srand(time(NULL));		//����ʱ������
	int m = rand() % F;
	setModifyBit(L, m, true);

	cout << "����λ���޸�λ��";
	for (p = L->next; p != L; p = p->next)
	{
		cout <<p->access<<","<< p->modify << "  ";
	}
	cout << endl;

	for (int i = F; i < P; i++)
	{
		int searchPos = searchListwithData(L, referenceString[i]);
		if (searchPos == -1)
		{
			int pos_modify = searchDatawithModify(L);
			if (!replacePage(L, pos_modify, referenceString[i]))
			{
				cout << "Error with replacePage in SECOND_CLOCK" << endl;
				exit(-1);
			}
			total_interrupt++;
		}
		else
			setAccessBit(L, searchPos, true);

		cout << "����ҳ��" << referenceString[i] << endl;
		
		cout << "����ҳ�棺";
		for (p = L->next; p != L; p = p->next)
		{
			cout << p->data << "  ";
		}
		cout << endl;

		Sleep(1000);
		cout << "�޸�ҳ���ڴ�����ţ���" << m << endl;
		setModifyBit(L, m, true);

		cout << "����λ��Ľ�λ��";
		p = L->next;
		while (p != L)
		{
			cout << p->access << "," << p->modify << "  ";
			p = p->next;
		}
		cout << endl;
	}
	cout << "FIRST_CLOCK method`s interupt is " << total_interrupt << endl;
	interrupt[5] = ((float)total_interrupt / 20.0)*100.0;
	destroyList(L);

}
/*
	---------------------------------------------------------------------------------------------
*/


int main()
{

	getReferenceString(0,5,5,0.6);
	OPT();
	PRA();
	
	FIFO();
	LRU();
	FIRST_CLOCK();
	SECOND_CLOCK();


	cout << "ȱҳ��:" << endl;
	cout << "\n����û�: " << interrupt[0] << "%" << endl;
	cout << "\n����û�: " << interrupt[1] << "%" << endl;
	cout << "\n�Ƚ��ȳ��û�: " << interrupt[2] << "%" << endl;
	cout << "\n������δʹ���û�: " << interrupt[3] << "%" << endl;
	cout << "\nCLOCK�û�: " << interrupt[4] << "%" << endl;
	cout << "\n�Ľ�CLOCK�û�: " << interrupt[5] << "%" << endl;

	system("pause");
	return 1;
}
