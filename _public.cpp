/*
 *������: _public.h���������ܺ���������ͷ�ļ�
 * 
*/

#include "_public.h"

CLogFile logfile;

bool Logfile(){
  if(logfile.Open("/root/httpd/logfile") == false) return false;

  return true;
}


//C++�ķ�����װsocket�ͻ���
CTcpClient::CTcpClient()
{
  m_sockfd=-1;
  memset(m_ip,0,sizeof(m_ip));
  m_port=0;
  m_btimeout=false;
}
 
// ��������������ӣ�ip-�����ip��portͨ�Ŷ˿�
bool CTcpClient::ConnectToServer(const char *ip,const int port)
{
  if(m_sockfd!=-1) { Close(); m_sockfd=-1;  };

  strcpy(m_ip,ip);
  m_port=port;
 
  struct hostent* h;  // ip��ַ��Ϣ�����ݽṹ
  struct sockaddr_in servaddr;

  // �����ͻ��˵�socket
  if ( (m_sockfd = socket(AF_INET,SOCK_STREAM,0)) <0) return false;  
 
  if ( (h=gethostbyname(m_ip))==0 )
  { close(m_sockfd); m_sockfd=-1; return false; }
 
  // �ѷ������ĵ�ַ�Ͷ˿�ת��Ϊ���ݽṹ
  
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);
 
  // �������������������
  if (connect(m_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)
  { close(m_sockfd); m_sockfd=-1; return false; }
 
  return true;

}

bool CTcpClient::Read(char *buffer,const int itimeout)
{
  if (m_sockfd == -1) return false;

  if (itimeout>0)
  {
    fd_set tmpfd;

    FD_ZERO(&tmpfd);
    FD_SET(m_sockfd,&tmpfd);

    struct timeval timeout;
    timeout.tv_sec = itimeout; timeout.tv_usec = 0;

    m_btimeout = false;

    int i;
    if ( (i = select(m_sockfd+1,&tmpfd,0,0,&timeout)) <= 0 )
    {
      if (i==0) m_btimeout = true;
      return false;
    }
  }

  m_buflen = 0;
  return (TcpRead(m_sockfd,buffer,&m_buflen));
}

bool CTcpClient::Write(const char *buffer,const int ibuflen)
{
  if (m_sockfd == -1) return false;

  fd_set tmpfd;

  FD_ZERO(&tmpfd);
  FD_SET(m_sockfd,&tmpfd);

  struct timeval timeout;
  timeout.tv_sec = 5; timeout.tv_usec = 0;

  m_btimeout = false;

  int i;
  if ( (i=select(m_sockfd+1,0,&tmpfd,0,&timeout)) <= 0 )
  {
    if (i==0) m_btimeout = true;
    return false;
  }

  int ilen=ibuflen;

  if (ibuflen==0) ilen=strlen(buffer);

  return(TcpWrite(m_sockfd,buffer,ilen));

}

void CTcpClient::Close()
{
  if(m_sockfd>0) close(m_sockfd);
  m_sockfd=-1;
  memset(m_ip,0,sizeof(m_ip));
  m_port=0;
  m_btimeout=false;
}

CTcpClient::~CTcpClient()
{
  Close();  // ���������ر�m_sockfd
}

/*
// �����˷��ͱ���
int CTcpClient::Send(const void *buf,const int buflen)
{
  return send(m_sockfd,buf,buflen,0);
}
// ���նԶ˵ı���
int CTcpClient::Recv(void *buf,const int buflen)
{
  return recv(m_sockfd,buf,buflen,0);
}
*/

//C++�ķ�����װsocket�����
CTcpServer::CTcpServer()
{
  m_socklen=1;
  m_listenfd=-1;
  m_connfd=0;
  m_btimeout=false;
}
 
