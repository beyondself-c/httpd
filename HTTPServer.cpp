#include "_public.h"

CTcpServer TcpServer;

void mainexit(int sig);   // �ź�2��15�Ĵ�������
vector<long> vpthid;  // ����߳�id��������

int main(){
 
   // �ر�ȫ�����ź�
  for (int ii=0;ii<100;ii++) signal(ii,SIG_IGN);

  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  signal(SIGINT,mainexit); signal(SIGTERM,mainexit);

  if ( Logfile() == false ){
    printf("logfile open failed\n"); return -1;
  }
	
  if(TcpServer.InitServer(80) == false){
    printf("TcpServer.InitServer(80) failed\n");
    TcpServer.CloseListen();
    return -1;
  }

  while(1){
    if(TcpServer.Accept() == false) continue;    
    
    pthread_t pthid;
    if(pthread_create(&pthid, NULL, accept_request,\
                      (void*)((long)TcpServer.m_connfd))!=0 )
    perror("pthread_create");

    vpthid.push_back(pthid);

  }

  TcpServer.CloseClient();
  
  return 0;	
}

void mainexit(int sig){
  printf("mainexit begin.\n");

  // �رռ�����socket��
  TcpServer.CloseListen();

  // ȡ��ȫ�����̡߳�
  for (int ii=0;ii<vpthid.size();ii++)
  {
    printf("cancel %ld\n",vpthid[ii]);
    pthread_cancel(vpthid[ii]);
  }

  printf("mainexit end.\n");

  exit(0);
}
 
