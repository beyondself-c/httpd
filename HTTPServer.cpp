#include "_public.h"

CTcpServer TcpServer;

void mainexit(int sig);   // 信号2和15的处理函数。
vector<long> vpthid;  // 存放线程id的容器。

int main(){
 
   // 关闭全部的信号
  for (int ii=0;ii<100;ii++) signal(ii,SIG_IGN);

  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
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

  // 关闭监听的socket。
  TcpServer.CloseListen();

  // 取消全部的线程。
  for (int ii=0;ii<vpthid.size();ii++)
  {
    printf("cancel %ld\n",vpthid[ii]);
    pthread_cancel(vpthid[ii]);
  }

  printf("mainexit end.\n");

  exit(0);
}
 