// ��ʼ������˵�socket��portΪͨ�Ŷ˿�
bool CTcpServer::InitServer(const unsigned int port)  // ��ʼ�������
{
  if( m_listenfd >0) { close(m_listenfd); m_listenfd=-1;}

  m_listenfd = socket(AF_INET,SOCK_STREAM,0);  // ��������˵�socket
 

  // Linux����
  int opt = 1; unsigned int len = sizeof(opt);
  setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,len);


  // �ѷ��������ͨ�ŵĵ�ַ�Ͷ˿ڰ󶨵�socket��
  memset(&m_servaddr,0,sizeof(m_servaddr));
  m_servaddr.sin_family = AF_INET;  // Э���壬��socket�����ֻ����AF_INET
  m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // ������������ip��ַ
  m_servaddr.sin_port = htons(port);  // ��ͨ�Ŷ˿�
  if (bind(m_listenfd,(struct sockaddr *)&m_servaddr,sizeof(m_servaddr)) != 0 )
  { CloseListen();  return false; }
 
  // ��socket����Ϊ����ģʽ
  if (listen(m_listenfd,5) != 0 ) {CloseListen(); return false; }
 
  m_socklen=sizeof(struct sockaddr_in);

  return true;
}

bool CTcpServer::Accept()  // �ȴ��ͻ��˵�����
{
  if(m_listenfd==-1) return false;
  if ( (m_connfd=accept(m_listenfd,(struct sockaddr* )&m_clientaddr,\
                       (socklen_t*)&m_socklen)) <= 0) return false;
 
  logfile.Write("�ͻ���%s������\n",inet_ntoa(m_clientaddr.sin_addr));
  return true;
}

char* CTcpServer::GetIP()
{
  return ( inet_ntoa(m_clientaddr.sin_addr)  );

}

bool CTcpServer::Read(char *buffer,const int itimeout )
{
    if (m_connfd == -1) return false;

  if (itimeout>0)
  {
    fd_set tmpfd;

    FD_ZERO(&tmpfd);
    FD_SET(m_connfd,&tmpfd);

    struct timeval timeout;
    timeout.tv_sec = itimeout; timeout.tv_usec = 0;

    m_btimeout = false;

    int i;
    if ( (i = select(m_connfd+1,&tmpfd,0,0,&timeout)) <= 0 )
    {
      if (i==0) m_btimeout = true;
      return false;
    }
  }

  m_buflen = 0;
  return(TcpRead(m_connfd,buffer,&m_buflen));
}


bool CTcpServer::Write(const char *buffer,const int ibuflen)
{
  if (m_connfd == -1) return false;

  fd_set tmpfd;

  FD_ZERO(&tmpfd);
  FD_SET(m_connfd,&tmpfd);

  struct timeval timeout;
  timeout.tv_sec = 5; timeout.tv_usec = 0;

  m_btimeout = false;

  int i;
  if ( (i=select(m_connfd+1,0,&tmpfd,0,&timeout)) <= 0 )
  {
    if (i==0) m_btimeout = true;
    return false;
  }
  int ilen = ibuflen;
  if (ilen==0) ilen=strlen(buffer);

  return(TcpWrite(m_connfd,buffer,ilen));

}

void CTcpServer::CloseListen()
{
  if(m_listenfd>0)
  { close(m_listenfd); m_listenfd=-1;}
}

void CTcpServer::CloseClient()
{
  if(m_connfd>0)
  { close(m_connfd); m_connfd=-1; }

}

CTcpServer::~CTcpServer()
{
  CloseListen(); CloseClient();
}

bool TcpRead(const int sockfd,char *buffer,int *ibuflen,const int itimeout)
{
  if(sockfd==-1) return false;

  if(itimeout>0)
  {
        fd_set tmpfd;

    FD_ZERO(&tmpfd);
    FD_SET(sockfd,&tmpfd);

    struct timeval timeout;
    timeout.tv_sec = itimeout; timeout.tv_usec = 0;

    int i;
    if ( (i = select(sockfd+1,&tmpfd,0,0,&timeout)) <= 0 ) return false;
  }

  (*ibuflen)=0;
  if( (Readn(sockfd,(char*)ibuflen,4)==false)  ) return false;

  (*ibuflen)=ntohl(*ibuflen); // �������ֽ���ת��Ϊ�����ֽ���

  if( (Readn(sockfd,buffer,(*ibuflen) )==false)  ) return false;
  
  return true;
}

bool TcpWrite(const int sockfd,const char *buffer,const int ibuflen)
{
  if (sockfd == -1) return false;

  fd_set tmpfd;

  FD_ZERO(&tmpfd);
  FD_SET(sockfd,&tmpfd);

  struct timeval timeout;
  timeout.tv_sec = 5; timeout.tv_usec = 0;

  if ( select(sockfd+1,0,&tmpfd,0,&timeout) <= 0 ) return false;

  int ilen=0;
  if(ibuflen==0) ilen=strlen(buffer);
  else ilen=ibuflen;

  int ilenn=htonl(ilen);

  char strTBuffer[1024];
  memset(strTBuffer,0,sizeof(strTBuffer));
  memcpy(strTBuffer,&ilenn,4);
  memcpy(strTBuffer+4,buffer,ilen);

  if ( (Writen(sockfd,strTBuffer,ilen+4)) == false) return false;

  return true;

}

bool Readn(const int sockfd,char *buffer,const size_t n)
{
  int nLeft,nread,idx;
 
  nLeft=n;
  idx=0;
  
  while(nLeft>0)
  {
   if( (nread=recv(sockfd,buffer+idx,nLeft,0)) <=0 ) return false;

   idx += nread;
   nLeft -= nread; 
  }
 
  return true;

}

bool Writen(const int sockfd,const char *buffer,const size_t n)
{
  int nLeft,idx,nwritten;

  nLeft=n;
  idx=0;
  
  while(nLeft>0)
  {
    if( (nwritten=send(sockfd,buffer+idx,nLeft,0)) <=0 )  return false;
    
    nLeft -= nwritten;
    idx += nwritten;
  }

  return true;

}


/*
  ȡ����ϵͳ��ʱ�䣬����������ʾ��ʱ��ת��Ϊ�ַ�����ʾ�ĸ�ʽ��
  stime�����ڴ�Ż�ȡ����ʱ���ַ�����
  timetvl��ʱ���ƫ��������λ���룬0��ȱʡֵ����ʾ��ǰʱ�䣬30��ʾ��ǰʱ��30��֮���ʱ>��㣬-30��ʾ��ǰʱ��30��֮ǰ��ʱ��㡣
  fmt�����ʱ��ĸ�ʽ��ȱʡ��"yyyy-mm-dd hh24:mi:ss"��Ŀǰ֧�����¸�ʽ��
  "yyyy-mm-dd hh24:mi:ss"���˸�ʽ��ȱʡ��ʽ��
  "yyyymmddhh24miss"
  "yyyy-mm-dd"
  "yyyymmdd"
  "hh24:mi:ss"
  "hh24miss"
  "hh24:mi"
  "hh24mi"
  "hh24"
  "mi"
  ע�⣺
    1��Сʱ�ı�ʾ������hh24������hh����ô����Ŀ����Ϊ�˱��������ݿ��ʱ���ʾ����һ�£�
    2�������г��˳��õ�ʱ���ʽ���������������Ӧ�ÿ������������޸�Դ�������Ӹ����\ ��ʽ֧�֣�
    3�����ú�����ʱ�����fmt��������ʽ��ƥ�䣬stime�����ݽ�Ϊ�ա�
*/
void LocalTime(char *stime,const char *fmt,const int timetvl)
{
  if (stime==0) return;

  time_t  timer;

  time( &timer ); timer=timer+timetvl;

  timetostr(timer,stime,fmt);
}


// ��������ʾ��ʱ��ת��Ϊ�ַ�����ʾ��ʱ�䡣
// ltime��������ʾ��ʱ�䡣
// stime���ַ�����ʾ��ʱ�䡣
// fmt������ַ���ʱ��stime�ĸ�ʽ����LocalTime������fmt������ͬ�����fmt�ĸ�ʽ����ȷ��\stime��Ϊ�ա�
void timetostr(const time_t ltime,char *stime,const char *fmt)
{
  if (stime==0) return;

  strcpy(stime,"");

  struct tm sttm = *localtime ( &ltime );

  sttm.tm_year=sttm.tm_year+1900;
  sttm.tm_mon++;

  if (fmt==0)
  {
    snprintf(stime,20,"%04u-%02u-%02u %02u:%02u:%02u",sttm.tm_year,\
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,\
                    sttm.tm_min,sttm.tm_sec);
    return;
  }

  if (strcmp(fmt,"yyyy-mm-dd hh24:mi:ss") == 0)
  {
    snprintf(stime,20,"%04u-%02u-%02u %02u:%02u:%02u",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min,sttm.tm_sec);
    return;
  }

}

//////////////////////////////////////////////////////////////////////////////////////

// ���ݾ���·�����ļ�����Ŀ¼���𼶵Ĵ���Ŀ¼��
// pathorfilename������·�����ļ�����Ŀ¼����
// bisfilename��˵��pathorfilename�����ͣ�true-pathorfilename���ļ�����������Ŀ¼����\ȱʡֵΪtrue��
// ����ֵ��true-�����ɹ���false-����ʧ�ܣ��������ʧ�ܣ�ԭ���д�������������1��Ȩ��\���㣻 2��pathorfilename�������ǺϷ����ļ�����Ŀ¼����3�����̿ռ䲻�㡣
bool MKDIR(const char *filename,bool bisfilename)
{
  // ���Ŀ¼�Ƿ���ڣ���������ڣ��𼶴�����Ŀ¼
  char strPathName[301];

  int ilen=strlen(filename);

  for (int ii=1; ii<ilen;ii++)
  {
    if (filename[ii] != '/') continue;

    memset(strPathName,0,sizeof(strPathName));
    strncpy(strPathName,filename,ii);

    if (access(strPathName,F_OK) == 0) continue;

    if (mkdir(strPathName,0755) != 0) return false;
  }

  if (bisfilename==false)
  {
    if (access(filename,F_OK) != 0)
    {
      if (mkdir(filename,0755) != 0) return false;
    }
  }

  return true;
}


// ���ļ���
// FOPEN��������fopen�⺯�����ļ�������ļ����а�����Ŀ¼�����ڣ��ʹ���Ŀ¼��
// FOPEN�����Ĳ����ͷ���ֵ��fopen������ȫ��ͬ��
// ��Ӧ�ÿ����У���FOPEN��������fopen�⺯����
FILE *FOPEN(const char *filename,const char *mode)
{
  if (MKDIR(filename) == false) return 0;

  return fopen(filename,mode);
}


CLogFile::CLogFile(const long MaxLogSize)
{
  m_tracefp = 0;
  memset(m_filename,0,sizeof(m_filename));
  memset(m_openmode,0,sizeof(m_openmode));
  m_bBackup=true;
  m_bEnBuffer=false;
  m_MaxLogSize=MaxLogSize;
  if (m_MaxLogSize<10) m_MaxLogSize=10;
}

CLogFile::~CLogFile()
{
  Close();
}

void CLogFile::Close()
{
  if (m_tracefp != 0) { fclose(m_tracefp); m_tracefp=0; }

  memset(m_filename,0,sizeof(m_filename));
  memset(m_openmode,0,sizeof(m_openmode));
  m_bBackup=true;
  m_bEnBuffer=false;
}

// ����־�ļ���
// filename����־�ļ�����������þ���·��������ļ����е�Ŀ¼�����ڣ����ȴ���Ŀ¼��
// openmode����־�ļ��Ĵ򿪷�ʽ����fopen�⺯�����ļ��ķ�ʽ��ͬ��ȱʡֵ��"a+"��
// bBackup���Ƿ��Զ��л���true-�л���false-���л����ڶ���̵ķ�������У�����������\����һ����־�ļ���bBackup����Ϊfalse��
// bEnBuffer���Ƿ������ļ�������ƣ�true-���ã�false-�����ã�������û���������ôд��\��־�ļ��е����ݲ�������д���ļ���ȱʡ�ǲ����á�
bool CLogFile::Open(const char *filename,const char *openmode,bool bBackup,\
                    bool bEnBuffer)
{
  // ����ļ�ָ���Ǵ򿪵�״̬���ȹر�����
  Close();

  strcpy(m_filename,filename);
  m_bEnBuffer=bEnBuffer;
  m_bBackup=bBackup;
  if (openmode==0) strcpy(m_openmode,"a+");
  else strcpy(m_openmode,openmode);

 if ((m_tracefp=FOPEN(m_filename,m_openmode)) == 0) return false;

  return true;
}

// �����־�ļ�����100M���Ͱѵ�ǰ����־�ļ����ݳ���ʷ��־�ļ����л��ɹ�����յ�ǰ��־\�ļ������ݡ�
// ���ݺ���ļ�������־�ļ������������ʱ�䡣
// ע�⣬�ڶ���̵ĳ����У���־�ļ������л������ߵĳ����У���־�ļ������л���
bool CLogFile::BackupLogFile()
{
  if (m_tracefp == 0) return false;

  // ������
  if (m_bBackup == false) return true;

  fseek(m_tracefp,0L,2);

  if (ftell(m_tracefp) > m_MaxLogSize*1024*1024)
  {
    fclose(m_tracefp); m_tracefp=0;

    char strLocalTime[21];
    memset(strLocalTime,0,sizeof(strLocalTime));
    LocalTime(strLocalTime,"yyyymmddhh24miss");

    char bak_filename[301];
    memset(bak_filename,0,sizeof(bak_filename));
    snprintf(bak_filename,300,"%s.%s",m_filename,strLocalTime);
    rename(m_filename,bak_filename);

    if ((m_tracefp=FOPEN(m_filename,m_openmode)) == 0) return false;
  }

  return true;
}

// ������д����־�ļ���fmt�ǿɱ������ʹ�÷�����printf�⺯����ͬ��
// Write������д�뵱ǰ��ʱ�䣬WriteEx������дʱ�䡣
bool CLogFile::Write(const char *fmt,...)
{
  if (m_tracefp == 0) return false;

  if (BackupLogFile() == false) return false;

  char strtime[20]; LocalTime(strtime);

  va_list ap;
  va_start(ap,fmt);
  fprintf(m_tracefp,"%s ",strtime);
  vfprintf(m_tracefp,fmt,ap);
  va_end(ap);

  if (m_bEnBuffer==false) fflush(m_tracefp);
  return true;
}

// ������д����־�ļ���fmt�ǿɱ������ʹ�÷�����printf�⺯����ͬ��
// Write������д�뵱ǰ��ʱ�䣬WriteEx������дʱ�䡣
bool CLogFile::WriteEx(const char *fmt,...)
{
  if (m_tracefp == 0) return false;

  va_list ap;
  va_start(ap,fmt);
  vfprintf(m_tracefp,fmt,ap);
  va_end(ap);

  if (m_bEnBuffer==false) fflush(m_tracefp);

  return true;
}



// ɾ���ַ������ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteLChar(char *str,const char chr)
{
  if (str == 0) return;
  if (strlen(str) == 0) return;

  char strTemp[strlen(str)+1];

  int iTemp=0;

  memset(strTemp,0,sizeof(strTemp));
  strcpy(strTemp,str);

  while ( strTemp[iTemp] == chr )  iTemp++;

  memset(str,0,strlen(str)+1);

  strcpy(str,strTemp+iTemp);

  return;
}

// ɾ���ַ����ұ�ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteRChar(char *str,const char chr)
{
  if (str == 0) return;
  if (strlen(str) == 0) return;

  int istrlen = strlen(str);

  while (istrlen>0)
  {
    if (str[istrlen-1] != chr) break;

    str[istrlen-1]=0;

    istrlen--;
  }
}

// ɾ���ַ�����������ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteLRChar(char *str,const char chr)
{
  DeleteLChar(str,chr);
  DeleteRChar(str,chr);
}



// ���ַ����е�Сд��ĸת���ɴ�д�����Բ�����ĸ���ַ���
// str����ת�����ַ�����֧��char[]��string�������͡�
void ToUpper(char *str)
{
  if (str == 0) return;

  if (strlen(str) == 0) return;

  int istrlen=strlen(str);

  for (int ii=0;ii<istrlen;ii++)
  {
    if ( (str[ii] >= 'a') && (str[ii] <= 'z') ) str[ii]=str[ii] - 32;
  }
}

void ToUpper(string &str)
{
  if (str.empty()) return;

  char strtemp[str.size()+1];

  memset(strtemp,0,sizeof(strtemp));
  strcpy(strtemp,str.c_str());

  ToUpper(strtemp);

  str=strtemp;

  return;
}

/************************************************************************************/

void *accept_request(void *arg)
//void accept_request(int client)
{
  pthread_cleanup_push(pthmainexit,arg);  // �����߳���������

  pthread_detach(pthread_self());  // �����̡߳�

  pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);  // ����ȡ����ʽΪ����ȡ����

  int client = (long) arg;

  char buf[1024];
  int numchars;
  char method[255];
  char url[255];
  char path[512];
  size_t i, j;
  struct stat st;
  int cgi = 0;
    
  char *query_string = NULL;

  numchars = get_line(client, buf, sizeof(buf));
  logfile.Write(GREEN("%s"),buf);

  i = 0; j = 0;
  while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
  {
    method[i] = buf[j];
    i++; j++;
  }
  method[i] = '\0';

  if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
  {
     unimplemented(client);
  }

  if (strcasecmp(method, "POST") == 0)
     cgi = 1;

  i = 0;
  while (ISspace(buf[j]) && (j < sizeof(buf)))
    j++;
  while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
  {
    url[i] = buf[j];
    i++; j++;
  }
  url[i] = '\0';

  if (strcasecmp(method, "GET") == 0)
  {
     query_string = url;
     while ((*query_string != '?') && (*query_string != '\0'))
     query_string++;
     if (*query_string == '?')
     {
       cgi = 1;
       *query_string = '\0';
       query_string++;
     }
  }

  sprintf(path, "demo%s", url);
  if (path[strlen(path) - 1] == '/')
  strcat(path, "index.html");

  if (stat(path, &st) == -1) 
  {
     while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
       numchars = get_line(client, buf, sizeof(buf));
       logfile.Write("%s",buf);
     not_found(client);
  }
  else
  {
     if ((st.st_mode & S_IFMT) == S_IFDIR)
     strcat(path, "/index.html");
     if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) ||(st.st_mode & S_IXOTH) )
        cgi = 1;
     if (!cgi)
       serve_file(client, path);
     else
       execute_cgi(client, path, method, query_string);
  }

  pthread_cleanup_pop(1);

  pthread_exit(0);
}


void bad_request(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  send(client, buf, sizeof(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<P>Your browser sent a bad request,\r\n");
  send(client, buf, sizeof(buf), 0);
  logfile.Write(RED("%s"),buf);
  logfile.Write(RED("\n"));
  sprintf(buf, "such as a POST without a Content-Length.\r\n");
  send(client, buf, sizeof(buf), 0);
  logfile.Write(RED("%s"),buf);
}


void cat(int client, FILE *resource)
{
  char buf[1024];

  fgets(buf, sizeof(buf), resource);
    logfile.Write(GREEN("%s"),buf);
  while (!feof(resource))
  {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
    logfile.Write(GREEN("%s"),buf);
  }
}

void cannot_execute(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
}


void execute_cgi(int client, const char *path, const char *method,\
                 const char *query_string)
{
  char buf[1024];
  int pipefd[2];
  pid_t pid;
  int status;
  int i;
  int numchars = 1;
  int content_length = -1;
  char readbuf[512];

  buf[0] = 'A'; buf[1] = '\0';
  if (strcasecmp(method, "GET") == 0)
  {
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
      numchars = get_line(client, buf, sizeof(buf));
  }
  else    /* POST */
  {
    numchars = get_line(client, buf, sizeof(buf));
    logfile.Write("%s",buf);

    while ((numchars > 0) && strcmp("\n", buf))
    {
      if (strncmp(buf, "Content-Length:",15) == 0)
      content_length = atoi(buf+15); 
      numchars = get_line(client, buf, sizeof(buf));
      logfile.Write("%s",buf);
    }

    if (content_length == -1) 
    {
      bad_request(client);
      return;
    }
    else
    {
      memset(readbuf,0,sizeof(readbuf));
      read(client,readbuf,sizeof(readbuf));
      logfile.Write(BLUE("%s\n"),readbuf);
    } 
   
  }

  if(pipe(pipefd)==-1)
  {
    printf("ERROR : ���������ܵ�ʧ�ܣ�ʧ�ܺ� %d\n", errno);
    return;
  }

  headers(client,path);

  pid = fork();

  if (pid == 0)  /* child: CGI script */
  {
     dup2(pipefd[1],1);
     execl(path,path,readbuf,NULL);
  } 
  else 
  {  
    char sendData[1024];
    memset(sendData,0,sizeof(sendData));
    int readLength = 0;
    do{
        readLength = read(pipefd[0], sendData, sizeof(sendData));
        if(readLength==0)break;
        write(client, sendData, readLength);
        logfile.Write(GREEN("%s\n"),sendData);
        }while(readLength==sizeof(sendData));
    waitpid(pid, &status, 0);
  }
}


int get_line(int sock, char *buf, int size)
{
  int i = 0;
  char c = '\0';
  int n;

  while ((i < size - 1) && (c != '\n'))
  {
    n = recv(sock, &c, 1, 0);
  /* DEBUG printf("%02X\n", c); */
    if (n > 0)
    {
      if (c == '\r')
      {
         n = recv(sock, &c, 1, MSG_PEEK);
    /* DEBUG printf("%02X\n", c); */
         if ((n > 0) && (c == '\n'))
           recv(sock, &c, 1, 0);
         else
         c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
      c = '\n';
  }
  buf[i] = '\0';
 
  return i;
}


void headers(int client, const char *filename)
{
  char buf[1024];
  (void)filename;  
  //char strtime[20];LocalTime(strtime); 

  logfile.Write("\n");
  strcpy(buf, "HTTP/1.0 200 OK\r\n");
  logfile.Write(GREEN("%s"),buf);
  send(client, buf, strlen(buf), 0);
  strcpy(buf, SERVER_STRING);
  logfile.Write(GREEN("%s"),buf);
  send(client, buf, strlen(buf), 0);
  //sprintf(buf,"%s",strtime);
  //logfile.Write(GREEN("%s"),buf);
  //logfile.Write(GREEN("\n"));
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  logfile.Write(GREEN("%s"),buf);
  send(client, buf, strlen(buf), 0);
  strcpy(buf, "\r\n");
  logfile.Write(GREEN("%s"),buf);
  send(client, buf, strlen(buf), 0);
}


void not_found(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
}

void serve_file(int client, const char *filename)
{
  FILE *resource = NULL;
  int numchars = 1;
  char buf[1024];

  buf[0] = 'A'; buf[1] = '\0';
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
  {
    numchars = get_line(client, buf, sizeof(buf));
    logfile.Write("%s",buf);
  }

  resource = fopen(filename, "r");
  if (resource == NULL)
     not_found(client);
  else
  {
    headers(client, filename);
    cat(client, resource);
  }
  fclose(resource);
}


void unimplemented(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
  logfile.Write(RED("%s"),buf);
}


void pthmainexit(void *arg)
{
  close((int)(long)arg);
} 
